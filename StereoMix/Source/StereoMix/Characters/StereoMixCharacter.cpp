// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixCharacter.h"


AStereoMixCharacter::AStereoMixCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AStereoMixCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AStereoMixCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStereoMixCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

