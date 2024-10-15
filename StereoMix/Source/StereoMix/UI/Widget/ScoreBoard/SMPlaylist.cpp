// Copyright Studio Surround. All Rights Reserved.


#include "SMPlaylist.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "Data/SMTeam.h"


void USMPlaylist::ResetTeamInfo() const
{
	WinTeamScore->SetText(FText::FromString("0"));
	LoseTeamScore->SetText(FText::FromString("0"));

	WinTeamPlayerName1->SetText(FText::FromString(""));
	WinTeamPlayerName2->SetText(FText::FromString(""));
	WinTeamPlayerName3->SetText(FText::FromString(""));

	LoseTeamPlayerName1->SetText(FText::FromString(""));
	LoseTeamPlayerName2->SetText(FText::FromString(""));
	LoseTeamPlayerName3->SetText(FText::FromString(""));

	EDMRowName1->SetText(FText::FromString(""));
	EDMRowName2->SetText(FText::FromString(""));
	EDMRowName3->SetText(FText::FromString(""));

	FBRowName1->SetText(FText::FromString(""));
	FBRowName2->SetText(FText::FromString(""));
	FBRowName3->SetText(FText::FromString(""));
}

void USMPlaylist::SetVictoryTeam(ESMTeam WinTeam)
{
	EDMWinTeamBackground->SetVisibility(WinTeam == ESMTeam::EDM ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	FBWinTeamBackground->SetVisibility(WinTeam == ESMTeam::FutureBass ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	WinTeamType->SetText(WinTeam == ESMTeam::EDM ? FText::FromString("Inferno Beat") : FText::FromString("Chewing Cats"));
	LoseTeamType->SetText(WinTeam == ESMTeam::EDM ? FText::FromString("Chewing Cats") : FText::FromString("Inferno Beat"));

	TMap<ESMCharacterType, FString> PlayerTypeMap = {
		{ ESMCharacterType::None, "None" },
		{ ESMCharacterType::ElectricGuitar, "ElectricGuitar" },
		{ ESMCharacterType::Piano, "Piano" },
		{ ESMCharacterType::Bass, "Bass" }
	};

	ResetTeamInfo();

	ChangePlaylist(WinTeam);
}

void USMPlaylist::ChangePlaylist(ESMTeam WinTeam)
{
	TObjectPtr<UCommonTextBlock> EDMTeamScore = LoseTeamScore;
	TObjectPtr<UCommonTextBlock> EDMPlayerName1 = LoseTeamPlayerName1;
	TObjectPtr<UCommonTextBlock> EDMPlayerName2 = LoseTeamPlayerName2;
	TObjectPtr<UCommonTextBlock> EDMPlayerName3 = LoseTeamPlayerName3;

	TObjectPtr<UCommonTextBlock> FBTeamScore = LoseTeamScore;
	TObjectPtr<UCommonTextBlock> FBPlayerName1 = LoseTeamPlayerName1;
	TObjectPtr<UCommonTextBlock> FBPlayerName2 = LoseTeamPlayerName2;
	TObjectPtr<UCommonTextBlock> FBPlayerName3 = LoseTeamPlayerName3;

	switch (WinTeam)
	{
		case ESMTeam::EDM:
			EDMTeamScore = WinTeamScore;
			EDMPlayerName1 = WinTeamPlayerName1;
			EDMPlayerName2 = WinTeamPlayerName2;
			EDMPlayerName3 = WinTeamPlayerName3;
			break;
		case ESMTeam::FutureBass:
			FBTeamScore = WinTeamScore;
			FBPlayerName1 = WinTeamPlayerName1;
			FBPlayerName2 = WinTeamPlayerName2;
			FBPlayerName3 = WinTeamPlayerName3;
			break;
		default:
			break;
	}

	int TotalEDMTeamScore = 0;
	if (EDMAnalysisData.Num() > 0)
	{
		TotalEDMTeamScore += EDMAnalysisData[0].TotalScore();
		EDMPlayerName1->SetText(FText::FromString(EDMAnalysisData[0].PlayerName));
		EDMRowName1->SetText(FText::FromString(EDMAnalysisData[0].PlayerName));
	}
	if (EDMAnalysisData.Num() > 1)
	{
		TotalEDMTeamScore += EDMAnalysisData[1].TotalScore();
		EDMPlayerName2->SetText(FText::FromString(EDMAnalysisData[1].PlayerName));
		EDMRowName2->SetText(FText::FromString(EDMAnalysisData[1].PlayerName));
	}
	if (EDMAnalysisData.Num() > 2)
	{
		TotalEDMTeamScore += EDMAnalysisData[2].TotalScore();
		EDMPlayerName3->SetText(FText::FromString(EDMAnalysisData[2].PlayerName));
		EDMRowName3->SetText(FText::FromString(EDMAnalysisData[2].PlayerName));
	}
	EDMTeamScore->SetText(FText::FromString(FString::FromInt(TotalEDMTeamScore)));

	int TotalFBTeamScore = 0;
	if (FBAnalysisData.Num() > 0)
	{
		TotalFBTeamScore += FBAnalysisData[0].TotalScore();
		FBPlayerName1->SetText(FText::FromString(FBAnalysisData[0].PlayerName));
		FBRowName1->SetText(FText::FromString(FBAnalysisData[0].PlayerName));
	}
	if (FBAnalysisData.Num() > 1)
	{
		TotalFBTeamScore += FBAnalysisData[1].TotalScore();
		FBPlayerName2->SetText(FText::FromString(FBAnalysisData[1].PlayerName));
		FBRowName2->SetText(FText::FromString(FBAnalysisData[1].PlayerName));
	}
	if (FBAnalysisData.Num() > 2)
	{
		TotalFBTeamScore += FBAnalysisData[2].TotalScore();
		FBPlayerName3->SetText(FText::FromString(FBAnalysisData[2].PlayerName));
		FBRowName3->SetText(FText::FromString(FBAnalysisData[2].PlayerName));
	}
	FBTeamScore->SetText(FText::FromString(FString::FromInt(TotalFBTeamScore)));
}

void USMPlaylist::ChangeOverview()
{
	OverviewColumn4_Title->SetText(FText::FromString(TEXT("점수")));
	OverviewColumn5_Title->SetText(FText::FromString(TEXT("점령한 타일 수")));
	OverviewColumn6_Title->SetText(FText::FromString(TEXT("가한 피해량")));
	OverviewColumn7_Title->SetText(FText::FromString(TEXT("무력화 시킨 수")));

	if (EDMAnalysisData.Num() > 0)
	{
		OverviewColumn4_1->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[0].TotalScore())));
		OverviewColumn5_1->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[0].TotalCapturedTiles)));
		OverviewColumn6_1->SetText(FText::FromString(FString::SanitizeFloat(EDMAnalysisData[0].TotalDamageDealt)));
		OverviewColumn7_1->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[0].TotalKillCount)));
	}
	if (EDMAnalysisData.Num() > 1)
	{
		OverviewColumn4_2->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[1].TotalScore())));
		OverviewColumn5_2->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[1].TotalCapturedTiles)));
		OverviewColumn6_2->SetText(FText::FromString(FString::SanitizeFloat(EDMAnalysisData[1].TotalDamageDealt)));
		OverviewColumn7_2->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[1].TotalKillCount)));
	}
	if (EDMAnalysisData.Num() > 2)
	{
		OverviewColumn4_3->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[2].TotalScore())));
		OverviewColumn5_3->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[2].TotalCapturedTiles)));
		OverviewColumn6_3->SetText(FText::FromString(FString::SanitizeFloat(EDMAnalysisData[2].TotalDamageDealt)));
		OverviewColumn7_3->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[2].TotalKillCount)));
	}

	if (FBAnalysisData.Num() > 0)
	{
		OverviewColumn4_4->SetText(FText::FromString(FString::FromInt(FBAnalysisData[0].TotalScore())));
		OverviewColumn5_4->SetText(FText::FromString(FString::FromInt(FBAnalysisData[0].TotalCapturedTiles)));
		OverviewColumn6_4->SetText(FText::FromString(FString::SanitizeFloat(FBAnalysisData[0].TotalDamageDealt)));
		OverviewColumn7_4->SetText(FText::FromString(FString::FromInt(FBAnalysisData[0].TotalKillCount)));
	}
	if (FBAnalysisData.Num() > 1)
	{
		OverviewColumn4_5->SetText(FText::FromString(FString::FromInt(FBAnalysisData[1].TotalScore())));
		OverviewColumn5_5->SetText(FText::FromString(FString::FromInt(FBAnalysisData[1].TotalCapturedTiles)));
		OverviewColumn6_5->SetText(FText::FromString(FString::SanitizeFloat(FBAnalysisData[1].TotalDamageDealt)));
		OverviewColumn7_5->SetText(FText::FromString(FString::FromInt(FBAnalysisData[1].TotalKillCount)));
	}
	if (FBAnalysisData.Num() > 2)
	{
		OverviewColumn4_6->SetText(FText::FromString(FString::FromInt(FBAnalysisData[2].TotalScore())));
		OverviewColumn5_6->SetText(FText::FromString(FString::FromInt(FBAnalysisData[2].TotalCapturedTiles)));
		OverviewColumn6_6->SetText(FText::FromString(FString::SanitizeFloat(FBAnalysisData[2].TotalDamageDealt)));
		OverviewColumn7_6->SetText(FText::FromString(FString::FromInt(FBAnalysisData[2].TotalKillCount)));
	}
}

