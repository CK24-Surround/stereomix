// Copyright Studio Surround. All Rights Reserved.


#include "SMPlayerStartBase.h"

void ASMPlayerStartBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlayerStartTag = FName(GetPlayerStartTagByTypeAndTeam(CharacterType, Team));
	UE_LOG(LogTemp, Warning, TEXT("InitCompTag: %d %d"), CharacterType, Team);
}

FString ASMPlayerStartBase::GetPlayerStartTagByTypeAndTeam(ESMCharacterType InType, ESMTeam InTeam)
{
	return FString::Printf(TEXT("PlayerStater_%s%s"), *GetStringByType(InType), *GetStringByTeam(InTeam));
}

FString ASMPlayerStartBase::GetStringByType(ESMCharacterType InType)
{
	FString TypeString = UEnum::GetValueAsString(InType);
	TypeString.RemoveFromStart(TEXT("ESMCharacterType::"));
	return TypeString;
}

FString ASMPlayerStartBase::GetStringByTeam(ESMTeam InTeam)
{
	FString TeamString = UEnum::GetValueAsString(InTeam);
	TeamString.RemoveFromStart(TEXT("ESMTeam::"));
	return TeamString;
}
