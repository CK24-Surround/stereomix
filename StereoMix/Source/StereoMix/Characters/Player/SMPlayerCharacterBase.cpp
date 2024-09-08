// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SMTeamComponent.h"
#include "Components/WidgetComponent.h"
#include "Controllers/SMGamePlayerController.h"
#include "Data/SMControlData.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Games/SMGamePlayerState.h"
#include "HoldInteraction/SMHIC_Character.h"
#include "HoldInteraction/SMHoldInteractionComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/Game/SMUserWidget_CharacterState.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMPlayerCharacterBase::ASMPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;

	USkeletalMeshComponent* CachedMeshComponent = GetMesh();
	CachedMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	CachedMeshComponent->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	UCharacterMovementComponent* CachedMovementComponent = GetCharacterMovement();
	CachedMovementComponent->MaxAcceleration = 9999.0f;
	CachedMovementComponent->MaxStepHeight = 10.0f;
	CachedMovementComponent->SetWalkableFloorAngle(5.0f);
	CachedMovementComponent->bCanWalkOffLedges = false;

	HitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionProfileName(SMCollisionProfileName::Player);
	HitBox->InitCapsuleSize(125.0f, 125.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(-50.0f, 0.0, 0.0));
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->TargetArmLength = 1800.0f;
	CameraBoom->TargetOffset = FVector(-50.0, 0.0, 0.0);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	Camera->SetFieldOfView(75.0f);

	ListenerComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ListenerComponent"));
	ListenerComponent->SetAbsolute(false, true);
	ListenerComponent->SetupAttachment(CameraBoom);

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));

	CharacterStateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CharacterStateWidgetComponent"));
	CharacterStateWidgetComponent->SetupAttachment(CachedMeshComponent);
	CharacterStateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	CharacterStateWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, 300.0));
	CharacterStateWidgetComponent->SetDrawAtDesiredSize(true);

	DefaultMoveTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DefaultMoveTrailFXComponent"));
	DefaultMoveTrailFXComponent->SetupAttachment(CachedMeshComponent);
	DefaultMoveTrailFXComponent->SetAbsolute(false, true, true);
	DefaultMoveTrailFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	DefaultMoveTrailFXComponent->SetAutoActivate(false);

	CatchMoveTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CatchMoveTrailFXComponent"));
	CatchMoveTrailFXComponent->SetupAttachment(CachedMeshComponent);
	CatchMoveTrailFXComponent->SetAbsolute(false, true, true);
	CatchMoveTrailFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	CatchMoveTrailFXComponent->SetAutoActivate(false);

	ImmuneMoveTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ImmuneMoveTrailFXComponent"));
	ImmuneMoveTrailFXComponent->SetupAttachment(CachedMeshComponent);
	ImmuneMoveTrailFXComponent->SetAbsolute(false, true, true);
	ImmuneMoveTrailFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	ImmuneMoveTrailFXComponent->SetAutoActivate(false);

	HIC = CreateDefaultSubobject<USMHIC_Character>(TEXT("HIC"));

	LockAimTags.AddTag(SMTags::Character::State::Caught);
	LockAimTags.AddTag(SMTags::Character::State::NoiseBreaking);
	LockAimTags.AddTag(SMTags::Character::State::NoiseBreaked);
	LockAimTags.AddTag(SMTags::Character::State::Neutralize);

	LockMovementTags.AddTag(SMTags::Character::State::Caught);
	LockMovementTags.AddTag(SMTags::Character::State::NoiseBreaking);
	LockMovementTags.AddTag(SMTags::Character::State::NoiseBreaked);
	LockMovementTags.AddTag(SMTags::Character::State::Neutralize);
	LockMovementTags.AddTag(SMTags::Character::State::Jump);
}

void ASMPlayerCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsHiddenCharacter);
	DOREPLIFETIME(ThisClass, bActivateCollision);
	DOREPLIFETIME(ThisClass, bEnableMovement);
	DOREPLIFETIME(ThisClass, bUseControllerRotation);
	DOREPLIFETIME(ThisClass, LastAttackInstigator);
}

void ASMPlayerCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!ensureAlways(DataAsset))
	{
		return;
	}

	UCharacterMovementComponent* CachedMovementComponent = GetCharacterMovement();
	CachedMovementComponent->MaxWalkSpeed = DataAsset->MoveSpeed;

	// 트레일 위치를 교정하기 위해 재어태치합니다. 재어태치하는 이유는 생성자에서는 메시가 null이므로 소켓을 찾을 수 없기 때문입니다.
	DefaultMoveTrailFXComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	CatchMoveTrailFXComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	ImmuneMoveTrailFXComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);

	// 원본 머티리얼을 저장해둡니다. 플레이 도중 시시각각 머티리얼이 변하게 되는데 이때 기존 머티리얼로 돌아오기 위해 사용됩니다.
	if (!HasAuthority())
	{
		OriginalMaterials = GetMesh()->GetMaterials();
		OriginalOverlayMaterial = GetMesh()->GetOverlayMaterial();
	}
}

void ASMPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	const USMControlData* ControlData = SMPlayerController->GetControlData();
	if (ControlData)
	{
		EnhancedInputComponent->BindAction(ControlData->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Triggered, this, &ThisClass::GAInputPressed, EActiveAbility::Attack);
		EnhancedInputComponent->BindAction(ControlData->HoldAction, ETriggerEvent::Triggered, this, &ThisClass::GAInputPressed, EActiveAbility::Hold);
		EnhancedInputComponent->BindAction(ControlData->SkillAction, ETriggerEvent::Triggered, this, &ThisClass::GAInputPressed, EActiveAbility::Skill);
		EnhancedInputComponent->BindAction(ControlData->NoiseBreakAction, ETriggerEvent::Triggered, this, &ThisClass::GAInputPressed, EActiveAbility::NoiseBreak);
	}
}

void ASMPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASMPlayerCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled())
	{
		UpdateCameraLocation();
		UpdateFocusToCursor();
	}
}

void ASMPlayerCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASC.Get())
	{
		ASC->OnChangedTag.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ASMPlayerCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	ASMGamePlayerController* CachedPlayerController = GetController<ASMGamePlayerController>();
	if (ensureAlways(CachedPlayerController))
	{
		SMPlayerController = GetController<ASMGamePlayerController>();
		SMPlayerController->SetAudioListenerOverride(ListenerComponent, FVector::ZeroVector, FRotator::ZeroRotator);
	}
}

void ASMPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASMGamePlayerController* CachedPlayerController = GetController<ASMGamePlayerController>();
	if (ensureAlways(CachedPlayerController))
	{
		SMPlayerController = GetController<ASMGamePlayerController>();
	}

	InitASC();
}

void ASMPlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitASC();

	ESMTeam SourceTeam = GetTeam();

	USMUserWidget_CharacterState* CharacterStateWidget = CreateWidget<USMUserWidget_CharacterState>(GetWorld(), DataAsset->CharacterStateWidget[SourceTeam]);
	if (CharacterStateWidget)
	{
		CharacterStateWidgetComponent->SetWidget(CharacterStateWidget);
		BindCharacterStateWidget(CharacterStateWidget);
	}

	DefaultMoveTrailFXComponent->SetAsset(DataAsset->DefaultMoveTrailFX[SourceTeam]);
	DefaultMoveTrailFXComponent->Activate();

	CatchMoveTrailFXComponent->SetAsset(DataAsset->CatchMoveTrailFX[SourceTeam]);
	CatchMoveTrailFXComponent->Deactivate();

	ImmuneMoveTrailFXComponent->SetAsset(DataAsset->ImmuneMoveTrailFX[SourceTeam]);
	ImmuneMoveTrailFXComponent->Deactivate();
}

UAbilitySystemComponent* ASMPlayerCharacterBase::GetAbilitySystemComponent() const
{
	return ASC.Get();
}

ESMTeam ASMPlayerCharacterBase::GetTeam() const
{
	return TeamComponent->GetTeam();
}

USMHoldInteractionComponent* ASMPlayerCharacterBase::GetHoldInteractionComponent() const
{
	return HIC;
}

void ASMPlayerCharacterBase::PredictHPChange(float Amount)
{
	USMUserWidget_CharacterState* StateWidget = Cast<USMUserWidget_CharacterState>(CharacterStateWidgetComponent->GetWidget());
	if (!ensureAlways(StateWidget))
	{
		return;
	}

	FOnAttributeChangeData HPAttributeChangeData;
	HPAttributeChangeData.NewValue = StateWidget->CurrentHealth + Amount;

	if (StateWidget->CurrentHealth > 0.0f)
	{
		StateWidget->OnChangeCurrentHealth(HPAttributeChangeData);
	}
}

