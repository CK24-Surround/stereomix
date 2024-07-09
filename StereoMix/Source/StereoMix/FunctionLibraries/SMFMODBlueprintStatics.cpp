// Copyright Surround, Inc. All Rights Reserved.


#include "SMFMODBlueprintStatics.h"

#include "FMODBus.h"
#include "FMODVCA.h"

float USMFMODBlueprintStatics::BusGetVolume(UFMODBus* Bus)
{
	FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (StudioSystem != nullptr && IsValid(Bus))
	{
		FMOD::Studio::ID Guid = FMODUtils::ConvertGuid(Bus->AssetGuid);
		FMOD::Studio::Bus* BusPtr = nullptr;
		FMOD_RESULT Result = StudioSystem->getBusByID(&Guid, &BusPtr);
		if (Result == FMOD_OK && BusPtr != nullptr)
		{
			float Volume;
			BusPtr->getVolume(&Volume);
			return Volume;
		}
	}

	return 0.0f;
}

float USMFMODBlueprintStatics::VCAGetVolume(UFMODVCA* VCA)
{
	FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (StudioSystem != nullptr && IsValid(VCA))
	{
		FMOD::Studio::ID Guid = FMODUtils::ConvertGuid(VCA->AssetGuid);
		FMOD::Studio::VCA* VCAPtr = nullptr;
		FMOD_RESULT Result = StudioSystem->getVCAByID(&Guid, &VCAPtr);
		if (Result == FMOD_OK && VCAPtr != nullptr)
		{
			float Volume;
			VCAPtr->getVolume(&Volume);
			return Volume;
		}
	}

	return 0.0f;
}
