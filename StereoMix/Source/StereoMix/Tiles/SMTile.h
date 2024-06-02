// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SMTeamInterface.h"
#include "Data/SMTeam.h"
#include "SMTile.generated.h"

class USMTeamComponent;
class USMTileAssetData;
class UBoxComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangeTileWithTeamInformationSignature, ESMTeam /*PreviousTeam*/, ESMTeam /*NewTeam*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeTileSignature);

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
	/** 머티리얼로 타일 애니메이션을 넣기 위해 필요한 머티리얼을 다이나믹 머티리얼로 변경합니다. 그리고 저장합니다. */
	void ChangeMaterialInstanceDynamic();

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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MeshMIDs;

public:
	FOnChangeTileWithTeamInformationSignature OnChangeTileWithTeamInformation;

	UPROPERTY(BlueprintAssignable)
	FOnChangeTileSignature OnChangeTile;
};