FVector ASMPlayerCharacterBase::GetCursorTargetingPoint(bool bIsZeroBasis)
{
	const FVector SourceLocation = GetActorLocation();
	FVector BasisLocation = bIsZeroBasis ? SourceLocation - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : SourceLocation;

	FVector Result(0.0, 0.0, BasisLocation.Z);

	if (SMPlayerController.Get())
	{
		FVector WorldLocation;
		FVector WorldDirection;
		SMPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		const FPlane Plane(BasisLocation, FVector::UpVector);
		const FVector IntersectionPoint = FMath::RayPlaneIntersection(WorldLocation, WorldDirection, Plane);
		if (!IntersectionPoint.ContainsNaN())
		{
			Result = IntersectionPoint;
		}
	}

	// 만약 커서위치와 평면의 위치가 평면상에 놓이지 않았다면 경고를 표시합니다. 일반적으로 이런상황이 존재하지는 않을 것으로 예상됩니다.
	if (!FMath::IsNearlyEqual(BasisLocation.Z, Result.Z, KINDA_SMALL_NUMBER))
	{
		NET_LOG(this, Warning, TEXT("캐릭터의 위치와 커서의 위치가 평면상에 놓이지 않았습니다. 캐릭터 Z값: %f 커서 Z값: %f"), BasisLocation.Z, Result.Z);
	}

	return Result;
}

void ASMPlayerCharacterBase::SetCharacterHidden(bool bIsEnable)
{
	if (!HasAuthority())
	{
		return;
	}

	bIsHiddenCharacter = bIsEnable;
	OnRep_bIsHiddenCharacter();
}

void ASMPlayerCharacterBase::SetCollisionEnable(bool bIsEnable)
{
	if (!HasAuthority())
	{
		return;
	}

	bActivateCollision = bIsEnable;
	OnRep_bActivateCollision();
}

void ASMPlayerCharacterBase::SetMovementEnable(bool bIsEnable)
{
	if (!HasAuthority())
	{
		return;
	}

	bEnableMovement = bIsEnable;
	OnRep_bEnableMovement();
}

void ASMPlayerCharacterBase::SetUseControllerRotation(bool bNewUseControllerRotation)
{
	if (!HasAuthority())
	{
		return;
	}

	bUseControllerRotation = true;
	OnRep_bUseControllerRotation();
}

void ASMPlayerCharacterBase::MulticastRPCAddScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	if (HasAuthority())
	{
		return;
	}

	// 자기 자신은 제외합니다.
	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (TargetCharacter->IsLocallyControlled())
	{
		return;
	}

	// 로컬 컨트롤러를 찾고 스크린 인디케이터를 추가해줍니다.
	ASMGamePlayerController* LocalPlayerController = Cast<ASMGamePlayerController>(GetWorld()->GetFirstPlayerController());
	if (ensureAlways(LocalPlayerController))
	{
		LocalPlayerController->AddScreendIndicator(TargetActor);
	}
}

void ASMPlayerCharacterBase::MulticastRPCRemoveScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	if (HasAuthority())
	{
		return;
	}

	// 자기 자신은 제외합니다.
	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (TargetCharacter->IsLocallyControlled())
	{
		return;
	}

	// 로컬 컨트롤러를 찾고 스크린 인디케이터를 제거합니다.
	ASMGamePlayerController* LocalPlayerController = Cast<ASMGamePlayerController>(GetWorld()->GetFirstPlayerController());
	if (ensureAlways(LocalPlayerController))
	{
		LocalPlayerController->RemoveScreenIndicator(TargetActor);
	}
}

void ASMPlayerCharacterBase::ClientRPCRemoveScreendIndicatorToSelf_Implementation(AActor* TargetActor)
{
	// ClientRPC로 무조건 오너십을 갖고 있어 CachedSMPlayerController가 유효하겠지만 만약을 위한 예외처리입니다.
	if (ensureAlways(SMPlayerController.Get()))
	{
		SMPlayerController->RemoveScreenIndicator(TargetActor);
	}
}

void ASMPlayerCharacterBase::SetCharacterStateVisibility_Implementation(bool bNewVisibility)
{
	CharacterStateWidgetComponent->SetVisibility(bNewVisibility);
}

