// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Data/StereoMixControlData.h"
#include "Data/StereoMixDesignData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/StereoMixPlayerController.h"
#include "Player/StereoMixPlayerState.h"
#include "Utilities/StereoMixAssetPath.h"
#include "Utilities/StereoMixCollision.h"
#include "Utilities/StereoMixeLog.h"
#include "Utilities/StereoMixTag.h"


AStereoMixPlayerCharacter::AStereoMixPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStereoMixDesignData> StereoMixDesignDataRef(StereoMixAssetPath::DesignData);
	if (StereoMixDesignDataRef.Object)
	{
		DesignData = StereoMixDesignDataRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DesignData 로드에 실패했습니다."));
	}

	GetMesh()->SetCollisionProfileName(StereoMixCollisionProfileName::NoCollision);

	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionProfileName(StereoMixCollisionProfileName::Player);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	MaxWalkSpeed = 0.0f;
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
}

void AStereoMixPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	CachedStereoMixPlayerController = GetController<AStereoMixPlayerController>();
	check(CachedStereoMixPlayerController);
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
		if (Controller)
		{
			const FVector Direction = (GetCursorTargetingPoint() - GetActorLocation()).GetSafeNormal();
			const FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
			Controller->SetControlRotation(NewRotation);
		}
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
	if (!ASC)
	{
		NET_LOG(this, Warning, TEXT("%s는 GAS를 지원하지 않는 액터입니다."), *GetName())
		return;
	}

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	const AStereoMixPlayerController* PlayerController = CastChecked<AStereoMixPlayerController>(Controller);
	const UStereoMixControlData* ControlData = PlayerController->GetControlData();
	EnhancedInputComponent->BindAction(ControlData->ShootAction, ETriggerEvent::Triggered, this, &AStereoMixPlayerCharacter::GAInputPressed, EActiveAbility::Shoot);
}

void AStereoMixPlayerCharacter::InitASC()
{
	AStereoMixPlayerState* StereoMixPlayerState = GetPlayerStateChecked<AStereoMixPlayerState>();
	ASC = StereoMixPlayerState->GetAbilitySystemComponent();
	if (HasAuthority())
	{
		if (ASC.Get())
		{
			ASC->InitAbilityActorInfo(StereoMixPlayerState, this);

			const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
			if (GEContextHandle.IsValid())
			{
				const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(GEForInit, 0, GEContextHandle);
				if (GESpecHandle.IsValid())
				{
					GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(StereoMixTag::AttributeSet_Character_Init_MoveSpeed, DesignData->MoveSpeed);
					GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(StereoMixTag::AttributeSet_Character_Init_ProjectileCooldown, 1.0f / DesignData->ProjectileRate);
					GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(StereoMixTag::AttributeSet_Character_Init_ProjectileAttack, DesignData->ProjectileAttack);
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
}

void AStereoMixPlayerCharacter::GAInputPressed(EActiveAbility InInputID)
{
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
	const FVector2D InputScalar = InputActionValue.Get<FVector2D>().GetSafeNormal();
	const FRotator CameraYawRotation(0.0, Camera->GetComponentRotation().Yaw, 0.0);
	const FVector ForwardDirection = FRotationMatrix(CameraYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(CameraYawRotation).GetUnitAxis(EAxis::Y);
	const FVector MoveVector = (ForwardDirection * InputScalar.X) + (RightDirection * InputScalar.Y);

	AddMovementInput(MoveVector);
}

FVector AStereoMixPlayerCharacter::GetCursorTargetingPoint()
{
	if (CachedStereoMixPlayerController)
	{
		FVector WorldLocation, WorldDirection;
		CachedStereoMixPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
		const FPlane Plane(GetActorLocation(), FVector::UpVector);
		const FVector IntersectionPoint = FMath::RayPlaneIntersection(WorldLocation, WorldDirection, Plane);
		if (!IntersectionPoint.ContainsNaN())
		{
			return IntersectionPoint;
		}
	}

	return FVector(0.0, 0.0, GetActorLocation().Z);
}

void AStereoMixPlayerCharacter::SetMaxWalkSpeed(float InSpeed)
{
	if (HasAuthority())
	{
		MaxWalkSpeed = InSpeed;
		OnRep_MaxWalkSpeed();
	}
}

void AStereoMixPlayerCharacter::OnRep_MaxWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}
