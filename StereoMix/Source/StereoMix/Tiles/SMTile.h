// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SMTeamComponentInterface.h"
#include "Utilities/SMTeam.h"
#include "SMTile.generated.h"

class USMTeamComponent;
class USMTileAssetData;
class UBoxComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangeTileSignature, ESMTeam /*PreviousTeam*/, ESMTeam /*NewTeam*/);

UCLASS()
class STEREOMIX_API ASMTile : public AActor, public ISMTeamComponentInterface
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
	FORCEINLINE UBoxComponent* GetBoxComponent() const { return BoxComponent; }

	FVector GetTileLocation();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> FrameMesh;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> TileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY()
	TObjectPtr<USMTileAssetData> AssetData;

public:
	FOnChangeTileSignature OnChangeTile;
};
