// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Popup/SMPopup.h"
#include "SMCreateRoomPopup.generated.h"

UENUM(BlueprintType)
enum class ECreateRoomVisibilityOption : uint8
{
	Public,
	Private
};

USTRUCT(BlueprintType)
struct FCreateRoomOptions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Options")
	ECreateRoomVisibilityOption RoomVisibility = ECreateRoomVisibilityOption::Public;
};

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCreateRoomPopup : public USMPopup
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	const FCreateRoomOptions& GetCreateRoomOptions() const { return CreateRoomOptions; }

	UFUNCTION(BlueprintPure)
	ECreateRoomVisibilityOption GetRoomVisibility() const { return CreateRoomOptions.RoomVisibility; }

	UFUNCTION(BlueprintCallable)
	void SetRoomVisibility(const ECreateRoomVisibilityOption InRoomVisibility) { CreateRoomOptions.RoomVisibility = InRoomVisibility; }

private:
	FCreateRoomOptions CreateRoomOptions;
};
