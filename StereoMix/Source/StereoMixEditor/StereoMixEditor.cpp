#include "StereoMixEditor.h"

#include "AbilitySystemGlobals.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameplayAbilitiesEditorModule.h"
#include "GameplayAbilitiesModule.h"
#include "GameplayCueInterface.h"
#include "GameplayCueNotify_Burst.h"
#include "GameplayCueNotify_BurstLatent.h"
#include "GameplayCueNotify_Looping.h"
#include "UnrealEdGlobals.h"
#include "Development/SMDeveloperSettings.h"
#include "Editor/UnrealEdEngine.h"
#include "Misc/DataValidation.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "UObject/UObjectIterator.h"
#include "GameEditorStyle.h"
#include "Validation/EditorValidator.h"

class SWidget;

#define LOCTEXT_NAMESPACE "StereoMixEditor"

DEFINE_LOG_CATEGORY(LogStereoMixEditor)


void FStereoMixEditorModule::StartupModule()
{
	FGameEditorStyle::Initialize();

	if (!IsRunningGame())
	{
		FModuleManager::Get().OnModulesChanged().AddStatic(&ThisClass::ModulesChangedCallback);

		BindGameplayAbilitiesEditorDelegates();

		if (FSlateApplication::IsInitialized())
		{
			ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&ThisClass::RegisterGameEditorMenus));
		}

		FEditorDelegates::BeginPIE.AddStatic(&ThisClass::OnBeginPIE);
		FEditorDelegates::EndPIE.AddStatic(&ThisClass::OnEndPIE);
	}
}

void FStereoMixEditorModule::ShutdownModule()
{
	FEditorDelegates::BeginPIE.RemoveAll(this);
	FEditorDelegates::EndPIE.RemoveAll(this);

	// Undo UToolMenus
	if (UObjectInitialized() && ToolMenusHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
	}

	UnbindGameplayAbilitiesEditorDelegates();
	FModuleManager::Get().OnModulesChanged().RemoveAll(this);
	FGameEditorStyle::Shutdown();
}

void FStereoMixEditorModule::BindGameplayAbilitiesEditorDelegates()
{
	IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();

	GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().BindStatic(&ThisClass::GetGameplayCueDefaultClasses);
	GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().BindStatic(&ThisClass::GetGameplayCueInterfaceClasses);
	GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().BindStatic(&ThisClass::GetGameplayCuePath);
}

void FStereoMixEditorModule::UnbindGameplayAbilitiesEditorDelegates()
{
	if (IGameplayAbilitiesEditorModule::IsAvailable())
	{
		IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();
		GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().Unbind();
		GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().Unbind();
		GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().Unbind();
	}
}

void FStereoMixEditorModule::ModulesChangedCallback(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	if ((ReasonForChange == EModuleChangeReason::ModuleLoaded) && (ModuleThatChanged.ToString() == TEXT("GameplayAbilitiesEditor")))
	{
		BindGameplayAbilitiesEditorDelegates();
	}
}

void FStereoMixEditorModule::OnBeginPIE(bool bIsSimulating)
{
}

void FStereoMixEditorModule::OnEndPIE(bool bIsSimulating)
{
}

void FStereoMixEditorModule::GetGameplayCueDefaultClasses(TArray<UClass*>& Classes)
{
	Classes.Empty();
	Classes.Add(UGameplayCueNotify_Burst::StaticClass());
	Classes.Add(AGameplayCueNotify_BurstLatent::StaticClass());
	Classes.Add(AGameplayCueNotify_Looping::StaticClass());
}

void FStereoMixEditorModule::GetGameplayCueInterfaceClasses(TArray<UClass*>& Classes)
{
	Classes.Empty();

	for (UClass* Class : TObjectRange<UClass>())
	{
		if (Class->IsChildOf<AActor>() && Class->ImplementsInterface(UGameplayCueInterface::StaticClass()))
		{
			Classes.Add(Class);
		}
	}
}

