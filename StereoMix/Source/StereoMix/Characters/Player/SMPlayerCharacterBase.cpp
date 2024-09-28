// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Characters/Component/SMCharacterMovementComponent.h"
#include "Characters/Component/SMHIC_Character.h"
#include "Characters/Weapon/SMWeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SMTeamComponent.h"
#include "Components/WidgetComponent.h"
#include "Controllers/SMGamePlayerController.h"
#include "Data/SMControlData.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Games/SMGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Tiles/SMTile.h"
#include "UI/Widget/Game/SMUserWidget_CharacterState.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMPlayerCharacterBase::ASMPlayerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;

	USkeletalMeshComponent* CachedMeshComponent = GetMesh();
	CachedMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	CachedMeshComponent->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	CachedMeshComponent->bRenderCustomDepth = true;
	CachedMeshComponent->bReceivesDecals = false;

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
	CameraBoom->SetRelativeRotation(FRotator(-52.5f, 0.0, 0.0));
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

	NoteMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NoteMeshComponent"));
	NoteMeshComponent->SetupAttachment(CachedMeshComponent);
	NoteMeshComponent->SetAbsolute(false, true, false);
	NoteMeshComponent->SetWorldRotation(FRotator(0.0, 90.0, 0.0));
	NoteMeshComponent->SetVisibility(false);
	NoteMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	NoteMeshComponent->bReceivesDecals = false;

	CharacterStateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CharacterStateWidgetComponent"));
	CharacterStateWidgetComponent->SetupAttachment(CachedMeshComponent);
	CharacterStateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	CharacterStateWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, 300.0));
	CharacterStateWidgetComponent->SetDrawAtDesiredSize(true);

	NoiseBreakIndicatorComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NoiseBreakIndicatorComponent"));
	NoiseBreakIndicatorComponent->SetAbsolute(true, true, true);
	NoiseBreakIndicatorComponent->SetAutoActivate(false);

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
	HIC->OnHoldStateEnrty.AddUObject(this, &ThisClass::OnHoldStateEntry);
	HIC->OnHoldStateExit.AddUObject(this, &ThisClass::OnHoldStateExit);

	LockAimTags.AddTag(SMTags::Character::State::Holded);
	LockAimTags.AddTag(SMTags::Character::State::NoiseBreak);
	LockAimTags.AddTag(SMTags::Character::State::NoiseBreaked);
	LockAimTags.AddTag(SMTags::Character::State::Neutralize);

	LockMovementTags.AddTag(SMTags::Character::State::Holded);
	LockMovementTags.AddTag(SMTags::Character::State::NoiseBreak);
	LockMovementTags.AddTag(SMTags::Character::State::NoiseBreaked);
	LockMovementTags.AddTag(SMTags::Character::State::Neutralize);
	LockMovementTags.AddTag(SMTags::Character::State::Jump);
	LockMovementTags.AddTag(SMTags::Character::State::Stun);

	PushBackImmuneTags.AddTag(SMTags::Character::State::Holded);
	PushBackImmuneTags.AddTag(SMTags::Character::State::NoiseBreak);
	PushBackImmuneTags.AddTag(SMTags::Character::State::NoiseBreaked);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Neutralize);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Immune);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Jump);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Stun);
}

void ASMPlayerCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsActorHidden);
	DOREPLIFETIME(ThisClass, bIsCollisionEnabled);
	DOREPLIFETIME(ThisClass, bIsMovementEnabled);
	DOREPLIFETIME(ThisClass, bUseControllerRotation);
	DOREPLIFETIME(ThisClass, LastAttacker);
	DOREPLIFETIME(ThisClass, Weapon);
	DOREPLIFETIME(ThisClass, bIsNoteState);
}

void ASMPlayerCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!DataAsset)
	{
		return;
	}

	UCharacterMovementComponent* CachedMovementComponent = GetCharacterMovement();
	CachedMovementComponent->MaxWalkSpeed = DataAsset->MoveSpeed;
	CachedMovementComponent->GravityScale = 2.0f;

	// 트레일 위치를 교정하기 위해 재어태치합니다. 재어태치하는 이유는 생성자에서는 메시가 null이므로 소켓을 찾을 수 없기 때문입니다.
	DefaultMoveTrailFXComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	CatchMoveTrailFXComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	ImmuneMoveTrailFXComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);

	// 원본 머티리얼을 저장해둡니다. 플레이 도중 시시각각 머티리얼이 변하게 되는데 이때 기존 머티리얼로 돌아오기 위해 사용됩니다.
	OriginalMaterials = GetMesh()->GetMaterials();
	OriginalOverlayMaterial = GetMesh()->GetOverlayMaterial();

	TeamComponent->OnChangeTeam.AddDynamic(this, &ThisClass::OnTeamChanged);
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
		EnhancedInputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Completed, this, &ThisClass::GAInputReleased, EActiveAbility::Attack);
		EnhancedInputComponent->BindAction(ControlData->HoldAction, ETriggerEvent::Started, this, &ThisClass::GAInputPressed, EActiveAbility::Hold);
		EnhancedInputComponent->BindAction(ControlData->SkillAction, ETriggerEvent::Started, this, &ThisClass::GAInputPressed, EActiveAbility::Skill);
		EnhancedInputComponent->BindAction(ControlData->NoiseBreakAction, ETriggerEvent::Started, this, &ThisClass::GAInputPressed, EActiveAbility::NoiseBreak);
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

	if (Weapon)
	{
		Weapon->Destroy();
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

	const ESMTeam SourceTeam = GetTeam();

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

	NoiseBreakIndicatorComponent->SetAsset(DataAsset->NoiseBreakIndicatorFX[SourceTeam]);
}

void ASMPlayerCharacterBase::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);

	Weapon->SetActorHiddenInGame(bNewHidden);
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
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent();
	if (!SourceASC)
	{
		return;
	}

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

FVector ASMPlayerCharacterBase::GetLocationFromCursor(bool bUseZeroBasis, float MaxDistance)
{
	FVector TargetLocation = GetCursorTargetingPoint(bUseZeroBasis);
	if (MaxDistance > 0.0f)
	{
		const FVector SourceLocation = GetActorLocation();
		const FVector SourceLocationWithTargetZ(SourceLocation.X, SourceLocation.Y, TargetLocation.Z);
		const FVector SourceToTarget = TargetLocation - SourceLocationWithTargetZ;

		if (SourceToTarget.SizeSquared() >= FMath::Square(MaxDistance))
		{
			const FVector TargetDirection = SourceToTarget.GetSafeNormal2D();
			TargetLocation = SourceLocationWithTargetZ + (TargetDirection * MaxDistance);
		}
	}

	return TargetLocation;
}

bool ASMPlayerCharacterBase::GetTileLocationFromCursor(FVector& OutTileLocation, float MaxDistance)
{
	OutTileLocation = FVector::ZeroVector;

	if (ASMTile* Tile = USMTileFunctionLibrary::GetTileFromLocation(GetWorld(), GetLocationFromCursor(true, MaxDistance)))
	{
		OutTileLocation = Tile->GetTileLocation();
		return true;
	}

	return false;
}

void ASMPlayerCharacterBase::ServerSetActorHiddenInGame_Implementation(bool bNewIsActorHidden)
{
	bIsActorHidden = bNewIsActorHidden;
	OnRep_bIsActorHidden();
}

void ASMPlayerCharacterBase::ServerSetCollisionEnabled_Implementation(bool bNewIsCollisionEnabled)
{
	bIsCollisionEnabled = bNewIsCollisionEnabled;
	OnRep_bIsCollisionEnabled();
}

void ASMPlayerCharacterBase::ServerSetMovementEnabled_Implementation(bool bNewIsMovementEnabled)
{
	bIsMovementEnabled = bNewIsMovementEnabled;
	OnRep_bIsMovementEnabled();
}

