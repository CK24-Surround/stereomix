// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_NoiseBreak.h"

#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "SMGA_NoiseBreakIndicator.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"


USMGA_NoiseBreak::USMGA_NoiseBreak()
{
	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::NoiseBreaking);
	ActivationRequiredTags = FGameplayTagContainer(SMTags::Character::State::Hold);

	ActivationBlockedTags.AddTag(SMTags::Character::State::Stun);
}

bool USMGA_NoiseBreak::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return false;
	}

	FVector TargetLocation;
	if (IsLocallyControlled())
	{
		const float MaxDistance = MaxDistanceByTile * 150.0f;
		if (!SourceCharacter->GetTileLocationFromCursor(TargetLocation, MaxDistance))
		{
			return false;
		}
	}

	return true;
}

void USMGA_NoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (IsLocallyControlled())
	{
		UAbilitySystemComponent* SourceASC = GetASC();
		if (SourceASC)
		{
			FGameplayAbilitySpec* NoiseBreakIndicatorGA = SourceASC->FindAbilitySpecFromClass(USMGA_NoiseBreakIndicator::StaticClass());
			if (NoiseBreakIndicatorGA)
			{
				SourceASC->CancelAbilityHandle(NoiseBreakIndicatorGA->Handle);
			}
		}
	}
}

void USMGA_NoiseBreak::ApplySplash(const FVector& TargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceDataAsset)
	{
		return;
	}

	FHitResult HitResult;
	const FVector End = TargetLocation + (FVector::DownVector * 1000.0f);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, TargetLocation, End, SMCollisionTraceChannel::TileAction))
	{
		return;
	}

	ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
	if (!Tile)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	const FVector TileLocation = Tile->GetTileLocation();
	FCollisionObjectQueryParams CollisionParams;
	CollisionParams.AddObjectTypesToQuery(ECC_Pawn);
	const float Size = (CaptureSize * 150.0f) - 75.0f;
	const FVector BoxHalfExtend = FVector(Size, Size, 100.0f);
	const FCollisionShape BoxCollider = FCollisionShape::MakeBox(BoxHalfExtend);
	const FCollisionQueryParams Params(TEXT("NoiseBreak"), false, SourceCharacter);
	if (!GetWorld()->OverlapMultiByObjectType(OverlapResults, TileLocation, FQuat::Identity, CollisionParams, BoxCollider))
	{
		return;
	}

	DrawDebugBox(GetWorld(), TileLocation, BoxHalfExtend, FColor::Red, false, 3.0f);

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	for (const auto& OverlapResult : OverlapResults)
	{
		AActor* TargetActor = OverlapResult.GetActor();
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!TargetASC)
		{
			continue;
		}

		const ESMTeam TargetTEam = USMTeamBlueprintLibrary::GetTeam(TargetActor);
		if (SourceTeam == TargetTEam)
		{
			continue;
		}

		FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(SourceDataAsset->DamageGE);
		if (GESpecHandle.IsValid())
		{
			GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::Data::Damage, Damage);
			TargetASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
		}
	}
}
