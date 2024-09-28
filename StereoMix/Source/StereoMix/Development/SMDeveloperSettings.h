// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "SMDeveloperSettings.generated.h"

/**
 * StereoMix developer settings
 */
UCLASS(DisplayName = "StereoMix Developer Settings", Config = EditorPerProjectUserSettings, MinimalAPI)
class USMDeveloperSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override;
	
	// GameplayMessageSubsystem을 통해 브로드캐스트 되는 메시지를 로깅할 지 여부를 나타냅니다.
	UPROPERTY(Config, EditAnywhere, DisplayName = "Log Gameplay Messages", Category=GameplayMessages, meta=(ConsoleVariable="GameplayMessageSubsystem.LogMessages"))
	bool bLogGameplayMessages = false;

#if WITH_EDITOR

public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostReloadConfig(FProperty* PropertyThatWasLoaded) override;

	virtual void PostInitProperties() override;

	/** PIE가 실행될 때 호출됩니다. */
	STEREOMIX_API void OnPlayInEditorStarted() const;

#if WITH_EDITORONLY_DATA
	// 에디터 개발 도구 모음을 통해 접근 가능한 레벨 목록입니다.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category=Maps, meta=(AllowedClasses="/Script/Engine.World"))
	TArray<FSoftObjectPath> CommonEditorMaps;
#endif // WITH_EDITORONLY_DATA

private:
	void ApplySettings();
#endif // WITH_EDITOR
};
