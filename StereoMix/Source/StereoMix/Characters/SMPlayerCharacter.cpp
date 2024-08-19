// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SMCatchInteractionComponent_Character.h"
#include "Components/SMTeamComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Controllers/SMGamePlayerController.h"
#include "Data/SMCharacterAssetData.h"
#include "Data/SMControlData.h"
#include "Data/SMDesignData.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "Games/SMGamePlayerState.h"
#include "Tiles/SMTile.h"
#include "UI/SMWidgetComponent.h"
#include "UI/Widget/Game/SMUserWidget_CharacterState.h"
#include "Utilities/SMAssetPath.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

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

	USkeletalMeshComponent* CachedMeshComponent = GetMesh();
	CachedMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	CachedMeshComponent->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	UCharacterMovementComponent* CachedMovementComponent = GetCharacterMovement();
	CachedMovementComponent->MaxAcceleration = 9999.0f;
	CachedMovementComponent->MaxStepHeight = 10.0f;
	CachedMovementComponent->SetWalkableFloorAngle(5.0f);
	CachedMovementComponent->bCanWalkOffLedges = false;

	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionProfileName(SMCollisionProfileName::Player);
	HitBox->InitSphereRadius(125.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	InitCamera();

	ListenerComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ListenerComponent"));
	ListenerComponent->SetAbsolute(false, true, true);
	ListenerComponent->SetupAttachment(CameraBoom);

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

	// TODO: 임시코드
	TrajectoryFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrajectoryFXComponent"));
	TrajectoryFXComponent->SetAbsolute(true, true, true);
	TrajectoryFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	TrajectoryFXComponent->SetAutoActivate(false);
	TrajectoryFXComponent->SetHiddenInGame(true);

	TrajectoryMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrajectoryMeshComponent"));
	TrajectoryMeshComponent->SetAbsolute(false, true, true);
	TrajectoryMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	TrajectoryMeshComponent->SetupAttachment(TrajectoryFXComponent);
	TrajectoryMeshComponent->SetHiddenInGame(true);
	TrajectoryMeshComponent->SetWorldScale3D(FVector(1.0, 0.8, 1.0));
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
		OriginalMaterials = GetMesh()->GetMaterials();
		OriginalOverlayMaterial = GetMesh()->GetOverlayMaterial();
	}

	TeamComponent->OnChangeTeam.AddDynamic(this, &ASMPlayerCharacter::OnTeamChangeCallback);
}

void ASMPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CachedSMPlayerController = GetController<ASMGamePlayerController>();
	check(CachedSMPlayerController);

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

	CachedSMPlayerController = GetController<ASMGamePlayerController>();
	check(CachedSMPlayerController);

	CachedSMPlayerController->SetAudioListenerOverride(ListenerComponent, FVector::ZeroVector, FRotator::ZeroRotator);
}

void ASMPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!ASC.Get())
	{
		return;
	}

	ASC->OnChangedTag.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

void ASMPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CatchInteractionComponent->OnCatch.AddUObject(this, &ThisClass::OnCatch);
	CatchInteractionComponent->OnCatchRelease.AddUObject(this, &ThisClass::OnCatchRelease);

	if (IsLocallyControlled())
	{
		TrajectoryFXComponent->Activate(true);
	}
}

void ASMPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		UpdateCameraLocation();
		FocusToCursor();
		DrawSmashEndPoint();
	}
}

void ASMPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	const ASMGamePlayerController* PlayerController = CastChecked<ASMGamePlayerController>(Controller);
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

	ESMTeam SourceTeam = TeamComponent->GetTeam();
	const TSubclassOf<UUserWidget>* CharacterStateWidgetPtr = AssetData->CharacterStateWidget.Find(SourceTeam);
	USMUserWidget_CharacterState* NewWidget = CreateWidget<USMUserWidget_CharacterState>(GetWorld(), *CharacterStateWidgetPtr);
	CharacterStateWidgetComponent->SetWidget(NewWidget);

	DefaultMoveTrailFXComponent->SetAsset(DefaultMoveTrailFX[SourceTeam]);
	DefaultMoveTrailFXComponent->Activate(true);

	CatchMoveTrailFXComponent->SetAsset(CatchMoveTrailFX[SourceTeam]);
	ImmuneMoveTrailFXComponent->SetAsset(ImmuneMoveTrailFX[SourceTeam]);
}

