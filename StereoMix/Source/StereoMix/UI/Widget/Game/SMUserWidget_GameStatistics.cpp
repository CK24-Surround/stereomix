// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_GameStatistics.h"

void USMUserWidget_GameStatistics::NativeConstruct()
{
	Super::NativeConstruct();
	PingTextBlock->SetText(FText::FromString(TEXT("0ms")));
}

void USMUserWidget_GameStatistics::UpdatePingText(int32 NewPing) const
{
	PingTextBlock->SetText(FText::Format(FTextFormat::FromString(TEXT("{0}ms")), NewPing));
}
