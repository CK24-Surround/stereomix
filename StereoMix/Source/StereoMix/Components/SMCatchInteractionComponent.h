// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMCatchInteractionComponent.generated.h"


UCLASS(Abstract, ClassGroup=(CatchInteraction), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCatchInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMCatchInteractionComponent();

public:
	AActor* GetActorCatchingMe() { return CatchingMeActor.Get(); }

	void SetActorCatchingMe(AActor* NewActorCatchingMe) { CatchingMeActor = NewActorCatchingMe; }

	virtual bool IsCatchble(AActor* TargetActor) PURE_VIRTUAL(USMCatchInteractionComponent::IsCatchble, return false;)

	virtual void OnSpecialActionPerformed(AActor* TargetActor) PURE_VIRTUAL(USMCatchInteractionComponent::OnSpecialActionPerformed)

	virtual void OnSpecialActionEnded(AActor* TargetActor) PURE_VIRTUAL(USMCatchInteractionComponent::OnSpecialActionEnded)

protected:
	/** 자신을 잡고 있는 액터입니다. */
	TWeakObjectPtr<AActor> CatchingMeActor;
};
