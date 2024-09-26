// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "SMVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FSMVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;

UCLASS()
class STEREOMIX_API USMVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Lyra")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Lyra")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Lyra")
	static FGameplayCueParameters VerbMessageToCueParameters(const FSMVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Lyra")
	static FSMVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
