#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStereoMixEditor, Log, All);

class FStereoMixEditorModule : public IModuleInterface
{
	using ThisClass = FStereoMixEditorModule;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	static void BindGameplayAbilitiesEditorDelegates();
	static void UnbindGameplayAbilitiesEditorDelegates();

private:
	static void RegisterGameEditorMenus();

	// This function tells the GameplayCue editor what classes to expose when creating new notifies.
	static void GetGameplayCueDefaultClasses(TArray<UClass*>& Classes);

	// This function tells the GameplayCue editor what classes to search for GameplayCue events.
	static void GetGameplayCueInterfaceClasses(TArray<UClass*>& Classes);

	// This function tells the GameplayCue editor where to create the GameplayCue notifies based on their tag.
	static FString GetGameplayCuePath(FString GameplayCueTag);

	static bool HasPlayWorld();

	static bool HasNoPlayWorld();

	static bool HasPlayWorldAndRunning();

	static void OpenCommonMap(const FString MapPath);

	static bool CanShowCommonMaps();

	static TSharedRef<SWidget> GetCommonMapsDropdown();

	static void CheckGameContent();

	static void ModulesChangedCallback(FName ModuleThatChanged, EModuleChangeReason ReasonForChange);

	static void OnBeginPIE(bool bIsSimulating);

	static void OnEndPIE(bool bIsSimulating);

	FDelegateHandle ToolMenusHandle;
};