void ASMPlayerCharacterBase::MulticastRPCSetYawRotation_Implementation(float InYaw)
{
	SetActorRotation(FRotator(0.0, InYaw, 0.0));
}

void ASMPlayerCharacterBase::MulitcastRPCSetLocation_Implementation(const FVector_NetQuantize10& NewLocation)
{
	SetActorLocation(NewLocation);
}

void ASMPlayerCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnCharacterLanded.Broadcast(this);
}

void ASMPlayerCharacterBase::FocusToCursor()
{
	if (!SMPlayerController.Get())
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Yellow, TEXT("SMPlayerController가 무효합니다."));
		return;
	}

	const FVector Direction = (GetCursorTargetingPoint() - GetActorLocation()).GetSafeNormal();
	const FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	Controller->SetControlRotation(NewRotation);
}

void ASMPlayerCharacterBase::Move(const FInputActionValue& InputActionValue)
{
	if (!ASC.Get())
	{
		NET_LOG(this, Warning, TEXT("ASC가 유효하지 않습니다."));
		return;
	}

	if (ASC->HasAnyMatchingGameplayTags(LockMovementTags))
	{
		return;
	}

	const FVector2D InputScalar = InputActionValue.Get<FVector2D>().GetSafeNormal();
	const FRotator CameraYawRotation(0.0, Camera->GetComponentRotation().Yaw, 0.0);
	const FVector ForwardDirection = FRotationMatrix(CameraYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(CameraYawRotation).GetUnitAxis(EAxis::Y);
	const FVector MoveVector = (ForwardDirection * InputScalar.X) + (RightDirection * InputScalar.Y);

	AddMovementInput(MoveVector);
}

void ASMPlayerCharacterBase::InitASC()
{
	ASMGamePlayerState* StereoMixPlayerState = GetPlayerState<ASMGamePlayerState>();
	if (!ensureAlways(StereoMixPlayerState))
	{
		return;
	}

	ASC = Cast<USMAbilitySystemComponent>(StereoMixPlayerState->GetAbilitySystemComponent());
	if (!ensureAlways(ASC.Get()))
	{
		return;
	}

	HIC->InitASC(ASC.Get());

	if (HasAuthority())
	{
		ASC->ClearAllAbilities();
		ASC->InitAbilityActorInfo(StereoMixPlayerState, this);
		GiveDefaultAbilities();
	}

	TeamComponent->SetTeam(StereoMixPlayerState->GetTeam());
}

void ASMPlayerCharacterBase::GiveDefaultAbilities()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ensureAlways(DataAsset))
	{
		return;
	}

	if (!ensureAlways(ASC.Get()))
	{
		return;
	}

	const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	if (GEContextHandle.IsValid())
	{
		const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(DataAsset->ForInitGE, 0, GEContextHandle);
		if (GESpecHandle.IsValid())
		{
			GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::Character::Init::MoveSpeed, DataAsset->MoveSpeed);
			GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::Character::Init::MaxHP, DataAsset->MaxHP);
			GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::Character::Init::CurrentHP, DataAsset->MaxHP);
			ASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
		}
	}

	for (const auto& DefaultActiveAbility : DataAsset->DefaultActiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(DefaultActiveAbility.Value, 1, static_cast<int32>(DefaultActiveAbility.Key));
		ASC->GiveAbility(AbilitySpec);
	}

	for (const auto& DefaultAbility : DataAsset->DefaultAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(DefaultAbility);
		ASC->GiveAbility(AbilitySpec);
	}

	for (const auto& PassiveAbility : DataAsset->PassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(PassiveAbility);
		ASC->GiveAbility(AbilitySpec);
		ASC->TryActivateAbility(AbilitySpec.Handle);
	}
}

void ASMPlayerCharacterBase::GAInputPressed(EActiveAbility InInputID)
{
	if (!ASC.Get())
	{
		NET_LOG(this, Warning, TEXT("ASC가 유효하지 않습니다."));
		return;
	}

	FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InInputID));
	if (GASpec)
	{
		if (GASpec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*GASpec);
		}
		else
		{
			ASC->TryActivateAbility(GASpec->Handle);
		}
	}
}

