// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "Characters/SMPlayerCharacter.h"
#include "Data/SMTeam.h"
#include "SMGameplayCueNotify_ImmuneMaterial.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayCueNotify_ImmuneMaterial : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	// 면역 머티리얼을 적용합니다.
	void ApplyMaterial(ASMPlayerCharacter* SourceCharacter) const;

	// 머티리얼을 원상복구합니다.
	void ResetMaterial(ASMPlayerCharacter* SourceCharacter) const;

protected:
	/** 적용할지 적용을 해제할지 여부입니다. */
	UPROPERTY(EditAnywhere, Category = "Asset")
	uint32 bIsApply:1 = true;

	/** 먼역 머티리얼이 적용되야할 캐릭터 메시 머티리얼 인덱스입니다.*/
	UPROPERTY(EditAnywhere, Category = "Asset", meta = (EditCondition = "bIsApply", EditConditionHides))
	TArray<int32> ApplyIndexNumbers = {0, 1, 2, 3};

	UPROPERTY(EditAnywhere, Category = "Asset", meta = (EditCondition = "bIsApply", EditConditionHides))
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> ImmuneMaterial;
};
