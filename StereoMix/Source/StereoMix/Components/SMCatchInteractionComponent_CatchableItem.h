// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchInteractionComponent.h"
#include "SMCatchInteractionComponent_CatchableItem.generated.h"


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCatchInteractionComponent_CatchableItem : public USMCatchInteractionComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	virtual bool IsCatchable(AActor* TargetActor) const override;

	virtual void OnCaught(AActor* TargetActor) override;

	virtual void OnCaughtReleased(AActor* TargetActor, bool bIsStunTimeOut) override;

protected:
	void InternalOnCaught(AActor* TargetActor);

	void InternalOnCaughtReleased(AActor* TargetActor);

protected:
	TWeakObjectPtr<AActor> SourceActor;
};
