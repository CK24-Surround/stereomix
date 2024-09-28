// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "SMDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USMDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class STEREOMIX_API ISMDamageInterface
{
	GENERATED_BODY()

public:
	/** 마지막으로 공격한 인스티게이터를 가져옵니다. */
	virtual AActor* GetLastAttacker() const = 0;

	/** 마지막으로 공격한 인스티게이터를 저장할때 사용합니다. */
	virtual void SetLastAttacker(AActor* NewAttacker) = 0;

	/**
	 * 대미지량을 전달받습니다. 대미지를 받을 수 있는 대상은 이 함수를 통해서 로직이 시작됩니다. SetLastAttacker가 먼저 호출되도록 설계해야합니다.
	 * @param NewAttacker SetLastAttacker에 사용하기 위한 매개변수입니다.
	 */
	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) = 0;

	/** 이펙트 처리를 위한 장애물 여부입니다. */
	virtual bool IsObstacle() = 0;
};
