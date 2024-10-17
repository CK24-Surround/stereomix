// Copyright Surround, Inc. All Rights Reserved.


#include "SMTile.h"

#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/Common/SMTeamComponent.h"
#include "Data/DataAsset/Tile/SMTileAssetData.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMTile::ASMTile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	RootSceneComponent->Mobility = EComponentMobility::Static;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootSceneComponent);
	BoxComponent->Mobility = EComponentMobility::Static;
	BoxComponent->SetComponentTickEnabled(false);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Tile);
	BoxComponent->SetRelativeLocation(FVector(75.0, 75.0, -7.5));
	BoxComponent->InitBoxExtent(FVector(75.0, 75.0, 7.5));

	TileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMeshComponent"));
	TileMeshComponent->SetupAttachment(BoxComponent);
	TileMeshComponent->Mobility = EComponentMobility::Static;
	TileMeshComponent->SetComponentTickEnabled(false);
	TileMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	TileMeshComponent->SetRelativeLocation(FVector(0.0, 0.0, BoxComponent->GetScaledBoxExtent().Z));

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("TeamComponent"));
	TeamComponent->OnChangeTeam.AddDynamic(this, &ASMTile::OnChangeTeamCallback);
}

void ASMTile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	const UWorld* World = GetWorld();
	if (World ? World->IsGameWorld() : false)
	{
		if (bUseDataAssetTileMaterial)
		{
			for (int32 MaterialIndex = 0; MaterialIndex < TileMeshComponent->GetNumMaterials(); ++MaterialIndex)
			{
				TileMeshComponent->SetMaterial(MaterialIndex, DataAsset->DefaultTileMaterial);
			}
		}
	}
}

void ASMTile::TileTrigger(ESMTeam InTeam)
{
	if (HasAuthority())
	{
		const ESMTeam CurrentTeam = TeamComponent->GetTeam();
		if (CurrentTeam != InTeam)
		{
			TeamComponent->SetTeam(InTeam);
			OnChangeTileWithTeamInformation.Broadcast(CurrentTeam, InTeam);
			ForceNetUpdate();
		}
	}
}

ESMTeam ASMTile::GetTeam() const
{
	return TeamComponent->GetTeam();
}

FVector ASMTile::GetTileLocation() const
{
	return TileMeshComponent->GetComponentLocation();
}

void ASMTile::OnChangeTeamCallback()
{
	const ESMTeam Team = TeamComponent->GetTeam();

	if (GetNetMode() != NM_DedicatedServer && DataAsset)
	{
		for (int32 i = 0; i < TileMeshComponent->GetNumMaterials(); ++i)
		{
			TileMeshComponent->SetMaterial(i, DataAsset->CapturedTileMaterial[Team]);
		}

		TileMeshComponent->SetOverlayMaterial(DataAsset->CapturedTileOverlayMaterial[Team]);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DataAsset->TileChangeFX[Team], TileMeshComponent->GetComponentLocation(), FRotator::ZeroRotator, FVector(1.0), false, true, ENCPoolMethod::AutoRelease);
	}

	OnChangeTile.Broadcast();
}
