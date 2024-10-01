// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Items/SMItemBase.h"
#include "SMOverlapItemBase.generated.h"

class UGameplayEffect;

UCLASS(Abstract)
class STEREOMIX_API ASMOverlapItemBase : public ASMItemBase
{
	GENERATED_BODY()

public:
	ASMOverlapItemBase();

	virtual void PostInitializeComponents() override;

	virtual void ActivateItem(AActor* InActivator) override;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<UGameplayEffect> GE;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	float ActivateDelay = 0.5f;

private:
	FGameplayTagContainer UnavailableTags;
};
