// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchInteractionComponent.h"
#include "SMCatchInteractionComponent_Character.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCatchInteractionComponent_Character : public USMCatchInteractionComponent
{
	GENERATED_BODY()

public:
	USMCatchInteractionComponent_Character();

public:
	AActor* GetActorIAmCatching() { return IAmCatchingActor.Get(); }

	void SetActorIAmCatching(AActor* NewIAmCathingActor) { IAmCatchingActor = NewIAmCathingActor; }

protected:
	/** 자신이 잡고 있는 액터입니다. */
	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> IAmCatchingActor;

	/** 한 캐릭터에게 여러번 잡히지 않도록 자신을 잡았던 캐릭터들을 담아둡니다. 서버에서만 유효합니다. */
	TArray<TWeakObjectPtr<ASMPlayerCharacter>> CapturedCharcters;
};
