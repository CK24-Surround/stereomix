// Copyright Studio Surround. All Rights Reserved.


#include "SMEditorSettings.h"

#include "GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMEditorSettings)

#define LOCTEXT_NAMESPACE "StereoMixEditorSettings"

FName USMEditorSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

#if WITH_EDITOR
void USMEditorSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ApplySettings();
}

void USMEditorSettings::PostReloadConfig(FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);
	ApplySettings();
}

void USMEditorSettings::PostInitProperties()
{
	Super::PostInitProperties();
	ApplySettings();
}

void USMEditorSettings::ApplySettings()
{
}
#endif

#undef LOCTEXT_NAMESPACE