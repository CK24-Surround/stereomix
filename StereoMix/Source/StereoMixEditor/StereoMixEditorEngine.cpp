// Copyright Studio Surround. All Rights Reserved.


#include "StereoMixEditorEngine.h"

#include "Development/SMDeveloperSettings.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Engine/GameInstance.h"
#include "Games/SMWorldSettings.h"
#include "Widgets/Notifications/SNotificationList.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StereoMixEditorEngine)

#define LOCTEXT_NAMESPACE "StereoMixEditor"

FGameInstancePIEResult UStereoMixEditorEngine::PreCreatePIEInstances(const bool bAnyBlueprintErrors, const bool bStartInSpectatorMode, const float PIEStartTime, const bool bSupportsOnlinePIE, int32& InNumOnlinePIEInstances)
{
	if (const ASMWorldSettings* WorldSettings = Cast<ASMWorldSettings>(EditorWorld->GetWorldSettings()))
	{
		if (WorldSettings->bForceStandaloneNetMode)
		{
			EPlayNetMode OutPlayNetMode;
			PlaySessionRequest->EditorPlaySettings->GetPlayNetMode(OutPlayNetMode);
			if (OutPlayNetMode != PIE_Standalone)
			{
				PlaySessionRequest->EditorPlaySettings->SetPlayNetMode(PIE_Standalone);

				FNotificationInfo Info(LOCTEXT("ForcingStandaloneForFrontend", "Forcing NetMode: Standalone for the Frontend"));
				Info.ExpireDuration = 2.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
		}
	}

	//@TODO: Should add delegates that a *non-editor* module could bind to for PIE start/stop instead of poking directly
	GetDefault<USMDeveloperSettings>()->OnPlayInEditorStarted();

	return Super::PreCreatePIEInstances(bAnyBlueprintErrors, bStartInSpectatorMode, PIEStartTime, bSupportsOnlinePIE, InNumOnlinePIEInstances);
}

#undef LOCTEXT_NAMESPACE
