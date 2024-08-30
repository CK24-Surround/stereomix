// Copyright Surround, Inc. All Rights Reserved.


#include "SMPianoCharacter.h"


// Sets default values
ASMPianoCharacter::ASMPianoCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASMPianoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASMPianoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASMPianoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

