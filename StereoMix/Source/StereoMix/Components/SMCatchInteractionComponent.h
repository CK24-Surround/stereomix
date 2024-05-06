// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/SMCatchableInterface.h"
#include "SMCatchInteractionComponent.generated.h"


UCLASS(Abstract, ClassGroup=(CatchInteraction), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCatchInteractionComponent : public UActorComponent, public ISMCatchInteractionInterface
{
	GENERATED_BODY()

public:
	USMCatchInteractionComponent();

public:
	FORCEINLINE virtual AActor* GetActorCatchingMe() const override { return CatchingMeActor.Get(); }

	FORCEINLINE virtual void SetActorCatchingMe(AActor* NewActorCatchingMe) override { CatchingMeActor = NewActorCatchingMe; }

	virtual bool IsCatchble(AActor* TargetActor) const override PURE_VIRTUAL(USMCatchInteractionComponent::IsCatchble, return false;)

	virtual bool OnCaught(AActor* TargetActor) override PURE_VIRTUAL(USMCatchInteractionComponent::IsCatchble, return false;)

	virtual bool OnCaughtReleased(AActor* TargetActor, bool bIsStunTimeOut) override PURE_VIRTUAL(USMCatchInteractionComponent::IsCatchble, return false;)

	virtual void OnSpecialActionPerformed(AActor* TargetActor) override PURE_VIRTUAL(USMCatchInteractionComponent::OnSpecialActionPerformed)

	virtual void OnSpecialActionEnded(AActor* TargetActor) override PURE_VIRTUAL(USMCatchInteractionComponent::OnSpecialActionEnded)

protected:
	/** 자신을 잡고 있는 액터입니다. */
	TWeakObjectPtr<AActor> CatchingMeActor;
};
