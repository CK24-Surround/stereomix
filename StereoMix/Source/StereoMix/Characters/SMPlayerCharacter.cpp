// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SMTeamComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Data/SMCharacterAssetData.h"
#include "Data/SMControlData.h"
#include "Data/SMDesignData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/SMPlayerController.h"
#include "Player/SMPlayerState.h"
#include "UI/SMUserWidget_CharacterState.h"
#include "UI/SMWidgetComponent.h"
#include "Utilities/SMAssetPath.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Components/SMCatchInteractionComponent_Character.h"

ASMPlayerCharacter::ASMPlayerCharacter()
{
	static ConstructorHelpers::FObjectFinder<USMDesignData> StereoMixDesignDataRef(SMAssetPath::DesignData);
	if (StereoMixDesignDataRef.Object)
	{
		DesignData = StereoMixDesignDataRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DesignData 로드에 실패했습니다."));
	}

	GetMesh()->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionProfileName(SMCollisionProfileName::Player);
	HitBox->InitSphereRadius(100.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	InitCamera();

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));

	CatchInteractionComponent = CreateDefaultSubobject<USMCatchInteractionComponent_Character>(TEXT("CatchInteractionComponent"));

	CharacterStateWidgetComponent = CreateDefaultSubobject<USMWidgetComponent>(TEXT("CharacterStateWidget"));
	CharacterStateWidgetComponent->SetupAttachment(GetMesh());
	CharacterStateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	CharacterStateWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, 300.0));
	CharacterStateWidgetComponent->SetDrawAtDesiredSize(true);

	DefaultMoveTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MoveTrailFXComponent"));
	DefaultMoveTrailFXComponent->SetupAttachment(GetMesh());
	DefaultMoveTrailFXComponent->SetAbsolute(false, true, true);
	DefaultMoveTrailFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	DefaultMoveTrailFXComponent->SetAutoActivate(false);

	CatchMoveTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CatchMoveTrailFXComponent"));
	CatchMoveTrailFXComponent->SetupAttachment(GetMesh());
	CatchMoveTrailFXComponent->SetAbsolute(false, true, true);
	CatchMoveTrailFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	CatchMoveTrailFXComponent->SetAutoActivate(false);

	ImmuneMoveTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ImmuneMoveTrailFXComponent"));
	ImmuneMoveTrailFXComponent->SetupAttachment(GetMesh());
	ImmuneMoveTrailFXComponent->SetAbsolute(false, true, true);
	ImmuneMoveTrailFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	ImmuneMoveTrailFXComponent->SetAutoActivate(false);

	MaxWalkSpeed = 0.0f;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	DefaultMoveTrailFX.FindOrAdd(ESMTeam::None, nullptr);
	DefaultMoveTrailFX.FindOrAdd(ESMTeam::EDM, nullptr);
	DefaultMoveTrailFX.FindOrAdd(ESMTeam::FutureBass, nullptr);

	CatchMoveTrailFX.FindOrAdd(ESMTeam::None, nullptr);
	CatchMoveTrailFX.FindOrAdd(ESMTeam::EDM, nullptr);
	CatchMoveTrailFX.FindOrAdd(ESMTeam::FutureBass, nullptr);

	ImmuneMoveTrailFX.FindOrAdd(ESMTeam::None, nullptr);
	ImmuneMoveTrailFX.FindOrAdd(ESMTeam::EDM, nullptr);
	ImmuneMoveTrailFX.FindOrAdd(ESMTeam::FutureBass, nullptr);
}

void ASMPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 트레일 위치를 교정하기 위해 재어태치합니다. 재어태치하는 이유는 생성자에서는 메시가 null이므로 소켓을 찾을 수 없기 때문입니다.
	DefaultMoveTrailFXComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	CatchMoveTrailFXComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	ImmuneMoveTrailFXComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);

	if (!HasAuthority())
	{
		OriginMaterials = GetMesh()->GetMaterials();
	}

	TeamComponent->OnChangeTeam.AddDynamic(this, &ASMPlayerCharacter::OnTeamChangeCallback);
}

void ASMPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CachedStereoMixPlayerController = GetController<ASMPlayerController>();
	check(CachedStereoMixPlayerController);

	InitASC();
}

void ASMPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMPlayerCharacter, MaxWalkSpeed);
	DOREPLIFETIME(ASMPlayerCharacter, bEnableCollision);
	DOREPLIFETIME(ASMPlayerCharacter, bUseControllerRotation);
	DOREPLIFETIME(ASMPlayerCharacter, bEnableMovement);
	DOREPLIFETIME(ASMPlayerCharacter, LastAttackInstigator);
	DOREPLIFETIME(ASMPlayerCharacter, CharacterMoveTrailState);
}

void ASMPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	CachedStereoMixPlayerController = GetController<ASMPlayerController>();
	check(CachedStereoMixPlayerController);
}

void ASMPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!ASC.Get())
	{
		return;
	}

	ASC->OnChangedTag.RemoveAll(this);
}

void ASMPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CatchInteractionComponent->OnCatch.AddUObject(this, &ThisClass::OnCatch);
	CatchInteractionComponent->OnCatchRelease.AddUObject(this, &ThisClass::OnCatchRelease);
}

void ASMPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		FocusToCursor();
	}
}

void ASMPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	const ASMPlayerController* PlayerController = CastChecked<ASMPlayerController>(Controller);
	const USMControlData* ControlData = PlayerController->GetControlData();
	if (ControlData)
	{
		EnhancedInputComponent->BindAction(ControlData->MoveAction, ETriggerEvent::Triggered, this, &ASMPlayerCharacter::Move);
	}

	SetupGASInputComponent();
}

UAbilitySystemComponent* ASMPlayerCharacter::GetAbilitySystemComponent() const
{
	return ASC.Get();
}

void ASMPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitASC();

	USMUserWidget_CharacterState* NewWidget = CreateWidget<USMUserWidget_CharacterState>(GetWorld(), AssetData->CharacterStateWidget[TeamComponent->GetTeam()]);
	CharacterStateWidgetComponent->SetWidget(NewWidget);
}

void ASMPlayerCharacter::InitCamera()
{
	const FRotator CameraRotation(-50.0f, 0.0, 0.0);
	const float CameraDistance = 1800.0f;
	const FVector CameraTargetOffset(-50.0, 0.0, 0.0);
	const float CameraFOV = 75.0f;

	CameraBoom->SetRelativeRotation(CameraRotation);
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->TargetArmLength = CameraDistance;
	CameraBoom->TargetOffset = CameraTargetOffset;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;

	Camera->SetFieldOfView(CameraFOV);
}

ESMTeam ASMPlayerCharacter::GetTeam() const
{
	return TeamComponent->GetTeam();
}

USMCatchInteractionComponent* ASMPlayerCharacter::GetCatchInteractionComponent()
{
	return CatchInteractionComponent;
}

void ASMPlayerCharacter::SetupGASInputComponent()
{
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	const ASMPlayerController* PlayerController = CastChecked<ASMPlayerController>(Controller);
	const USMControlData* ControlData = PlayerController->GetControlData();
	EnhancedInputComponent->BindAction(ControlData->ShootAction, ETriggerEvent::Triggered, this, &ASMPlayerCharacter::GAInputPressed, EActiveAbility::Launch);
	EnhancedInputComponent->BindAction(ControlData->CatchAction, ETriggerEvent::Triggered, this, &ASMPlayerCharacter::GAInputPressed, EActiveAbility::Catch);
	EnhancedInputComponent->BindAction(ControlData->SmashAction, ETriggerEvent::Triggered, this, &ASMPlayerCharacter::GAInputPressed, EActiveAbility::Smash);
}

void ASMPlayerCharacter::InitASC()
{
	ASMPlayerState* StereoMixPlayerState = GetPlayerState<ASMPlayerState>();
	if (!ensureAlways(StereoMixPlayerState))
	{
		return;
	}

	ASC = Cast<USMAbilitySystemComponent>(StereoMixPlayerState->GetAbilitySystemComponent());
	if (!ensureAlways(ASC.Get()))
	{
		return;
	}

	CatchInteractionComponent->InitASC(ASC.Get());

	if (HasAuthority())
	{
		ASC->InitAbilityActorInfo(StereoMixPlayerState, this);
		GiveDefaultAbilities();
	}

	ASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ThisClass::OnChangeMoveSpeed);
	const USMCharacterAttributeSet* AttributeSet = ASC->GetSet<USMCharacterAttributeSet>();
	if (!ensureAlways(AttributeSet))
	{
		return;
	}

	FOnAttributeChangeData NewData;
	NewData.NewValue = AttributeSet->GetMoveSpeed();
	OnChangeMoveSpeed(NewData);
}

void ASMPlayerCharacter::GiveDefaultAbilities()
{
	if (!HasAuthority())
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
		const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(ForInitGE, 0, GEContextHandle);
		if (GESpecHandle.IsValid())
		{
			GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::Character::Init::MoveSpeed, DesignData->MoveSpeed);
			ASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
		}
	}

	for (const auto& DefaultActiveAbility : DefaultActiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(DefaultActiveAbility.Value, 1, static_cast<int32>(DefaultActiveAbility.Key));
		ASC->GiveAbility(AbilitySpec);
	}

	for (const auto& DefaultAbility : DefaultAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(DefaultAbility);
		ASC->GiveAbility(AbilitySpec);
	}
}

