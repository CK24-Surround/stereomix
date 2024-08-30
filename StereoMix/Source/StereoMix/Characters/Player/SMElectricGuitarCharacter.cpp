// Copyright Surround, Inc. All Rights Reserved.


#include "SMElectricGuitarCharacter.h"


// Sets default values
ASMElectricGuitarCharacter::ASMElectricGuitarCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASMElectricGuitarCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASMElectricGuitarCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASMElectricGuitarCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

