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
#include "Actors/Notes/SMNoteBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Character/SMCharacterMovementComponent.h"
#include "Components/Character/SMHIC_Character.h"
#include "Components/Common/SMTeamComponent.h"
#include "Components/Core/SMTileManagerComponent.h"
#include "Components/PlayerController/SMScreenIndicatorComponent.h"
#include "Controllers/SMGamePlayerController.h"
#include "Data/SMControlData.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMScoreFunctionLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Games/SMGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/Game/SMUserWidget_CharacterState.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMPlayerCharacterBase::ASMPlayerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;

	UCapsuleComponent* CachedCapsuleComponent = GetCapsuleComponent();
	CachedCapsuleComponent->SetCollisionProfileName(SMCollisionProfileName::Player);

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
	HitBox->SetCollisionProfileName(SMCollisionProfileName::PlayerProjectileHitbox);
	HitBox->InitCapsuleSize(125.0f, 125.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(-52.5f, 0.0, 0.0));
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->TargetArmLength = 2000.0f;
	CameraBoom->TargetOffset = FVector(-50.0, 0.0, 0.0);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	Camera->SetFieldOfView(75.0f);

	ListenerComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ListenerComponent"));
	ListenerComponent->SetAbsolute(false, true);
	ListenerComponent->SetupAttachment(RootComponent);

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));

	NoteSlotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("NoteSlotComponent"));
	NoteSlotComponent->SetupAttachment(CachedMeshComponent);
	NoteSlotComponent->SetAbsolute(false, true, false);

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
	HIC->OnHoldStateEnrty.AddUObject(this, &ThisClass::OnHoldStateEntry);
	HIC->OnHoldStateExit.AddUObject(this, &ThisClass::OnHoldStateExit);

	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::Held);
	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::Neutralized);
	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::Immune);
	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);

	LockAimTags.AddTag(SMTags::Character::State::Common::Uncontrollable);
	LockAimTags.AddTag(SMTags::Character::State::Common::Held);
	LockAimTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	LockAimTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	LockAimTags.AddTag(SMTags::Character::State::Common::Neutralized);

	LockMovementTags.AddTag(SMTags::Character::State::Common::Uncontrollable);
	LockMovementTags.AddTag(SMTags::Character::State::Common::Held);
	LockMovementTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	LockMovementTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	LockMovementTags.AddTag(SMTags::Character::State::Common::Neutralized);
	LockMovementTags.AddTag(SMTags::Character::State::Common::Jump);
	LockMovementTags.AddTag(SMTags::Character::State::Common::Stun);

	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Held);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Neutralized);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Immune);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Jump);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Stun);
}

void ASMPlayerCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsActorHidden);
	DOREPLIFETIME(ThisClass, bIsCollisionEnabled);
	DOREPLIFETIME(ThisClass, bIsMovementEnabled);
	DOREPLIFETIME(ThisClass, bUseControllerRotation);
	DOREPLIFETIME(ThisClass, LastAttacker);
	DOREPLIFETIME(ThisClass, Note);
	DOREPLIFETIME(ThisClass, bIsNoteState);
}

void ASMPlayerCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if (!DataAsset || !World)
	{
		return;
	}

	const ESMTeam SourceTeam = GetTeam();

	USkeletalMeshComponent* CachedMeshComponent = GetMesh();

	UCharacterMovementComponent* CachedMovementComponent = GetCharacterMovement();
	CachedMovementComponent->GravityScale = 2.0f;

	// 트레일 위치를 교정하기 위해 재어태치합니다. 재어태치하는 이유는 생성자에서는 메시가 null이므로 소켓을 찾을 수 없기 때문입니다.
	DefaultMoveTrailFXComponent->AttachToComponent(CachedMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	CatchMoveTrailFXComponent->AttachToComponent(CachedMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	ImmuneMoveTrailFXComponent->AttachToComponent(CachedMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);

	// 원본 머티리얼을 저장해둡니다. 플레이 도중 시시각각 머티리얼이 변하게 되는데 이때 기존 머티리얼로 돌아오기 위해 사용됩니다.
	OriginalMaterials = CachedMeshComponent->GetMaterials();
	OriginalOverlayMaterial = CachedMeshComponent->GetOverlayMaterial();

	if (HasAuthority())
	{
		if (DataAsset->NoteClass.Contains(SourceTeam))
		{
			if (Note = World->SpawnActor<ASMNoteBase>(DataAsset->NoteClass[SourceTeam]); Note)
			{
				Note->AttachToComponent(NoteSlotComponent, FAttachmentTransformRules::KeepRelativeTransform);
				Note->SetOwner(this);
			}
		}

		if (USMTileManagerComponent* TileManager = USMTileFunctionLibrary::GetTileManagerComponent(World))
		{
			TileManager->OnTilesCaptured.AddDynamic(this, &ThisClass::OnTilesCaptured);
		}
	}

	if (USMTeamBlueprintLibrary::IsSameLocalTeam(this))
	{
		switch (SourceTeam)
		{
			case ESMTeam::EDM:
			{
				DefaultShaderStencil = ESMShaderStencil::EDMOutline;
				break;
			}
			case ESMTeam::FutureBass:
			{
				DefaultShaderStencil = ESMShaderStencil::FBOutline;
				break;
			}
			default: ;
		}
	}

	ResetStencil();

	TeamComponent->OnChangeTeam.AddDynamic(this, &ThisClass::OnTeamChanged);
}

void ASMPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (const USMControlData* ControlData = SMPlayerController->GetControlData())
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

	if (Note)
	{
		Note->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void ASMPlayerCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (ASMGamePlayerController* CachedPlayerController = GetController<ASMGamePlayerController>())
	{
		SMPlayerController = CachedPlayerController;
		SMPlayerController->SetAudioListenerOverride(ListenerComponent, FVector::ZeroVector, FRotator::ZeroRotator);

		if (IsLocallyControlled())
		{
			DefaultShaderStencil = ESMShaderStencil::SelfOutline;
			ResetStencil();
		}
	}
}

void ASMPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SMPlayerController = GetController<ASMGamePlayerController>();

	if (IsLocallyControlled())
	{
		SMPlayerController->SetAudioListenerOverride(ListenerComponent, FVector::ZeroVector, FRotator::ZeroRotator);
	}

	InitASC();

	if (GetNetMode() != NM_DedicatedServer)
	{
		InitUI();
	}
}

void ASMPlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitASC();
	InitUI();

	const ESMTeam SourceTeam = GetTeam();

	DefaultMoveTrailFXComponent->SetAsset(DataAsset->DefaultMoveTrailFX[SourceTeam]);
	DefaultMoveTrailFXComponent->Activate();

	CatchMoveTrailFXComponent->SetAsset(DataAsset->CatchMoveTrailFX[SourceTeam]);
	CatchMoveTrailFXComponent->Deactivate();

	ImmuneMoveTrailFXComponent->SetAsset(DataAsset->ImmuneMoveTrailFX[SourceTeam]);
	ImmuneMoveTrailFXComponent->Deactivate();
}

void ASMPlayerCharacterBase::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);

	if (Weapon)
	{
		Weapon->SetActorHiddenInGame(bNewHidden);
	}

	if (Note)
	{
		Note->SetActorHiddenInGame(bNewHidden);
	}
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
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent();
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

	if (const ASMTile* Tile = USMTileFunctionLibrary::GetTileFromLocation(GetWorld(), GetLocationFromCursor(true, MaxDistance)))
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

void ASMPlayerCharacterBase::MulticastAddScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	// 자기 자신은 제외합니다.
	const ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!TargetCharacter || TargetCharacter->IsLocallyControlled())
	{
		return;
	}

	// 로컬 컨트롤러를 찾고 스크린 인디케이터를 추가해줍니다.
	const UWorld* World = GetWorld();
	const APlayerController* LocalPlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (USMScreenIndicatorComponent* ScreenIndicatorComponent = LocalPlayerController ? LocalPlayerController->GetComponentByClass<USMScreenIndicatorComponent>() : nullptr)
	{
		ScreenIndicatorComponent->AddScreenIndicator(TargetActor);
	}
}

void ASMPlayerCharacterBase::MulticastRemoveScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	// 자기 자신은 인디케이터가 추가되지 않았으니 제외합니다.
	const ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!TargetCharacter || TargetCharacter->IsLocallyControlled())
	{
		return;
	}

	// 로컬 컨트롤러를 찾고 스크린 인디케이터를 제거합니다.
	const UWorld* World = GetWorld();
	const APlayerController* LocalPlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (USMScreenIndicatorComponent* ScreenIndicatorComponent = LocalPlayerController ? LocalPlayerController->GetComponentByClass<USMScreenIndicatorComponent>() : nullptr)
	{
		ScreenIndicatorComponent->RemoveScreenIndicator(TargetActor);
	}
}

void ASMPlayerCharacterBase::ClientRemoveScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	if (USMScreenIndicatorComponent* ScreenIndicatorComponent = Controller ? Controller->GetComponentByClass<USMScreenIndicatorComponent>() : nullptr)
	{
		ScreenIndicatorComponent->RemoveScreenIndicator(TargetActor);
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

	const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(DataAsset->DamageGE, 1.0f, ASC->MakeEffectContext());
	if (GESpecHandle.IsValid())
	{
		SetLastAttacker(NewAttacker);
		GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, InDamageAmount);
		ASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);

		USMScoreFunctionLibrary::RecordDamage(this, NewAttacker, InDamageAmount);
	}

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = this;
	GCParams.TargetAttachComponent = GetMesh();
	ASC->ExecuteGC(this, SMTags::GameplayCue::Common::HitFlash, GCParams);
}