void ASMPlayerCharacter::GAInputPressed(EActiveAbility InInputID)
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

void ASMPlayerCharacter::GAInputReleased(EActiveAbility InInputID)
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

void ASMPlayerCharacter::MulticastRPCApplyAttachOffset_Implementation()
{
	// 메시의 오프셋만큼 다시 회복시켜주는 로직입니다.
	FVector MeshLocation = GetMesh()->GetRelativeLocation();
	FRotator MeshRotation = GetMesh()->GetRelativeRotation();

	SetActorRelativeLocation(-MeshLocation);
	SetActorRelativeRotation(FRotator::ZeroRotator - MeshRotation);
}

void ASMPlayerCharacter::Move(const FInputActionValue& InputActionValue)
{
	const UAbilitySystemComponent* CachedASC = ASC.Get();
	if (!CachedASC)
	{
		return;
	}

	if (CachedASC->HasAnyMatchingGameplayTags(LockMovementTags))
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

void ASMPlayerCharacter::FocusToCursor()
{
	if (!Controller)
	{
		return;
	}

	const USMAbilitySystemComponent* CachedASC = ASC.Get();
	if (CachedASC)
	{
		if (CachedASC->HasAnyMatchingGameplayTags(LockAimTags))
		{
			return;
		}
	}

	const FVector Direction = (GetCursorTargetingPoint() - GetActorLocation()).GetSafeNormal();
	const FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	Controller->SetControlRotation(NewRotation);
}

FVector ASMPlayerCharacter::GetCursorTargetingPoint(bool bIsZeroBasis)
{
	FVector BasisLocation;
	if (bIsZeroBasis)
	{
		BasisLocation = GetActorLocation() - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	else
	{
		BasisLocation = GetActorLocation();
	}

	FVector Result(0.0, 0.0, BasisLocation.Z);

	if (CachedStereoMixPlayerController)
	{
		FVector WorldLocation, WorldDirection;
		CachedStereoMixPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
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

void ASMPlayerCharacter::SetMaxWalkSpeed(float InSpeed)
{
	if (HasAuthority())
	{
		MaxWalkSpeed = InSpeed;
		OnRep_MaxWalkSpeed();
	}
}

void ASMPlayerCharacter::ServerRPCPreventGroundEmbedding_Implementation()
{
	FVector Location = GetActorLocation();
	float CapusleHalfHeight;
	float CapusleRadius;
	GetCapsuleComponent()->GetScaledCapsuleSize(CapusleRadius, CapusleHalfHeight);

	FHitResult HitResult;
	const FVector Start = Location + (FVector::UpVector * CapusleHalfHeight * 2.0f);
	const FVector End = Location + (-FVector::UpVector * CapusleHalfHeight * 2.0f);
	FCollisionObjectQueryParams Params;
	Params.AddObjectTypesToQuery(ECC_WorldStatic);
	Params.AddObjectTypesToQuery(ECC_WorldDynamic);
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(CapusleRadius);
	const bool bSuccess = GetWorld()->SweepSingleByObjectType(HitResult, Start, End, FQuat::Identity, Params, CollisionShape);
	if (bSuccess)
	{
		MulticastRPCSetLocation(HitResult.Location + (FVector::UpVector * CapusleHalfHeight));
	}

	FColor DebugColor = bSuccess ? FColor::Cyan : FColor::Red;
	DrawDebugLine(GetWorld(), Start, End, DebugColor, false, 1.0f);
}

void ASMPlayerCharacter::MulticastRPCSetLocation_Implementation(const FVector_NetQuantize10 InLocation)
{
	SetActorLocation(InLocation);
}

void ASMPlayerCharacter::MulticastRPCSetYawRotation_Implementation(float InYaw)
{
	SetActorRotation(FRotator(0.0, InYaw, 0.0));
}

void ASMPlayerCharacter::OnRep_MaxWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ASMPlayerCharacter::OnChangeMoveSpeed(const FOnAttributeChangeData& OnAttributeChangeData)
{
	GetCharacterMovement()->MaxWalkSpeed = OnAttributeChangeData.NewValue;
}

void ASMPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnLanded.Broadcast();
}

void ASMPlayerCharacter::SetCharacterStateVisibility_Implementation(bool bEnable)
{
	CharacterStateWidgetComponent->SetVisibility(bEnable);
}

void ASMPlayerCharacter::SetEnableCollision(bool bInEnableCollision)
{
	if (HasAuthority())
	{
		bEnableCollision = bInEnableCollision;
		OnRep_EnableCollision();
	}
}

void ASMPlayerCharacter::SetUseControllerRotation(bool bInUseControllerRotation)
{
	if (HasAuthority())
	{
		bUseControllerRotation = bInUseControllerRotation;
		OnRep_UseControllerRotation();
	}
}

void ASMPlayerCharacter::SetEnableMovement(bool bInEnableMovementMode)
{
	if (HasAuthority())
	{
		bEnableMovement = bInEnableMovementMode;
		OnRep_EnableMovement();
	}
}

void ASMPlayerCharacter::OnRep_EnableCollision()
{
	SetActorEnableCollision(bEnableCollision);
}

void ASMPlayerCharacter::OnRep_UseControllerRotation()
{
	bUseControllerRotationPitch = bUseControllerRotation;
	bUseControllerRotationYaw = bUseControllerRotation;
	bUseControllerRotationRoll = bUseControllerRotation;
}

void ASMPlayerCharacter::OnRep_EnableMovement()
{
	if (bEnableMovement)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
}

void ASMPlayerCharacter::OnCatch()
{
	if (HasAuthority())
	{
		SetCharacterMoveTrailState(ECharacterMoveTrailState::Catch);
	}
}

void ASMPlayerCharacter::OnCatchRelease()
{
	if (HasAuthority())
	{
		SetCharacterMoveTrailState(ECharacterMoveTrailState::Default);
	}
}

void ASMPlayerCharacter::OnTeamChangeCallback()
{
	const ESMTeam Team = TeamComponent->GetTeam();

	// 팀이 바뀔일은 없어야하지만 테스트 시에는 충분히 있는 상황으로 이에 대한 처리입니다.
	if (!HasAuthority())
	{
		GetMesh()->SetMaterial(0, AssetData->HairRingMaterial[Team]);
		GetMesh()->SetMaterial(2, AssetData->HairFrontMaterial[Team]);
		GetMesh()->SetMaterial(3, AssetData->HairTailMaterial[Team]);

		// 기본 머티리얼을 다시 업데이트 해줍니다.
		OriginMaterials = GetMesh()->GetMaterials();

		if (ASC.Get())
		{
			USMUserWidget_CharacterState* NewWidget = CreateWidget<USMUserWidget_CharacterState>(GetWorld(), AssetData->CharacterStateWidget[TeamComponent->GetTeam()]);
			CharacterStateWidgetComponent->SetWidget(NewWidget);
		}

		DefaultMoveTrailFXComponent->SetAsset(DefaultMoveTrailFX[Team]);
		DefaultMoveTrailFXComponent->Activate(true);

		CatchMoveTrailFXComponent->SetAsset(CatchMoveTrailFX[Team]);
		ImmuneMoveTrailFXComponent->SetAsset(ImmuneMoveTrailFX[Team]);
	}
}

bool ASMPlayerCharacter::bAmICatching()
{
	return CatchInteractionComponent->GetActorIAmCatching() != nullptr;
}

void ASMPlayerCharacter::SetCharacterMoveTrailState(ECharacterMoveTrailState NewCharacterMoveTrailState)
{
	if (!HasAuthority())
	{
		NET_LOG(this, Warning, TEXT("서버에서 실행되어야합니다. "));
		return;
	}

	if (CharacterMoveTrailState == NewCharacterMoveTrailState)
	{
		return;
	}

	CharacterMoveTrailState = NewCharacterMoveTrailState;
}

void ASMPlayerCharacter::OnRep_CharacterMoveTrailState()
{
	DeactivateAllMoveTrail();

	switch (CharacterMoveTrailState)
	{
		case ECharacterMoveTrailState::None:
		{
			break;
		}
		case ECharacterMoveTrailState::Default:
		{
			ActivateDefaultMoveTrail();
			break;
		}
		case ECharacterMoveTrailState::Catch:
		{
			ActivateCatchMoveTrail();
			break;
		}
		case ECharacterMoveTrailState::Immune:
		{
			ActivateImmuneMoveTrail();
			break;
		}
	}
}

void ASMPlayerCharacter::DeactivateAllMoveTrail()
{
	DefaultMoveTrailFXComponent->DeactivateImmediate();
	CatchMoveTrailFXComponent->DeactivateImmediate();
	ImmuneMoveTrailFXComponent->DeactivateImmediate();
}

void ASMPlayerCharacter::ActivateDefaultMoveTrail()
{
	DefaultMoveTrailFXComponent->Activate(true);
}

void ASMPlayerCharacter::ActivateCatchMoveTrail()
{
	CatchMoveTrailFXComponent->Activate(true);
}

void ASMPlayerCharacter::ActivateImmuneMoveTrail()
{
	ImmuneMoveTrailFXComponent->Activate(true);
}
