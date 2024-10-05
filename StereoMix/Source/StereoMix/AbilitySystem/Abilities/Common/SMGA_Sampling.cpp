// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Sampling.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_CheckUnderTile.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Tiles/SMTile.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"

USMGA_Sampling::USMGA_Sampling()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	DeactivateTags.AddTag(SMTags::Character::State::Common::Neutralized);
	DeactivateTags.AddTag(SMTags::Character::State::Common::Immune);
	DeactivateTags.AddTag(SMTags::Character::State::Common::NoiseBreak);

	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Stun);
}

void USMGA_Sampling::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAT_CheckUnderTile* CheckUnderTileTask = USMAT_CheckUnderTile::CheckUnderTile(this, SourceCharacter, TickRate);
	CheckUnderTileTask->OnUnderTileChanged.BindUObject(this, &ThisClass::OnUnderTileChanged);
	CheckUnderTileTask->ReadyForActivation();
}

void USMGA_Sampling::OnUnderTileChanged(ASMTile* UnderTile)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	if (!UnderTile || !SourceCharacter || !SourceASC)
	{
		return;
	}

	// 샘플링 비활성화 태그를 갖고 있다면 타일을 점령하지 않습니다.
	if (SourceASC->HasAnyMatchingGameplayTags(DeactivateTags))
	{
		return;
	}

	if (USMTeamBlueprintLibrary::IsSameTeam(SourceCharacter, UnderTile))
	{
		return;
	}

	USMTileFunctionLibrary::CaptureTiles(GetWorld(), TArray<ASMTile*>{UnderTile}, SourceCharacter);
}