FString FStereoMixEditorModule::GetGameplayCuePath(FString GameplayCueTag)
{
	FString Path = FString(TEXT("/Game"));

	//@TODO: Try plugins (e.g., GameplayCue.ShooterGame.Foo should be in ShooterCore or something)

	// Default path to the first entry in the UAbilitySystemGlobals::GameplayCueNotifyPaths.
	if (IGameplayAbilitiesModule::IsAvailable())
	{
		IGameplayAbilitiesModule& GameplayAbilitiesModule = IGameplayAbilitiesModule::Get();

		if (GameplayAbilitiesModule.IsAbilitySystemGlobalsAvailable())
		{
			UAbilitySystemGlobals* AbilitySystemGlobals = GameplayAbilitiesModule.GetAbilitySystemGlobals();
			check(AbilitySystemGlobals);

			TArray<FString> GetGameplayCueNotifyPaths = AbilitySystemGlobals->GetGameplayCueNotifyPaths();

			if (GetGameplayCueNotifyPaths.Num() > 0)
			{
				Path = GetGameplayCueNotifyPaths[0];
			}
		}
	}

	GameplayCueTag.RemoveFromStart(TEXT("GameplayCue."));

	FString NewDefaultPathName = FString::Printf(TEXT("%s/GCN_%s"), *Path, *GameplayCueTag);

	return NewDefaultPathName;
}

bool FStereoMixEditorModule::HasPlayWorld()
{
	return GEditor->PlayWorld != nullptr;
}

bool FStereoMixEditorModule::HasNoPlayWorld()
{
	return !HasPlayWorld();
}

bool FStereoMixEditorModule::HasPlayWorldAndRunning()
{
	return HasPlayWorld() && !GUnrealEd->PlayWorld->bDebugPauseExecution;
}

void FStereoMixEditorModule::OpenCommonMap(const FString MapPath)
{
	if (ensure(MapPath.Len()))
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(MapPath);
	}
}

bool FStereoMixEditorModule::CanShowCommonMaps()
{
	return HasNoPlayWorld() && !GetDefault<USMDeveloperSettings>()->CommonEditorMaps.IsEmpty();
}

TSharedRef<SWidget> FStereoMixEditorModule::GetCommonMapsDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	for (const FSoftObjectPath& Path : GetDefault<USMDeveloperSettings>()->CommonEditorMaps)
	{
		if (!Path.IsValid())
		{
			continue;
		}

		const FText DisplayName = FText::FromString(Path.GetAssetName());
		MenuBuilder.AddMenuEntry(
			DisplayName,
			LOCTEXT("CommonPathDescription", "Opens this map in the editor"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateStatic(&ThisClass::OpenCommonMap, Path.ToString()),
				FCanExecuteAction::CreateStatic(&ThisClass::HasNoPlayWorld),
				FIsActionChecked(),
				FIsActionButtonVisible::CreateStatic(&ThisClass::HasNoPlayWorld)
				)
			);
	}

	return MenuBuilder.MakeWidget();
}

void FStereoMixEditorModule::CheckGameContent()
{
	UEditorValidator::ValidateCheckedOutContent(/*bInteractive=*/true, EDataValidationUsecase::Manual);
}

void FStereoMixEditorModule::RegisterGameEditorMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = Menu->AddSection("PlayGameExtensions", TAttribute<FText>(), FToolMenuInsert("Play", EToolMenuInsertType::After));

	FToolMenuEntry CheckContentEntry = FToolMenuEntry::InitToolBarButton(
		"CheckContent",
		FUIAction(
			FExecuteAction::CreateStatic(&ThisClass::CheckGameContent),
			FCanExecuteAction::CreateStatic(&ThisClass::HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)),
		LOCTEXT("CheckContentButton", "Check Content"),
		LOCTEXT("CheckContentDescription", "Runs the Content Validation job on all checked out assets to look for warnings and errors"),
		FSlateIcon(FGameEditorStyle::GetStyleSetName(), "GameEditor.CheckContent"));
	CheckContentEntry.StyleNameOverride = "CalloutToolbar";
	Section.AddEntry(CheckContentEntry);

	FToolMenuEntry CommonMapEntry = FToolMenuEntry::InitComboButton(
		"CommonMapOptions",
		FUIAction(
			FExecuteAction(),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&CanShowCommonMaps)),
		FOnGetContent::CreateStatic(&GetCommonMapsDropdown),
		LOCTEXT("CommonMaps_Label", "Common Maps"),
		LOCTEXT("CommonMaps_ToolTip", "Some commonly desired maps while using the editor"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Level"));
	CommonMapEntry.StyleNameOverride = "CalloutToolbar";
	Section.AddEntry(CommonMapEntry);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStereoMixEditorModule, StereoMixEditor)
