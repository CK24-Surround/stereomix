// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "SMPlayerStartBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMPlayerStartBase : public APlayerStart
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;

	static FString GetPlayerStartTagByTypeAndTeam(ESMCharacterType InType, ESMTeam InTeam);

protected:
	static FString GetStringByType(ESMCharacterType InType);

	static FString GetStringByTeam(ESMTeam InTeam);

	UPROPERTY(EditAnywhere, Category = "Design")
	ESMCharacterType CharacterType;

	UPROPERTY(EditAnywhere, Category = "Design")
	ESMTeam Team;
};