bool ASMPlayerCharacterBase::CanIgnoreAttack() const
{
	if (!ASC.Get())
	{
		return false;
	}

	return ASC->HasAnyMatchingGameplayTags(IgnoreAttackTags);
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
		const ESMTeam OwnerTeam = GetTeam();
		if (const TSubclassOf<ASMWeaponBase>& CachedWeaponClass = DataAsset->WeaponClass.Contains(OwnerTeam) ? DataAsset->WeaponClass[OwnerTeam] : nullptr)
		{
			if (Weapon = GetWorld()->SpawnActor<ASMWeaponBase>(CachedWeaponClass); Weapon)
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

	HIC->SetASC(ASC.Get());

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

	if (const FSMCharacterStatsData* CharacterStat = USMDataTableFunctionLibrary::GetCharacterStatData(CharacterType))
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

	if (FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InInputID)))
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

	if (FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InInputID)))
	{
		if (GASpec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*GASpec);
		}
	}
}

void ASMPlayerCharacterBase::InitUI()
{
	const USMCharacterAttributeSet* SourceAttributeSet = ASC.IsValid() ? ASC->GetSet<USMCharacterAttributeSet>() : nullptr;
	const APlayerState* CachedPlayerState = GetPlayerState();
	if (!ASC.IsValid() || !SourceAttributeSet || !CachedPlayerState)
	{
		return;
	}

	const ESMTeam SourceTeam = GetTeam();

	if (USMUserWidget_CharacterState* CharacterStateWidget = CreateWidget<USMUserWidget_CharacterState>(GetWorld(), DataAsset->CharacterStateWidget[SourceTeam]))
	{
		CharacterStateWidgetComponent->SetWidget(CharacterStateWidget);

		CharacterStateWidget->SetNickname(CachedPlayerState->GetPlayerName());

		ASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetPostureGaugeAttribute()).AddUObject(CharacterStateWidget, &USMUserWidget_CharacterState::OnChangeCurrentHealth);
		ASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetMaxPostureGaugeAttribute()).AddUObject(CharacterStateWidget, &USMUserWidget_CharacterState::OnChangeMaxHealth);

		CharacterStateWidget->MaxHealth = SourceAttributeSet->GetMaxPostureGauge();
		CharacterStateWidget->CurrentHealth = SourceAttributeSet->GetPostureGauge();
		CharacterStateWidget->UpdateHPBar();
	}
}

FVector ASMPlayerCharacterBase::GetCursorTargetingPoint(bool bUseZeroBasis)
{
	const FVector SourceLocation = GetActorLocation();
	const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector CapsuleOffset(0.0, 0.0, -CapsuleHalfHeight);
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

	const FVector SourceLocation = GetActorLocation();
	const FVector MouseLocation = GetLocationFromCursor();
	FVector TargetLocation = (SourceLocation + MouseLocation) * 0.5;

	const FVector SourceToTargetVector = TargetLocation - SourceLocation;
	const float Distance = SourceToTargetVector.Size();

	const float MaxMouseRange = CameraMoveMouseThreshold / 2.0f;
	const float MoveDistance = FMath::Clamp((Distance / MaxMouseRange) * CameraMoveMaxDistance, 0.0f, CameraMoveMaxDistance);
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
	if (const USMAbilitySystemComponent* CachedASC = ASC.Get())
	{
		if (CachedASC->HasAnyMatchingGameplayTags(LockAimTags))
		{
			return;
		}
	}

	FocusToCursor();
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

	const EMovementMode MovementMode = bIsMovementEnabled ? MOVE_Walking : MOVE_None;
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
	USceneComponent* WeaponRootComponent = Weapon ? Weapon->GetRootComponent() : nullptr;
	USceneComponent* NoteRootComponent = Note ? Note->GetRootComponent() : nullptr;
	if (!CharacterMeshComponent || !WeaponRootComponent || !NoteRootComponent)
	{
		return;
	}

	CharacterMeshComponent->SetVisibility(!bIsNoteState);

	// SetActorHidden을 하지않고 루트를 기준으로 자식들을 비활성화하는 이유는 Hold, Held 될때 SetActorHidden을 사용하기 때문에 Held 되었을때 무기가 보이게 됩니다. 이를 방지하고자 아예 컴포넌트들의 비지빌리티를 끕니다.
	WeaponRootComponent->SetVisibility(!bIsNoteState, true);
	NoteRootComponent->SetVisibility(bIsNoteState, true);

	if (bIsNoteState)
	{
		Note->PlayAnimation();
	}
	else
	{
		Note->StopAnimation();
	}
}

void ASMPlayerCharacterBase::ServerRPCAddMoveSpeed_Implementation(float MoveSpeedMultiplier, float Duration)
{
	AddMoveSpeed(MoveSpeedMultiplier, Duration);
}

void ASMPlayerCharacterBase::OnTilesCaptured(const AActor* CapturedInstigator, int CapturedTileCount)
{
	if (CapturedInstigator == this)
	{
		if (ASC.IsValid() && !ASC->HasMatchingGameplayTag(SMTags::Character::State::Bass::Charge) && DataAsset) // 임시로 돌진중에 회복을 막습니다.
		{
			const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(DataAsset->StaminaHealGE, 1.0f, ASC->MakeEffectContext());
			if (GESpecHandle.IsValid())
			{
				GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::SkillGauge, StaminaHealAmountPerCapture * CapturedTileCount);
				ASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
			}
		}
	}
}
