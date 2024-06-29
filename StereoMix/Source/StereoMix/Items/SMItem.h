// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SMItem.generated.h"

DECLARE_DELEGATE(FOnUsedItemSignature);

UCLASS(Abstract)
class STEREOMIX_API ASMItem : public AActor
{
	GENERATED_BODY()

public:
	ASMItem();

	/** 아이템을 활성화합니다. 서브클래스에서 이를 상속하고 구현해줘야합니다. */
	virtual void ActivateItem(AActor* InActivator);

	FOnUsedItemSignature OnUsedItem;
};
