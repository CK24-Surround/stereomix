// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMViewModel_Title.generated.h"

enum class EGrpcServiceState : uint8;

UENUM()
enum class ETitleConnectionStatus
{
	None,
	Connecting,
	Connected,
	ConnectionFailed,
	LoggingIn,
	LoginSuccess,
	LoginFailed,
};

/**
 * Title ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_Title : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	FText ConnectionStatusText;

public:
	USMViewModel_Title();

	const FText& GetConnectionStatusText() const { return ConnectionStatusText; }

	void SetConnectionStatus(const ETitleConnectionStatus NewStatus);

protected:
	void SetConnectionStatusText(const FText& NewConnectionStatusText) { UE_MVVM_SET_PROPERTY_VALUE(ConnectionStatusText, NewConnectionStatusText); }
};
