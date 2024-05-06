// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SMCatchableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USMCatchInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEREOMIX_API ISMCatchInteractionInterface
{
	GENERATED_BODY()

public:
	/** 자신을 잡고 있는 액터를 반환합니다. 서버에서만 유효합니다. */
	virtual AActor* GetActorCatchingMe() = 0;

	/** 자신을 잡고 있는 액터를 할당합니다. 서버에서만 유효합니다. */
	virtual void SetActorCatchingMe(AActor* NewActorCatchingMe) = 0;

	/** 잡을 수 있는지 여부를 반환하도록 구현해야합니다. 서버에서만 유효합니다. */
	virtual bool IsCatchble(AActor* TargetActor) = 0;

public:
	/** 타겟에게 잡힐때 필요한 로직을 구현해야합니다. 성공 여부를 반환합니다. 서버에서 호출됩니다. */
	virtual bool OnCaught(AActor* TargetActor) = 0;

	/** 타겟으로부터 잡히기가 풀릴때 필요한 로직을 구현해야합니다. 성공 여부를 반환합니다. 서버에서 호출됩니다. */
	virtual bool OnCaughtReleased(AActor* TargetActor) = 0;

	/** 특수 액션 실행 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnSpecialActionPerformed(AActor* TargetActor) = 0;

	/** 특수 액션 종료 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnSpecialActionEnded(AActor* TargetActor) = 0;
};
