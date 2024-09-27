// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyBase.h"
#include "SMGCN_OneShot.generated.h"

/**
 * 타겟 컴포넌트가 존재하면 어태치되어 재생되고 nullptr이면 해당 위치에 재생됩니다.
 */
UCLASS()
class STEREOMIX_API USMGCN_OneShot : public USMGameplayCueNotifyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bUseRotationAttach:1 = false;
};
