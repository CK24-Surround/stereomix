// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotifyBase.h"


USMGameplayCueNotifyBase::USMGameplayCueNotifyBase()
{
	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		ESMTeam Key = static_cast<ESMTeam>(i);
		VFX.Add(Key, nullptr);
		SFX.Add(Key, nullptr);
	}
}