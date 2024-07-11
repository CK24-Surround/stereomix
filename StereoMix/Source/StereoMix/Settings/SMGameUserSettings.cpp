// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameUserSettings.h"

#include "FMODBlueprintStatics.h"
#include "FMODBus.h"
#include "FMODVCA.h"

USMGameUserSettings* USMGameUserSettings::GetStereoMixUserSettings()
{
	return Cast<USMGameUserSettings>(GetGameUserSettings());
}

USMGameUserSettings::USMGameUserSettings()
{
	static ConstructorHelpers::FObjectFinder<UFMODAsset> MasterVcaAsset(TEXT("/Script/FMODStudio.FMODVCA'/Game/FMOD/VCAs/Master_VCA.Master_VCA'"));
	static ConstructorHelpers::FObjectFinder<UFMODAsset> BgmBusAsset(TEXT("/Script/FMODStudio.FMODBus'/Game/FMOD/Buses/BGM.BGM'"));
	static ConstructorHelpers::FObjectFinder<UFMODAsset> SfxBusAsset(TEXT("/Script/FMODStudio.FMODBus'/Game/FMOD/Buses/SFX.SFX'"));
	static ConstructorHelpers::FObjectFinder<UFMODAsset> UiBusAsset(TEXT("/Script/FMODStudio.FMODBus'/Game/FMOD/Buses/UI.UI'"));

	FMODMasterVCA = CastChecked<UFMODVCA>(MasterVcaAsset.Object);
	FMODBgmBus = CastChecked<UFMODBus>(BgmBusAsset.Object);
	FMODSfxBus = CastChecked<UFMODBus>(SfxBusAsset.Object);
	FMODUiBus = CastChecked<UFMODBus>(UiBusAsset.Object);
}

void USMGameUserSettings::ApplySettings(const bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
}

void USMGameUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	UFMODBlueprintStatics::VCASetVolume(FMODMasterVCA, MasterVolume);
	UFMODBlueprintStatics::BusSetVolume(FMODBgmBus, BgmVolume);
	UFMODBlueprintStatics::BusSetVolume(FMODSfxBus, SfxVolume);
	UFMODBlueprintStatics::BusSetVolume(FMODUiBus, SfxVolume);
}

void USMGameUserSettings::ResetToCurrentSettings()
{
	Super::ResetToCurrentSettings();
}

void USMGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
	SetMasterVolume(1.f, false);
	SetSfxVolume(1.f, false);
	SetBgmVolume(1.f, false);
}

void USMGameUserSettings::SetMasterVolume(const float InMasterVolume, const bool bUpdateImmediately)
{
	MasterVolume = FMath::Clamp(InMasterVolume, 0.f, 1.f);
	if (bUpdateImmediately)
	{
		UFMODBlueprintStatics::VCASetVolume(FMODMasterVCA, MasterVolume);
	}
}

void USMGameUserSettings::SetBgmVolume(const float InBgmVolume, const bool bUpdateImmediately)
{
	BgmVolume = FMath::Clamp(InBgmVolume, 0.f, 1.f);
	if (bUpdateImmediately)
	{
		UFMODBlueprintStatics::BusSetVolume(FMODBgmBus, BgmVolume);
	}
}

void USMGameUserSettings::SetSfxVolume(const float InSfxVolume, const bool bUpdateImmediately)
{
	SfxVolume = FMath::Clamp(InSfxVolume, 0.f, 1.f);
	if (bUpdateImmediately)
	{
		UFMODBlueprintStatics::BusSetVolume(FMODSfxBus, SfxVolume);
		UFMODBlueprintStatics::BusSetVolume(FMODUiBus, SfxVolume);
	}
}
