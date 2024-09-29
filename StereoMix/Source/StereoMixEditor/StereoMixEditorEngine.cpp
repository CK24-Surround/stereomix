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
		if (WorldSettings->bForceUseNetMode)
		{
			EPlayNetMode OutPlayNetMode;
			PlaySessionRequest->EditorPlaySettings->GetPlayNetMode(OutPlayNetMode);
			EPlayNetMode NetModeOverride = static_cast<EPlayNetMode>(WorldSettings->NetModeOverride);
			if (OutPlayNetMode != NetModeOverride)
			{
				PlaySessionRequest->EditorPlaySettings->SetPlayNetMode(NetModeOverride);

				static const FText ForcingOverrideNetModeFormat = LOCTEXT("ForcingOverrideNetMode", "Forcing NetMode: Override PIE NetMode to {NetMode}");
				FText NetModeText;
				switch (NetModeOverride)
				{
					case PIE_Standalone:
						NetModeText = LOCTEXT("NetModeStandalone", "Standalone");
						break;
					case PIE_ListenServer:
						NetModeText = LOCTEXT("NetModeListenServer", "ListenServer");
						break;
					case PIE_Client:
						NetModeText = LOCTEXT("NetModeClient", "Client");
						break;
				}
				FText Message = FText::FormatNamed(ForcingOverrideNetModeFormat, TEXT("NetMode"), NetModeText);
				FNotificationInfo Info(Message);
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
