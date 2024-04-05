// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Data/StereoMixControlData.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/StereoMixPlayerController.h"
#include "Player/StereoMixPlayerState.h"
#include "Utilities/StereoMixCollision.h"
#include "Utilities/StereoMixeLog.h"


AStereoMixPlayerCharacter::AStereoMixPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionProfileName(StereoMixCollisionProfileName::NoCollision);

	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionProfileName(StereoMixCollisionProfileName::Player);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
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
}

UAbilitySystemComponent* AStereoMixPlayerCharacter::GetAbilitySystemComponent() const
{
	return ASC.Get();
}

void AStereoMixPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AStereoMixPlayerState* StereoMixPlayerState = GetPlayerStateChecked<AStereoMixPlayerState>();
	ASC = StereoMixPlayerState->GetAbilitySystemComponent();
	if (ASC.Get())
	{
		ASC->InitAbilityActorInfo(StereoMixPlayerState, this);

		for (const auto& DefaultActiveAbility : DefaultActiveAbilities)
		{
			FGameplayAbilitySpec AbilitySpec(DefaultActiveAbility.Value, 1, DefaultActiveAbility.Key);
			ASC->GiveAbility(AbilitySpec);
		}

		for (const auto& DefaultAbility : DefaultAbilities)
		{
			FGameplayAbilitySpec AbilitySpec(DefaultAbility.Value);
			ASC->GiveAbility(AbilitySpec);
		}
	}
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

	return FVector();
}
