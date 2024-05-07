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
	/** 자신을 잡고 있는 액터를 반환합니다. 서버에서만 유효합니다. */
	FORCEINLINE virtual AActor* GetActorCatchingMe() const { return CatchingMeActor.Get(); }

	/** 자신을 잡고 있는 액터를 할당합니다. 서버에서만 유효합니다. */
	FORCEINLINE virtual void SetActorCatchingMe(AActor* NewActorCatchingMe) { CatchingMeActor = NewActorCatchingMe; }

	/** 잡을 수 있는지 여부를 반환하도록 구현해야합니다. 서버에서만 유효합니다. */
	virtual bool IsCatchable(AActor* TargetActor) const PURE_VIRTUAL(USMCatchInteractionComponent::IsCatchble, return false;)

	/** 타겟에게 잡힐때 필요한 로직을 구현해야합니다. 성공 여부를 반환합니다. 서버에서 호출됩니다. */
	virtual bool OnCaught(AActor* TargetActor) PURE_VIRTUAL(USMCatchInteractionComponent::IsCatchble, return false;)

	/** 타겟으로부터 잡히기가 풀릴때 필요한 로직을 구현해야합니다. 성공 여부를 반환합니다. 서버에서 호출됩니다. */
	virtual bool OnCaughtReleased(AActor* TargetActor, bool bIsStunTimeOut) PURE_VIRTUAL(USMCatchInteractionComponent::IsCatchble, return false;)

	/** 특수 액션 실행 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnSpecialActionPerformed(AActor* TargetActor) PURE_VIRTUAL(USMCatchInteractionComponent::OnSpecialActionPerformed)

	/** 특수 액션 종료 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnSpecialActionEnded(AActor* TargetActor) PURE_VIRTUAL(USMCatchInteractionComponent::OnSpecialActionEnded)

protected:
	/** 자신을 잡고 있는 액터입니다. */
	TWeakObjectPtr<AActor> CatchingMeActor;
};
