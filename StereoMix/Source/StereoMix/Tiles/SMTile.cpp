// Fill out your copyright notice in the Description page of Project Settings.


#include "SMTile.h"

ASMTile::ASMTile()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMeshComponent"));
	FrameMesh->SetupAttachment(SceneComponent);
	FrameMesh->SetRelativeLocation(FVector(50.0, 50.0, 0.0));
	
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMeshComponent"));
	TileMesh->SetupAttachment(FrameMesh);
	TileMesh->SetRelativeLocation(FVector(0.0, 0.0, 100.0));
}
