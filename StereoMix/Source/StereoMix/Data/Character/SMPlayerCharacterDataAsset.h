// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/SMTeam.h"
#include "SMPlayerCharacterDataAsset.generated.h"

class USMUserWidget_CharacterState;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMPlayerCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<ESMTeam, TSubclassOf<USMUserWidget_CharacterState>> CharacterStateWidget;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> DefaultMoveTrailFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> CatchMoveTrailFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> ImmuneMoveTrailFX;

	float MoveSpeed = 600.0f;
};
