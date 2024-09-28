// Copyright Studio Surround. All Rights Reserved.


#include "SMDeveloperSettings.h"

#include "GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMDeveloperSettings)

#define LOCTEXT_NAMESPACE "StereoMixCheats"

FName USMDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

#if WITH_EDITOR
void USMDeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ApplySettings();
}

void USMDeveloperSettings::PostReloadConfig(FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);
	ApplySettings();
}

void USMDeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();
	ApplySettings();
}

void USMDeveloperSettings::OnPlayInEditorStarted() const
{
}

void USMDeveloperSettings::ApplySettings()
{
}
#endif

#undef LOCTEXT_NAMESPACE