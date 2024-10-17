// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "SMPlaylist.generated.h"


class UOverlay;
class UCommonTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMPlaylist : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ResetTeamInfo() const;

	void SetVictoryTeam(ESMTeam WinTeam);

	UFUNCTION(BlueprintCallable)
	void ChangePlaylist(ESMTeam WinTeam);

	UFUNCTION(BlueprintCallable)
	void ChangeOverview();

	UFUNCTION(BlueprintCallable)
	void ChangeAnalytics();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowPlaylist(ESMTeam WinTeam, const TArray<FPlayerScoreData>& EDMPlayerData, const TArray<FPlayerScoreData>& FBPlayerData);

protected:
	void SetOverviewColumnTitles(const TArray<FString>& Titles) const;

	static void SetOverviewRowNames(TArray<TObjectPtr<UCommonTextBlock>>& Row, const TArray<FString>& Names);

	TArray<FPlayerScoreData> EDMAnalysisData;

	TArray<FPlayerScoreData> FBAnalysisData;

	TArray<TObjectPtr<UCommonTextBlock>> WinTeamPlayerNames;

	TArray<TObjectPtr<UCommonTextBlock>> LoseTeamPlayerNames;

	TArray<TObjectPtr<UCommonTextBlock>> EDMRowNames;

	TArray<TObjectPtr<UCommonTextBlock>> FBRowNames;

	TArray<TObjectPtr<UCommonTextBlock>> OverviewColumnTitles;

	TArray<TArray<TObjectPtr<UCommonTextBlock>>> OverviewRows;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> EDMWinAlbum;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> FBWinAlbum;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlaylistWinTeamResult;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlaylistLoseTeamResult;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMWinTeamBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FBWinTeamBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> WinTeamType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WinTeamPlayerType1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> WinTeamPlayerName1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WinTeamPlayerType2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> WinTeamPlayerName2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WinTeamPlayerType3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> WinTeamPlayerName3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> WinTeamScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> LoseTeamType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LoseTeamPlayerType1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> LoseTeamPlayerName1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LoseTeamPlayerType2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> LoseTeamPlayerName2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LoseTeamPlayerType3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> LoseTeamPlayerName3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> LoseTeamScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMWinCD;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FBWinCD;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewEDMTeamResult;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewFBTeamResult;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMRowProfile1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> EDMRowName1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMRowProfile2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> EDMRowName2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMRowProfile3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> EDMRowName3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FBRowProfile1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> FBRowName1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FBRowProfile2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> FBRowName2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FBRowProfile3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> FBRowName3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn1_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn1_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn1_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn1_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn1_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn1_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn1_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn2_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn2_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn2_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn2_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn2_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn2_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn2_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn3_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn3_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn3_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn3_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn3_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn3_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn3_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn4_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn4_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn4_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn4_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn4_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn4_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn4_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn5_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn5_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn5_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn5_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn5_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn5_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn5_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn6_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn6_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn6_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn6_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn6_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn6_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn6_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn7_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn7_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn7_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn7_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn7_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn7_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OverviewColumn7_6;
};
