// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/SMTeam.h"
#include "Interfaces/SMTeamInterface.h"

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

	virtual void PostInitializeComponents() override;

	void TileTrigger(ESMTeam InTeam);

	FORCEINLINE virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

	FORCEINLINE UBoxComponent* GetBoxComponent() const { return BoxComponent; }

	FVector GetTileLocation() const;

	FOnChangeTileWithTeamInformationSignature OnChangeTileWithTeamInformation;

	/** 타일이 변경될 때를 알리는 델리게이트 입니다. 서버, 클라이언트 모두 호출됩니다. */
	UPROPERTY(BlueprintAssignable)
	FOnChangeTileSignature OnChangeTile;

protected:
	UFUNCTION()
	void OnChangeTeamCallback();

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> TileMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Design")
	TObjectPtr<USMTileAssetData> DataAsset;

	/** 데이터에셋의 타일 머티리얼을 적용할지 여부입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bUseDataAssetTileMaterial:1 = true;
};
