// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerState.h"

#include "Net/UnrealNetwork.h"

void ASMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team);
	DOREPLIFETIME(ThisClass, CharacterType);
}