void ASMPlayerCharacter::InitCamera()
{
	const FRotator CameraRotation(-50.0f, 0.0, 0.0);
	constexpr float CameraDistance = 1800.0f;
	const FVector CameraTargetOffset(-50.0, 0.0, 0.0);
	constexpr float CameraFOV = 75.0f;

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

void ASMPlayerCharacter::UpdateCameraLocation()
{
	FVector2D MouseScreenLocation;
	if (!CachedSMPlayerController->GetMousePosition(MouseScreenLocation.X, MouseScreenLocation.Y))
	{
		return;
	}

	if (!GEngine)
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
	const ASMGamePlayerController* PlayerController = CastChecked<ASMGamePlayerController>(Controller);
	const USMControlData* ControlData = PlayerController->GetControlData();
	EnhancedInputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Triggered, this, &ASMPlayerCharacter::GAInputPressed, EActiveAbility_Legacy::Launch);
	EnhancedInputComponent->BindAction(ControlData->HoldAction, ETriggerEvent::Triggered, this, &ASMPlayerCharacter::GAInputPressed, EActiveAbility_Legacy::Catch);
	EnhancedInputComponent->BindAction(ControlData->NoiseBreakAction, ETriggerEvent::Triggered, this, &ASMPlayerCharacter::GAInputPressed, EActiveAbility_Legacy::Smash);
}

void ASMPlayerCharacter::InitASC()
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

	CatchInteractionComponent->InitASC(ASC.Get());

	if (HasAuthority())
	{
		ASC->ClearAllAbilities();
		ASC->InitAbilityActorInfo(StereoMixPlayerState, this);
		GiveDefaultAbilities();
	}

	ASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ThisClass::OnChangeMoveSpeed);
	const USMCharacterAttributeSet* AttributeSet = ASC->GetSet<USMCharacterAttributeSet>();
	if (!ensureAlways(AttributeSet))
	{
		return;
	}

	TeamComponent->SetTeam(StereoMixPlayerState->GetTeam());

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
			GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::Character::Init::MoveSpeed, MoveSpeed);
			GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::Character::Init::MaxHP, MaxHP);
			GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::Character::Init::CurrentHP, MaxHP);
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

void ASMPlayerCharacter::GAInputPressed(EActiveAbility_Legacy InInputID)
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

void ASMPlayerCharacter::GAInputReleased(EActiveAbility_Legacy InInputID)
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

	if (CachedSMPlayerController)
	{
		FVector WorldLocation, WorldDirection;
		CachedSMPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
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

	OnLanded.Broadcast(this);
}

void ASMPlayerCharacter::SetCharacterStateVisibility_Implementation(bool bEnable)
{
	CharacterStateWidgetComponent->SetVisibility(bEnable);
}

void ASMPlayerCharacter::MulticastRPCAddScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	if (HasAuthority())
	{
		return;
	}

	// 자기 자신은 제외합니다.
	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(TargetActor);
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

void ASMPlayerCharacter::MulticastRPCRemoveScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	if (HasAuthority())
	{
		return;
	}

	// 자기 자신은 제외합니다.
	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(TargetActor);
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