void ASMPlayerCharacterBase::GAInputReleased(EActiveAbility InInputID)
{
	if (!ASC.Get())
	{
		NET_LOG(this, Warning, TEXT("ASC가 유효하지 않습니다."));
		return;
	}

	FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InInputID));
	if (GASpec)
	{
		if (GASpec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*GASpec);
		}
	}
}

void ASMPlayerCharacterBase::UpdateCameraLocation()
{
	if (!GEngine)
	{
		return;
	}

	if (!SMPlayerController.Get())
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Yellow, TEXT("SMPlayerController가 무효합니다."));
		return;
	}

	FVector2D MouseScreenLocation;
	if (!SMPlayerController->GetMousePosition(MouseScreenLocation.X, MouseScreenLocation.Y))
	{
		return;
	}

	UGameViewportClient* GameViewport = GEngine->GameViewport;
	if (!GameViewport)
	{
		return;
	}

	const FVector SourceLocation = GetActorLocation();
	const FVector MouseLocation = GetCursorTargetingPoint();
	FVector TargetLocation = (SourceLocation + MouseLocation) * 0.5;

	const FVector SourceToTargetVector = TargetLocation - SourceLocation;
	float Distance = SourceToTargetVector.Size();

	const float MaxMouseRange = CameraMoveMouseThreshold / 2.0f;
	float MoveDistance = FMath::Clamp((Distance / MaxMouseRange) * CameraMoveMaxDistance, 0.0f, CameraMoveMaxDistance);
	if (MoveDistance > 0)
	{
		const FVector AdjustVector = SourceToTargetVector.GetSafeNormal() * MoveDistance;
		TargetLocation = SourceLocation + AdjustVector;
	}
	else
	{
		TargetLocation = SourceLocation;
	}

	CameraBoom->SetWorldLocation(TargetLocation);
}

void ASMPlayerCharacterBase::UpdateFocusToCursor()
{
	const USMAbilitySystemComponent* CachedASC = ASC.Get();
	if (CachedASC)
	{
		if (CachedASC->HasAnyMatchingGameplayTags(LockAimTags))
		{
			return;
		}
	}

	FocusToCursor();
}

void ASMPlayerCharacterBase::BindCharacterStateWidget(USMUserWidget_CharacterState* CharacterStateWidget)
{
	if (!ensureAlways(CharacterStateWidget))
	{
		return;
	}

	if (!ensureAlways(ASC.Get()))
	{
		return;
	}

	APlayerState* CachedPlayerState = GetPlayerState();
	if (ensureAlways(CachedPlayerState))
	{
		CharacterStateWidget->SetNickname(CachedPlayerState->GetPlayerName());

		ASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetPostureGaugeAttribute()).AddUObject(CharacterStateWidget, &USMUserWidget_CharacterState::OnChangeCurrentHealth);
		ASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetMaxPostureGaugeAttribute()).AddUObject(CharacterStateWidget, &USMUserWidget_CharacterState::OnChangeMaxHealth);

		const USMCharacterAttributeSet* SourceAttributeSet = ASC->GetSet<USMCharacterAttributeSet>();
		if (ensureAlways(SourceAttributeSet))
		{
			CharacterStateWidget->MaxHealth = SourceAttributeSet->GetPostureGauge();
			CharacterStateWidget->CurrentHealth = SourceAttributeSet->GetMaxPostureGauge();
			CharacterStateWidget->UpdateHPBar();
		}
	}
}

void ASMPlayerCharacterBase::OnRep_bIsHiddenCharacter()
{
	SetActorHiddenInGame(bIsHiddenCharacter);
}

void ASMPlayerCharacterBase::OnRep_bActivateCollision()
{
	SetActorEnableCollision(bActivateCollision);
}

void ASMPlayerCharacterBase::OnRep_bEnableMovement()
{
	UCharacterMovementComponent* SourceMovementComponent = GetCharacterMovement();
	if (!SourceMovementComponent)
	{
		return;
	}

	if (bEnableMovement)
	{
		SourceMovementComponent->SetMovementMode(MOVE_Walking);
	}
	else
	{
		SourceMovementComponent->SetMovementMode(MOVE_None);
	}
}

void ASMPlayerCharacterBase::OnRep_bUseControllerRotation()
{
	bUseControllerRotationPitch = bUseControllerRotation;
	bUseControllerRotationYaw = bUseControllerRotation;
	bUseControllerRotationRoll = bUseControllerRotation;
}
