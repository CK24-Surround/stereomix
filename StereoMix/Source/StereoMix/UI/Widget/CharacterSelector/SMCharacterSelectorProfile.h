// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "SMCharacterSelectorProfile.generated.h"

class ASMPlayerState;
class UCommonTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterSelectorProfile : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProfileImage(ESMTeam Team, ESMCharacterType CharacterType, ESMCharacterType FocusCharacterType);

	void SetPlayerName(const FString& Name, bool bIsOwner);

	void SetPlayerReady(ASMPlayerState* Player, bool bIsReady);

	void RemovePlayerInfo();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileEDM1;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileEDM2;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileEDM3;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileFB1;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileFB2;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileFB3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerCharacterType;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ReadyState;
};
