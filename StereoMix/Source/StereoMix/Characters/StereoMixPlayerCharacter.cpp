// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Data/StereoMixControlData.h"
#include "Data/StereoMixDesignData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/StereoMixPlayerController.h"
#include "Player/StereoMixPlayerState.h"
#include "Utilities/SMAssetPath.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "Utilities/SMTagName.h"


AStereoMixPlayerCharacter::AStereoMixPlayerCharacter()
{
	MoveSpeedTag = FGameplayTag::RequestGameplayTag(SMTagName::AttributeSet::Character::Init::MoveSpeed);
	ProjectileCooldownTag = FGameplayTag::RequestGameplayTag(SMTagName::AttributeSet::Character::Init::ProjectileCooldown);
	ProjectileAttackTag = FGameplayTag::RequestGameplayTag(SMTagName::AttributeSet::Character::Init::ProjectileAttack);
	
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UStereoMixDesignData> StereoMixDesignDataRef(SMAssetPath::DesignData);
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

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	MaxWalkSpeed = 0.0f;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;
}

void AStereoMixPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitCamera();
}

void AStereoMixPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CachedStereoMixPlayerController = GetController<AStereoMixPlayerController>();
	check(CachedStereoMixPlayerController);

	InitASC();
}

void AStereoMixPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStereoMixPlayerCharacter, MaxWalkSpeed);
	DOREPLIFETIME(AStereoMixPlayerCharacter, bEnableCollision);
	DOREPLIFETIME(AStereoMixPlayerCharacter, bUseControllerRotation);
	DOREPLIFETIME(AStereoMixPlayerCharacter, bEnableMovement);
	DOREPLIFETIME(AStereoMixPlayerCharacter, CatchCharacter);
	DOREPLIFETIME(AStereoMixPlayerCharacter, CaughtCharacter);
}

void AStereoMixPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	CachedStereoMixPlayerController = GetController<AStereoMixPlayerController>();
	check(CachedStereoMixPlayerController);
}

void AStereoMixPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!ASC.Get())
	{
		return;
	}

	ASC->OnChangedTag.RemoveAll(this);
}

void AStereoMixPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AStereoMixPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		FocusToCursor();
	}
}

void AStereoMixPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	const AStereoMixPlayerController* PlayerController = CastChecked<AStereoMixPlayerController>(Controller);
	const UStereoMixControlData* ControlData = PlayerController->GetControlData();
	if (ControlData)
	{
		EnhancedInputComponent->BindAction(ControlData->MoveAction, ETriggerEvent::Triggered, this, &AStereoMixPlayerCharacter::Move);
	}

	SetupGASInputComponent();
}

UAbilitySystemComponent* AStereoMixPlayerCharacter::GetAbilitySystemComponent() const
{
	return ASC.Get();
}

void AStereoMixPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitASC();
}

void AStereoMixPlayerCharacter::InitCamera()
{
	const FRotator CameraRotation(-50.0f, 0.0, 0.0);
	const float CameraDistance = 1800.0f;
	const float CameraFOV = 75.0f;

	CameraBoom->SetRelativeRotation(CameraRotation);
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->TargetArmLength = CameraDistance;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;

	Camera->SetFieldOfView(CameraFOV);
}

void AStereoMixPlayerCharacter::SetupGASInputComponent()
{
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	const AStereoMixPlayerController* PlayerController = CastChecked<AStereoMixPlayerController>(Controller);
	const UStereoMixControlData* ControlData = PlayerController->GetControlData();
	EnhancedInputComponent->BindAction(ControlData->ShootAction, ETriggerEvent::Triggered, this, &AStereoMixPlayerCharacter::GAInputPressed, EActiveAbility::Launch);
	EnhancedInputComponent->BindAction(ControlData->CatchAction, ETriggerEvent::Triggered, this, &AStereoMixPlayerCharacter::GAInputPressed, EActiveAbility::Catch);
	EnhancedInputComponent->BindAction(ControlData->SmashAction, ETriggerEvent::Triggered, this, &AStereoMixPlayerCharacter::GAInputPressed, EActiveAbility::Smash);
}

