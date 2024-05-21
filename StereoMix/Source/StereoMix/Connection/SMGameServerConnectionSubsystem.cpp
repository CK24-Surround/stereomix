// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameServerConnectionSubsystem.h"

bool USMGameServerConnectionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if UE_SERVER
	return true;
#else
	return false;
#endif
}