void ASMPlayerCharacterBase::ServerSetUseControllerRotation_Implementation(bool bNewUseControllerRotation)
{
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
	if (TargetCharacter)
	{
		if (TargetCharacter->IsLocallyControlled())
		{
			return;
		}
	}

	// 로컬 컨트롤러를 찾고 스크린 인디케이터를 추가해줍니다.
	ASMGamePlayerController* LocalPlayerController = Cast<ASMGamePlayerController>(GetWorld()->GetFirstPlayerController());
	if (LocalPlayerController)
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

void ASMPlayerCharacterBase::MulticastSetCharacterStateVisibility_Implementation(bool bNewVisibility)
{
	if (HasAuthority())
	{
		return;
	}

	CharacterStateWidgetComponent->SetVisibility(bNewVisibility);
}

void ASMPlayerCharacterBase::MulticastRPCSetYawRotation_Implementation(float InYaw)
{
	SetActorRotation(FRotator(0.0, InYaw, 0.0));
}

void ASMPlayerCharacterBase::MulticastRPCSetLocation_Implementation(const FVector_NetQuantize10& NewLocation)
{
	SetActorLocation(NewLocation);
}

void ASMPlayerCharacterBase::ReceiveDamage(AActor* NewAttacker, float InDamageAmount)
{
	if (!ASC.Get() || !DataAsset)
	{
		return;
	}

	FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(DataAsset->DamageGE, 1.0f, ASC->MakeEffectContext());
	if (GESpecHandle.IsValid())
	{
		SetLastAttacker(NewAttacker);
		GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, InDamageAmount);
		ASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
	}

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = this;
	GCParams.TargetAttachComponent = GetMesh();
	ASC->ExecuteGC(this, SMTags::GameplayCue::Common::HitFlash, GCParams);
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

	const FVector Direction = (GetLocationFromCursor() - GetActorLocation()).GetSafeNormal();
	const FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	Controller->SetControlRotation(NewRotation);
}

void ASMPlayerCharacterBase::AddMoveSpeed(float MoveSpeedMultiplier, float Duration)
{
	USMCharacterMovementComponent* SourceMovement = GetCharacterMovement<USMCharacterMovementComponent>();
	if (!SourceMovement)
	{
		return;
	}

	SourceMovement->AddMoveSpeedBuff(MoveSpeedMultiplier, Duration);
}

void ASMPlayerCharacterBase::ServerRPCCharacterPushBack_Implementation(FVector_NetQuantize10 Velocity)
{
	LaunchCharacter(Velocity, true, true);
}

void ASMPlayerCharacterBase::ClientRPCCharacterPushBack_Implementation(FVector_NetQuantize10 Velocity)
{
	if (!ASC.Get())
	{
		return;
	}

	if (ASC->HasAnyMatchingGameplayTags(PushBackImmuneTags))
	{
		return;
	}

	LaunchCharacter(Velocity, true, true);
	ServerRPCCharacterPushBack(Velocity);
}

void ASMPlayerCharacterBase::ClientRPCAddMoveSpeed_Implementation(float MoveSpeedMultiplier, float Duration)
{
	AddMoveSpeed(MoveSpeedMultiplier, Duration);
	ServerRPCAddMoveSpeed(MoveSpeedMultiplier, Duration);
}

void ASMPlayerCharacterBase::OnTeamChanged()
{
	if (HasAuthority())
	{
		const ESMTeam SourceTeam = GetTeam();
		if (SourceTeam != ESMTeam::None)
		{
			Weapon = GetWorld()->SpawnActor<ASMWeaponBase>(DataAsset->WeaponClass[GetTeam()]);
			if (Weapon)
			{
				Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, DataAsset->WeaponSocketName);
				Weapon->SetOwner(this);
			}
		}
	}
}

void ASMPlayerCharacterBase::ServerSetNoteState_Implementation(bool bNewIsNote)
{
	if (bIsNoteState != bNewIsNote)
	{
		bIsNoteState = bNewIsNote;
		OnRep_bIsNoteState();
	}
}

void ASMPlayerCharacterBase::Move(const FInputActionValue& InputActionValue)
{
	if (!ASC.Get())
	{
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
	if (!HasAuthority() || !DataAsset || !ASC.Get())
	{
		return;
	}

	InitStat();

	for (const auto& DefaultActiveAbility : DataAsset->DefaultActiveAbilities)
	{
		if (DefaultActiveAbility.Value)
		{
			FGameplayAbilitySpec AbilitySpec(DefaultActiveAbility.Value, 1, static_cast<int32>(DefaultActiveAbility.Key));
			ASC->GiveAbility(AbilitySpec);
		}
	}

	for (const auto& DefaultAbility : DataAsset->DefaultAbilities)
	{
		if (DefaultAbility)
		{
			FGameplayAbilitySpec AbilitySpec(DefaultAbility);
			ASC->GiveAbility(AbilitySpec);
		}
	}

	for (const auto& PassiveAbility : DataAsset->PassiveAbilities)
	{
		if (PassiveAbility)
		{
			FGameplayAbilitySpec AbilitySpec(PassiveAbility);
			ASC->GiveAbility(AbilitySpec);
			ASC->TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void ASMPlayerCharacterBase::InitStat()
{
	const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(DataAsset->ForInitGE, 0, ASC->MakeEffectContext());
	if (!GESpecHandle.IsValid())
	{
		return;
	}

	if (FSMCharacterStatsData* CharacterStat = USMDataTableFunctionLibrary::GetCharacterStatData(CharacterType))
	{
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::MaxHP, CharacterStat->HP);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::HP, CharacterStat->HP);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::MoveSpeed, CharacterStat->MoveSpeed);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::MaxStamina, CharacterStat->Stamina);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::Stamina, CharacterStat->Stamina);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::MaxSkillGauge, CharacterStat->SkillGauge);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::SkillGauge, CharacterStat->SkillGauge);
		ASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
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

