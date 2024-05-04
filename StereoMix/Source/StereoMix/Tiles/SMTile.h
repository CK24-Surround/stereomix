// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SMTeamInterface.h"
#include "Utilities/SMTeam.h"
#include "SMTile.generated.h"

class USMTeamComponent;
class USMTileAssetData;
class UBoxComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangeTileSignature, ESMTeam /*PreviousTeam*/, ESMTeam /*NewTeam*/);

UCLASS()
class STEREOMIX_API ASMTile : public AActor, public ISMTeamInterface
{
	GENERATED_BODY()

public:
	ASMTile();

protected:
	virtual void PostInitializeComponents() override;

public:
	void TileTrigger(ESMTeam InTeam);

protected:
	UFUNCTION()
	void OnChangeTeamCallback();

public:
	FORCEINLINE virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

	FORCEINLINE UBoxComponent* GetBoxComponent() const { return BoxComponent; }

	FVector GetTileLocation();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> TileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY()
	TObjectPtr<USMTileAssetData> AssetData;

public:
	FOnChangeTileSignature OnChangeTile;
};
