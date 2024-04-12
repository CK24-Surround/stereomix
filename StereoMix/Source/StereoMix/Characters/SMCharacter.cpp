// Fill out your copyright notice in the Description page of Project Settings.


#include "SMCharacter.h"


ASMCharacter::ASMCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASMCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

