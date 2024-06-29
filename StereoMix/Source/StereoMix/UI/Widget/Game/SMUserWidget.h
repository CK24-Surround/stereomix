// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Blueprint/UserWidget.h"
#include "CommonUserWidget.h"

#include "SMUserWidget.generated.h"

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMUserWidget : public UCommonUserWidget, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/** 위젯 컴포넌트에 위젯이 생성될때 호출되며 ASC가 할당됩니다. */
	virtual void SetASC(UAbilitySystemComponent* InASC);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
};
