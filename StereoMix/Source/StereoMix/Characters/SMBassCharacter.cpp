// Copyright Surround, Inc. All Rights Reserved.


#include "SMBassCharacter.h"

#include "EnhancedInputComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/SMGamePlayerController.h"
#include "Data/SMControlData.h"
#include "Slash/SMSlashComponent.h"


ASMBassCharacter::ASMBassCharacter()
{
	SlashComponent = CreateDefaultSubobject<USMSlashComponent>(TEXT("SlashComponent"));

	SlashColliderRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SlashColliderRootComponent"));
	SlashColliderRootComponent->SetupAttachment(RootComponent);

	SlashColliderComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SlashColliderComponent"));
	SlashColliderComponent->SetupAttachment(SlashColliderRootComponent);
	SlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	const double HalfDistance = 250.0;
	SlashColliderComponent->SetRelativeLocationAndRotation(FVector(HalfDistance, 0.0, 0.0), FRotator(-90.0, 0.0, 0.0));
	SlashColliderComponent->InitCapsuleSize(50.0f, HalfDistance);
}

void ASMBassCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASMBassCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		const USMControlData* ControlData = SMPlayerController->GetControlData();
		if (ControlData)
		{
			EnhancedInputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Triggered, this, &ThisClass::Slash);
		}
	}
}

void ASMBassCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASMBassCharacter::Slash()
{
	SlashComponent->TrySlash();
}
