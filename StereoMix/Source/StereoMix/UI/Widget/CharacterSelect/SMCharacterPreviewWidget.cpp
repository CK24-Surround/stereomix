// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterPreviewWidget.h"

void USMCharacterPreviewWidget::SetPreviewImage(ESMTeam Team, ESMCharacterType CharacterType)
{
	if (Team == ESMTeam::EDM || Team == ESMTeam::None)
	{
		for (const auto& [TargetCharacterType, TargetCharacterMaterial] : EdmCharacterPreviewDataList)
		{
			if (TargetCharacterType == CharacterType)
			{
				PreviewImage->SetBrushFromMaterial(TargetCharacterMaterial);
				SetVisibility(ESlateVisibility::HitTestInvisible);
				return;
			}
		}
	}
	else if (Team == ESMTeam::FutureBass)
	{
		for (const auto& [TargetCharacterType, TargetCharacterMaterial] : FutureBassCharacterPreviewDataList)
		{
			if (TargetCharacterType == CharacterType)
			{
				PreviewImage->SetBrushFromMaterial(TargetCharacterMaterial);
				SetVisibility(ESlateVisibility::HitTestInvisible);
				return;
			}
		}
	}

	// 조건에 맞는 캐릭터 프리뷰가 없을 경우
	SetVisibility(ESlateVisibility::Collapsed);
}