void AStereoMixPlayerCharacter::InitASC()
{
	AStereoMixPlayerState* StereoMixPlayerState = GetPlayerStateChecked<AStereoMixPlayerState>();
	ASC = StereoMixPlayerState->GetAbilitySystemComponent();
	if (!ASC.Get())
	{
		return;
	}

	if (HasAuthority())
	{
		ASC->InitAbilityActorInfo(StereoMixPlayerState, this);

		const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
		if (GEContextHandle.IsValid())
		{
			const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(ForInitGE, 0, GEContextHandle);
			if (GESpecHandle.IsValid())
			{
				GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(MoveSpeedTag, DesignData->MoveSpeed);
				GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(ProjectileCooldownTag, 1.0f / DesignData->ProjectileRate);
				GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(ProjectileAttackTag, DesignData->ProjectileAttack);
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
}

void AStereoMixPlayerCharacter::GAInputPressed(EActiveAbility InInputID)
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

void AStereoMixPlayerCharacter::GAInputReleased(EActiveAbility InInputID)
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

void AStereoMixPlayerCharacter::Move(const FInputActionValue& InputActionValue)
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

void AStereoMixPlayerCharacter::FocusToCursor()
{
	if (!Controller)
	{
		return;
	}

	const UStereoMixAbilitySystemComponent* CachedASC = ASC.Get();
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

FVector AStereoMixPlayerCharacter::GetCursorTargetingPoint()
{
	const FVector CachedActorLocation = GetActorLocation();
	FVector Result(0.0, 0.0, CachedActorLocation.Z);
	if (CachedStereoMixPlayerController)
	{
		FVector WorldLocation, WorldDirection;
		CachedStereoMixPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
		const FPlane Plane(CachedActorLocation, FVector::UpVector);
		const FVector IntersectionPoint = FMath::RayPlaneIntersection(WorldLocation, WorldDirection, Plane);
		if (!IntersectionPoint.ContainsNaN())
		{
			Result = IntersectionPoint;
		}
	}

	// 만약 커서위치와 캐릭터의 위치가 평면상에 놓이지 않았다면 경고를 표시합니다. 일반적으로 이런상황이 존재하지는 않을 것으로 예상됩니다.
	if (!FMath::IsNearlyEqual(CachedActorLocation.Z, Result.Z, KINDA_SMALL_NUMBER))
	{
		NET_LOG(this, Warning,
		        TEXT("캐릭터의 위치와 커서의 위치가 평면상에 놓이지 않았습니다. 캐릭터 Z값: %f 커서 Z값: %f"),
		        CachedActorLocation.Z, Result.Z);
	}

	return Result;
}

void AStereoMixPlayerCharacter::SetMaxWalkSpeed(float InSpeed)
{
	if (HasAuthority())
	{
		MaxWalkSpeed = InSpeed;
		OnRep_MaxWalkSpeed();
	}
}

void AStereoMixPlayerCharacter::MulticastRPCResetRelativeRotation_Implementation()
{
	SetActorRelativeRotation(FRotator::ZeroRotator);
}

void AStereoMixPlayerCharacter::MulticastRPCSetYawRotation_Implementation(float InYaw)
{
	SetActorRotation(FRotator(0.0, InYaw, 0.0));
}

void AStereoMixPlayerCharacter::OnRep_MaxWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AStereoMixPlayerCharacter::SetEnableCollision(bool bInEnableCollision)
{
	if (HasAuthority())
	{
		bEnableCollision = bInEnableCollision;
		OnRep_EnableCollision();
	}
}

void AStereoMixPlayerCharacter::SetUseControllerRotation(bool bInUseControllerRotation)
{
	if (HasAuthority())
	{
		bUseControllerRotation = bInUseControllerRotation;
		OnRep_UseControllerRotation();
	}
}

void AStereoMixPlayerCharacter::SetEnableMovement(bool bInEnableMovementMode)
{
	if (HasAuthority())
	{
		bEnableMovement = bInEnableMovementMode;
		OnRep_EnableMovement();
	}
}

void AStereoMixPlayerCharacter::OnRep_EnableCollision()
{
	SetActorEnableCollision(bEnableCollision);
}

void AStereoMixPlayerCharacter::OnRep_UseControllerRotation()
{
	bUseControllerRotationPitch = bUseControllerRotation;
	bUseControllerRotationYaw = bUseControllerRotation;
	bUseControllerRotationRoll = bUseControllerRotation;
}

void AStereoMixPlayerCharacter::OnRep_EnableMovement()
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