void USMPlaylist::ChangeAnalytics()
{
	OverviewColumn1_Title->SetText(FText::FromString(TEXT("")));
	OverviewColumn2_Title->SetText(FText::FromString(TEXT("점령한 타일 수")));
	OverviewColumn3_Title->SetText(FText::FromString(TEXT("가한 피해량")));
	OverviewColumn4_Title->SetText(FText::FromString(TEXT("무력화 시킨 수")));
	OverviewColumn5_Title->SetText(FText::FromString(TEXT("받은 피해량")));
	OverviewColumn6_Title->SetText(FText::FromString(TEXT("무력화 된 수")));
	OverviewColumn7_Title->SetText(FText::FromString(TEXT("노브 시전 횟수")));

	if (EDMAnalysisData.Num() > 0)
	{
		OverviewColumn1_1->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[0].TotalScore())));
		OverviewColumn2_1->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[0].TotalCapturedTiles)));
		OverviewColumn3_1->SetText(FText::FromString(FString::SanitizeFloat(EDMAnalysisData[0].TotalDamageDealt)));
		OverviewColumn4_1->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[0].TotalKillCount)));
		OverviewColumn5_1->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[0].TotalDamageReceived)));
		OverviewColumn6_1->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[0].TotalDeathCount)));
		OverviewColumn7_1->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[0].TotalNoiseBreakUsage)));
	}
	if (EDMAnalysisData.Num() > 1)
	{
		OverviewColumn1_2->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[1].TotalScore())));
		OverviewColumn2_2->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[1].TotalCapturedTiles)));
		OverviewColumn3_2->SetText(FText::FromString(FString::SanitizeFloat(EDMAnalysisData[1].TotalDamageDealt)));
		OverviewColumn4_2->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[1].TotalKillCount)));
		OverviewColumn5_2->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[1].TotalDamageReceived)));
		OverviewColumn6_2->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[1].TotalDeathCount)));
		OverviewColumn7_2->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[1].TotalNoiseBreakUsage)));
	}
	if (EDMAnalysisData.Num() > 2)
	{
		OverviewColumn1_3->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[2].TotalScore())));
		OverviewColumn2_3->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[2].TotalCapturedTiles)));
		OverviewColumn3_3->SetText(FText::FromString(FString::SanitizeFloat(EDMAnalysisData[2].TotalDamageDealt)));
		OverviewColumn4_3->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[2].TotalKillCount)));
		OverviewColumn5_3->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[2].TotalDamageReceived)));
		OverviewColumn6_3->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[2].TotalDeathCount)));
		OverviewColumn7_3->SetText(FText::FromString(FString::FromInt(EDMAnalysisData[2].TotalNoiseBreakUsage)));
	}

	if (FBAnalysisData.Num() > 0)
	{
		OverviewColumn1_4->SetText(FText::FromString(FString::FromInt(FBAnalysisData[0].TotalScore())));
		OverviewColumn2_4->SetText(FText::FromString(FString::FromInt(FBAnalysisData[0].TotalCapturedTiles)));
		OverviewColumn3_4->SetText(FText::FromString(FString::SanitizeFloat(FBAnalysisData[0].TotalDamageDealt)));
		OverviewColumn4_4->SetText(FText::FromString(FString::FromInt(FBAnalysisData[0].TotalKillCount)));
		OverviewColumn5_4->SetText(FText::FromString(FString::FromInt(FBAnalysisData[0].TotalDamageReceived)));
		OverviewColumn6_4->SetText(FText::FromString(FString::FromInt(FBAnalysisData[0].TotalDeathCount)));
		OverviewColumn7_4->SetText(FText::FromString(FString::FromInt(FBAnalysisData[0].TotalNoiseBreakUsage)));
	}
	if (FBAnalysisData.Num() > 1)
	{
		OverviewColumn1_5->SetText(FText::FromString(FString::FromInt(FBAnalysisData[1].TotalScore())));
		OverviewColumn2_5->SetText(FText::FromString(FString::FromInt(FBAnalysisData[1].TotalCapturedTiles)));
		OverviewColumn3_5->SetText(FText::FromString(FString::SanitizeFloat(FBAnalysisData[1].TotalDamageDealt)));
		OverviewColumn4_5->SetText(FText::FromString(FString::FromInt(FBAnalysisData[1].TotalKillCount)));
		OverviewColumn5_5->SetText(FText::FromString(FString::FromInt(FBAnalysisData[1].TotalDamageReceived)));
		OverviewColumn6_5->SetText(FText::FromString(FString::FromInt(FBAnalysisData[1].TotalDeathCount)));
		OverviewColumn7_5->SetText(FText::FromString(FString::FromInt(FBAnalysisData[1].TotalNoiseBreakUsage)));
	}
	if (FBAnalysisData.Num() > 2)
	{
		OverviewColumn1_6->SetText(FText::FromString(FString::FromInt(FBAnalysisData[2].TotalScore())));
		OverviewColumn2_6->SetText(FText::FromString(FString::FromInt(FBAnalysisData[2].TotalCapturedTiles)));
		OverviewColumn3_6->SetText(FText::FromString(FString::SanitizeFloat(FBAnalysisData[2].TotalDamageDealt)));
		OverviewColumn4_6->SetText(FText::FromString(FString::FromInt(FBAnalysisData[2].TotalKillCount)));
		OverviewColumn5_6->SetText(FText::FromString(FString::FromInt(FBAnalysisData[2].TotalDamageReceived)));
		OverviewColumn6_6->SetText(FText::FromString(FString::FromInt(FBAnalysisData[2].TotalDeathCount)));
		OverviewColumn7_6->SetText(FText::FromString(FString::FromInt(FBAnalysisData[2].TotalNoiseBreakUsage)));
	}
}

void USMPlaylist::MulticastShowPlaylist_Implementation(ESMTeam WinTeam, const TArray<FPlayerScoreData>& EDMPlayerData, const TArray<FPlayerScoreData>& FBPlayerData)
{
	if (EDMPlayerData.Num() > 0 || FBPlayerData.Num() > 0)
	{
		EDMAnalysisData = EDMPlayerData;
		FBAnalysisData = FBPlayerData;

		AddToViewport(2);

		SetVictoryTeam(WinTeam);
	}
}
