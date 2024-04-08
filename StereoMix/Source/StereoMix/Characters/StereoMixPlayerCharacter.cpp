// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "AbilitySystems/StereoMixAbilitySystemComponent.h"
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
	DOREPLIFETIME(AStereoMixPlayerCharacter, bIsEnableCollision);
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

void AStereoMixPlayerCharacter::SetCollision(bool bIsEnable)
{
	if (HasAuthority())
	{
		bIsEnableCollision = bIsEnable;
		OnRep_IsEnableCollision();
	}
}

void AStereoMixPlayerCharacter::OnRep_IsEnableCollision()
{
	SetActorEnableCollision(bIsEnableCollision);
}

void AStereoMixPlayerCharacter::SetupGASInputComponent()
{
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	const AStereoMixPlayerController* PlayerController = CastChecked<AStereoMixPlayerController>(Controller);
	const UStereoMixControlData* ControlData = PlayerController->GetControlData();
	EnhancedInputComponent->BindAction(ControlData->ShootAction, ETriggerEvent::Triggered, this, &AStereoMixPlayerCharacter::GAInputPressed, EActiveAbility::Launch);
	EnhancedInputComponent->BindAction(ControlData->CatchAction, ETriggerEvent::Triggered, this, &AStereoMixPlayerCharacter::GAInputPressed, EActiveAbility::Catch);
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
				GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(StereoMixTag::AttributeSet::Character::Init::MoveSpeed, DesignData->MoveSpeed);
				GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(StereoMixTag::AttributeSet::Character::Init::ProjectileCooldown, 1.0f / DesignData->ProjectileRate);
				GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(StereoMixTag::AttributeSet::Character::Init::ProjectileAttack, DesignData->ProjectileAttack);
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

	ASC->OnChangedTag.AddUObject(this, &AStereoMixPlayerCharacter::OnChangedTag);
}

void AStereoMixPlayerCharacter::GAInputPressed(EActiveAbility InInputID)
{
	if (!ASC.Get())
	{
		NET_LOG(this, Warning, TEXT("ASC가 유효하지 않습니다."))
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
		NET_LOG(this, Warning, TEXT("ASC가 유효하지 않습니다."))
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

void AStereoMixPlayerCharacter::OnChangedTag(const FGameplayTag& Tag, bool TagExists)
{
	if (Tag == StereoMixTag::Character::State::Stun)
	{
		if (TagExists)
		{
			OnAddStunTag();
		}
		else
		{
			OnRemoveStunTag();
		}
	}

	if (Tag == StereoMixTag::Character::State::Caught)
	{
		if (TagExists)
		{
			OnAddCaughtTag();
		}
		else
		{
			OnRemoveCaughtTag();
		}
	}
}

void AStereoMixPlayerCharacter::OnAddStunTag() {}

void AStereoMixPlayerCharacter::OnRemoveStunTag()
{
	if (HasAuthority())
	{
		if (!ASC.Get())
		{
			NET_LOG(this, Log, TEXT("ASC가 유효하지 않습니다."));
			return;
		}

		for (const auto& StunEndedGE : StunEndedGEs)
		{
			if (!StunEndedGE)
			{
				NET_LOG(this, Log, TEXT("GE가 유효하지 않습니다. 참조하지 않고 있을 수도 있습니다. 확인해주세요."));
				return;
			}
		
			const FGameplayEffectContextHandle GESpec = ASC->MakeEffectContext();
			if (GESpec.IsValid())
			{
				ASC->BP_ApplyGameplayEffectToSelf(StunEndedGE, 0.0f, GESpec);
			}
		}
	}
}

void AStereoMixPlayerCharacter::OnAddCaughtTag()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetActorRelativeRotation(FRotator::ZeroRotator);
}

void AStereoMixPlayerCharacter::OnRemoveCaughtTag()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	SetActorRelativeRotation(FRotator(0.0, GetActorRotation().Yaw, 0.0));
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