void ASMPlayerCharacter::ClientRPCRemoveScreendIndicatorToSelf_Implementation(AActor* TargetActor)
{
	// ClientRPC로 무조건 오너십을 갖고 있어 CachedSMPlayerController가 유효하겠지만 만약을 위한 예외처리입니다.
	if (ensureAlways(CachedSMPlayerController))
	{
		CachedSMPlayerController->RemoveScreenIndicator(TargetActor);
	}
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
		SetCharacterMoveTrailState(EMoveTrailState_Legacy::Catch);
	}
}

void ASMPlayerCharacter::OnCatchRelease()
{
	if (HasAuthority())
	{
		SetCharacterMoveTrailState(EMoveTrailState_Legacy::Default);
	}
}

void ASMPlayerCharacter::OnTeamChangeCallback()
{
}

bool ASMPlayerCharacter::bAmICatching()
{
	return CatchInteractionComponent->GetActorIAmCatching() != nullptr;
}

void ASMPlayerCharacter::SetCharacterMoveTrailState(EMoveTrailState_Legacy NewCharacterMoveTrailState)
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
		case EMoveTrailState_Legacy::None:
		{
			break;
		}
		case EMoveTrailState_Legacy::Default:
		{
			ActivateDefaultMoveTrail();
			break;
		}
		case EMoveTrailState_Legacy::Catch:
		{
			ActivateCatchMoveTrail();
			break;
		}
		case EMoveTrailState_Legacy::Immune:
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

void ASMPlayerCharacter::DrawSmashEndPoint()
{
	TrajectoryFXComponent->SetHiddenInGame(true, true);

	if (!ASC.Get())
	{
		return;
	}

	if (!ASC->HasMatchingGameplayTag(SMTags::Character::State::Catch))
	{
		return;
	}

	if (ASC->HasMatchingGameplayTag(SMTags::Character::State::Smashing))
	{
		return;
	}

	auto CalculateMaxDistanceLocation = [](const FVector& InStartLocation, const FVector& InTargetLocation) {
		FVector AlignedSourceZ = InStartLocation;
		AlignedSourceZ.Z = InTargetLocation.Z;

		constexpr float TileSize = 150.0f;
		constexpr float Range = TileSize * 6;
		if (FVector::DistSquared(AlignedSourceZ, InTargetLocation) > FMath::Square(Range))
		{
			const FVector SourceToTargetDirection = (InTargetLocation - AlignedSourceZ).GetSafeNormal();

			const FVector Result = AlignedSourceZ + (SourceToTargetDirection * Range);
			return Result;
		}

		return InTargetLocation;
	};

	const FVector SourceLocation = GetActorLocation();
	const FVector TargetLocation = GetCursorTargetingPoint(true);
	const FVector Start = CalculateMaxDistanceLocation(SourceLocation, TargetLocation);
	const FVector End = Start + (FVector::DownVector * 100.0f);

	FHitResult HitResult;
	const bool bSuccess = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, SMCollisionTraceChannel::TileAction);
	if (bSuccess)
	{
		ASMTile* TargetTile = Cast<ASMTile>(HitResult.GetActor());
		if (TargetTile)
		{
			TrajectoryFXComponent->SetHiddenInGame(false, true);

			const FVector TrajectoryTargetLocation = TargetTile->GetTileLocation() + FVector(0.0, 0.0, 10.0);
			TrajectoryFXComponent->SetWorldLocation(TrajectoryTargetLocation);

			const FRotator TrajectoryTargetRotation = (TrajectoryTargetLocation - SourceLocation).GetSafeNormal2D().Rotation();
			TrajectoryMeshComponent->SetWorldRotation(TrajectoryTargetRotation);

			const float Distance = FVector::Dist2D(SourceLocation, TrajectoryTargetLocation);
			const float Ratio = Distance / 100.0f;
			const FVector CachedScale = TrajectoryMeshComponent->GetComponentScale();
			TrajectoryMeshComponent->SetWorldScale3D(FVector(Ratio, CachedScale.Y, CachedScale.Z));
		}
	}
	else
	{
		TrajectoryFXComponent->SetHiddenInGame(true, true);
	}
}
