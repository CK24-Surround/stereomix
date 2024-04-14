// Fill out your copyright notice in the Description page of Project Settings.


#include "SMTile.h"

#include "Components/BoxComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMTile::ASMTile()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetComponentTickEnabled(false);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Tile);
	BoxComponent->SetRelativeLocation(FVector(50.0, 50.0, 55.0));
	BoxComponent->InitBoxExtent(FVector(50.0, 50.0, 55.0));

	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMeshComponent"));
	FrameMesh->SetupAttachment(SceneComponent);
	FrameMesh->SetComponentTickEnabled(false);
	FrameMesh->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	FrameMesh->SetGenerateOverlapEvents(false);
	FrameMesh->SetRelativeLocation(FVector(50.0, 50.0, 0.0));

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMeshComponent"));
	TileMesh->SetupAttachment(FrameMesh);
	TileMesh->SetComponentTickEnabled(false);
	TileMesh->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	TileMesh->SetGenerateOverlapEvents(false);
	TileMesh->SetRelativeLocation(FVector(0.0, 0.0, 100.0));
}

void ASMTile::TileTrigger(ESMTeam InTeam)
{
	FString TeamString = UEnum::GetValueAsString(TEXT("StereoMix.ESMTeam"), InTeam);
	NET_LOG(this, Warning, TEXT("%s팀이 타일을 트리거했습니다."), *TeamString);
}
