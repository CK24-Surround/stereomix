// Copyright Surround, Inc. All Rights Reserved.


#include "SMPianoCharacter.h"


ASMPianoCharacter::ASMPianoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASMPianoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASMPianoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASMPianoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

