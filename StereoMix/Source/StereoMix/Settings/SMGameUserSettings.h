// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

#include "SMGameUserSettings.generated.h"

class UFMODVCA;
class UFMODBus;

/**
 *
 */
UCLASS(ConfigDoNotCheckDefaults, MinimalAPI)
class USMGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	USMGameUserSettings(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	static USMGameUserSettings* GetStereoMixUserSettings();

	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;

	virtual void ApplyNonResolutionSettings() override;

	virtual void ResetToCurrentSettings() override;

	virtual void SetToDefaults() override;

	UFUNCTION(BlueprintPure)
	float GetMasterVolume() const { return MasterVolume; }

	UFUNCTION(BlueprintPure)
	float GetBgmVolume() const { return BgmVolume; }

	UFUNCTION(BlueprintPure)
	float GetSfxVolume() const { return SfxVolume; }

	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(float InMasterVolume, bool bUpdateImmediately);

	UFUNCTION(BlueprintCallable)
	void SetBgmVolume(float InBgmVolume, bool bUpdateImmediately);

	UFUNCTION(BlueprintCallable)
	void SetSfxVolume(float InSfxVolume, bool bUpdateImmediately);

protected:
	UPROPERTY(Config)
	float MasterVolume = 1.f;

	UPROPERTY(Config)
	float BgmVolume = 1.f;

	UPROPERTY(Config)
	float SfxVolume = 1.f;

private:
	UPROPERTY()
	TObjectPtr<UFMODVCA> FMODMasterVCA;

	UPROPERTY()
	TObjectPtr<UFMODBus> FMODBgmBus;

	UPROPERTY()
	TObjectPtr<UFMODBus> FMODSfxBus;

	UPROPERTY()
	TObjectPtr<UFMODBus> FMODUiBus;
};
