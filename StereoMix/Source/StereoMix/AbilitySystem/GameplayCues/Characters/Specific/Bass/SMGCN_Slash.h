// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyBase.h"
#include "SMGCN_Slash.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGCN_Slash : public USMGameplayCueNotifyBase
{
	GENERATED_BODY()

public:
	USMGCN_Slash();

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UFMODEvent>> RightSFX;
};
