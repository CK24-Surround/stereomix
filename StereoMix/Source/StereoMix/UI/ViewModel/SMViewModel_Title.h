// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMViewModel_Title.generated.h"

/**
 * Title ViewModel
 */
UCLASS()
class STEREOMIX_API USMViewModel_Title : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, meta=(AllowPrivateAccess))
	FText ConnectionStatus;

public:
	const FText& GetConnectionStatus() const { return ConnectionStatus; }
	void SetConnectionStatus(const FText& NewConnectionStatus) { UE_MVVM_SET_PROPERTY_VALUE(ConnectionStatus, NewConnectionStatus); }
};