FVector ASMPlayerCharacterBase::GetCursorTargetingPoint(bool bUseZeroBasis)
{
	const FVector SourceLocation = GetActorLocation();
	const float CapsuleHalfHeigh = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector CapsuleOffset(0.0, 0.0, -CapsuleHalfHeigh);
	const FVector BasisLocation = bUseZeroBasis ? SourceLocation + CapsuleOffset : SourceLocation;

	if (!SMPlayerController.Get())
	{
		return BasisLocation;
	}

	FVector CursorWorldLocation;
	FVector CursorWorldDirection;
	SMPlayerController->DeprojectMousePositionToWorld(CursorWorldLocation, CursorWorldDirection);

	const FPlane Plane(BasisLocation, FVector::UpVector);
	const FVector IntersectionPoint = FMath::RayPlaneIntersection(CursorWorldLocation, CursorWorldDirection, Plane);
	if (!IntersectionPoint.ContainsNaN())
	{
		return IntersectionPoint;
	}

	return BasisLocation;
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
	const FVector MouseLocation = GetLocationFromCursor();
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
			CharacterStateWidget->MaxHealth = SourceAttributeSet->GetMaxPostureGauge();
			CharacterStateWidget->CurrentHealth = SourceAttributeSet->GetPostureGauge();
			CharacterStateWidget->UpdateHPBar();
		}
	}
}

void ASMPlayerCharacterBase::OnRep_bIsActorHidden()
{
	SetActorHiddenInGame(bIsActorHidden);
}

void ASMPlayerCharacterBase::OnRep_bIsCollisionEnabled()
{
	SetActorEnableCollision(bIsCollisionEnabled);
}

void ASMPlayerCharacterBase::OnRep_bIsMovementEnabled()
{
	UCharacterMovementComponent* SourceMovementComponent = GetCharacterMovement();
	if (!SourceMovementComponent)
	{
		return;
	}

	EMovementMode MovementMode = bIsMovementEnabled ? MOVE_Walking : MOVE_None;
	SourceMovementComponent->SetMovementMode(MovementMode);
}

void ASMPlayerCharacterBase::OnRep_bUseControllerRotation()
{
	bUseControllerRotationPitch = bUseControllerRotation;
	bUseControllerRotationYaw = bUseControllerRotation;
	bUseControllerRotationRoll = bUseControllerRotation;
}

void ASMPlayerCharacterBase::OnRep_bIsNoteState()
{
	USkeletalMeshComponent* CharacterMeshComponent = GetMesh();
	UMeshComponent* WeaponMeshComponent = Weapon.Get() ? Weapon->GetWeaponMeshComponent() : nullptr;
	if (!CharacterMeshComponent || !WeaponMeshComponent || !NoteMeshComponent)
	{
		return;
	}

	CharacterMeshComponent->SetVisibility(!bIsNoteState);
	WeaponMeshComponent->SetVisibility(!bIsNoteState);
	NoteMeshComponent->SetVisibility(bIsNoteState);

	if (bIsNoteState)
	{
		NoteMeshComponent->SetPosition(0.0f);
		NoteMeshComponent->Play(false);
	}
	else
	{
		NoteMeshComponent->Stop();
	}
}

void ASMPlayerCharacterBase::ServerRPCAddMoveSpeed_Implementation(float MoveSpeedMultiplier, float Duration)
{
	AddMoveSpeed(MoveSpeedMultiplier, Duration);
}
