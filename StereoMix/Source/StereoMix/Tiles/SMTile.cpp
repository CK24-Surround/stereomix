// Copyright Surround, Inc. All Rights Reserved.


#include "SMTile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SMTeamComponent.h"
#include "Data/SMTileAssetData.h"
#include "Utilities/SMAssetPath.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMTile::ASMTile()
{
	bReplicates = true;

	static ConstructorHelpers::FObjectFinder<USMTileAssetData> TileAssetDataRef(SMAssetPath::TileAssetData);
	if (TileAssetDataRef.Object)
	{
		AssetData = TileAssetDataRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TileAssetDataRef 로드에 실패했습니다."));
	}

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetComponentTickEnabled(false);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Tile);
	BoxComponent->SetRelativeLocation(FVector(75.0, 75.0, 50.0));
	BoxComponent->InitBoxExtent(FVector(75.0, 75.0, 50.0));

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMeshComponent"));
	TileMesh->SetupAttachment(BoxComponent);
	TileMesh->SetComponentTickEnabled(false);
	TileMesh->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	TileMesh->SetGenerateOverlapEvents(false);
	TileMesh->SetRelativeLocation(FVector(0.0, 0.0, 100.0 - BoxComponent->GetScaledBoxExtent().Z));

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("TeamComponent"));
}

void ASMTile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TeamComponent->OnChangeTeam.AddDynamic(this, &ASMTile::OnChangeTeamCallback);
}

void ASMTile::TileTrigger(ESMTeam InTeam)
{
	if (HasAuthority())
	{
		const ESMTeam CurrentTeam = TeamComponent->GetTeam();
		if (CurrentTeam != InTeam)
		{
			TeamComponent->SetTeam(InTeam);
			OnChangeTile.Broadcast(CurrentTeam, InTeam);
		}
	}
}

void ASMTile::OnChangeTeamCallback()
{
	const ESMTeam Team = TeamComponent->GetTeam();

	if (!HasAuthority())
	{
		TileMesh->SetMaterial(0, AssetData->TileMaterial[Team]);
		TileMesh->SetMaterial(1, AssetData->TileMaterial[Team]);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AssetData->TileChangeFX[Team], TileMesh->GetComponentLocation(), FRotator::ZeroRotator, FVector(1.0), false, true, ENCPoolMethod::AutoRelease);
	}
}

ESMTeam ASMTile::GetTeam() const
{
	return TeamComponent->GetTeam();
}

FVector ASMTile::GetTileLocation()
{
	return TileMesh->GetComponentLocation();
}
