// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_NoiseBreak.h"

#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Indicator/SMGA_NoiseBreakIndicator.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"


USMGA_NoiseBreak::USMGA_NoiseBreak()
{
	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::NoiseBreak);
	ActivationRequiredTags = FGameplayTagContainer(SMTags::Character::State::Hold);

	ActivationBlockedTags.AddTag(SMTags::Character::State::Stun);
}

bool USMGA_NoiseBreak::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (IsLocallyControlled())
	{
		// 타일이 아니면 노이즈 브레이크를 할 수 없습니다.
		if (!IsValidTarget())
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
			FGameplayAbilitySpec* NoiseBreakIndicatorGASpec = SourceASC->FindAbilitySpecFromClass(USMGA_NoiseBreakIndicator::StaticClass());
			if (NoiseBreakIndicatorGASpec)
			{
				SourceASC->CancelAbilityHandle(NoiseBreakIndicatorGASpec->Handle);
			}
		}
	}
}

bool USMGA_NoiseBreak::IsValidTarget() const
{
	FVector TargetLocation;
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (!SourceCharacter)
	{
		return false;
	}

	const float MaxDistance = MaxDistanceByTile * 150.0f;
	if (!SourceCharacter->GetTileLocationFromCursor(TargetLocation, MaxDistance))
	{
		return false;
	}

	return true;
}

void USMGA_NoiseBreak::ApplySplash(const FVector& TargetLocation, const FGameplayTag& GCTag)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	ASMTile* Tile = GetTileFromLocation(TargetLocation);
	if (!SourceCharacter || !SourceASC || !SourceDataAsset || !Tile)
	{
		return;
	}

	const FVector TileLocation = Tile->GetTileLocation();
	TArray<AActor*> SplashHitActors = GetSplashHitActors(TileLocation);
	for (AActor* SplashHitActor : SplashHitActors)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SplashHitActor);
		if (!TargetASC)
		{
			continue;
		}

		FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(SourceDataAsset->DamageGE);
		if (GESpecHandle.IsValid())
		{
			GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, Damage);
			TargetASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
		}

		const FVector SplashHitActorLocation = SplashHitActor->GetActorLocation();
		const FVector NoiseBreakPointToSplashHitActorDirection = (SplashHitActorLocation - TileLocation).GetSafeNormal();
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Normal = NoiseBreakPointToSplashHitActorDirection;
		GCParams.TargetAttachComponent = SplashHitActor->GetRootComponent();
		SourceASC->ExecuteGC(SourceCharacter, GCTag, GCParams);
	}
}

ASMTile* USMGA_NoiseBreak::GetTileFromLocation(const FVector& Location)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return nullptr;
	}

	FHitResult HitResult;
	const FVector StartLocation = Location;
	const FVector EndLocation = StartLocation + FVector::DownVector * 1000.0;
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction))
	{
		return nullptr;
	}

	return Cast<ASMTile>(HitResult.GetActor());
}

TArray<AActor*> USMGA_NoiseBreak::GetSplashHitActors(const FVector& TargetLocation)
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionObjectQueryParams CollisionParams;
	CollisionParams.AddObjectTypesToQuery(ECC_Pawn);
	const float Size = (CaptureSize * 150.0f) - 75.0f;
	const FVector BoxHalfExtend = FVector(Size, Size, 100.0f);
	const FCollisionShape BoxCollider = FCollisionShape::MakeBox(BoxHalfExtend);
	const FCollisionQueryParams Params(TEXT("NoiseBreakSplash"), false, GetCharacter());
	if (!GetWorld()->OverlapMultiByObjectType(OverlapResults, TargetLocation, FQuat::Identity, CollisionParams, BoxCollider, Params))
	{
		return TArray<AActor*>();
	}

	DrawDebugBox(GetWorld(), TargetLocation, BoxHalfExtend, FColor::Red, false, 3.0f);

	TArray<AActor*> TargetActors;
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AActor* OverlapActor = OverlapResult.GetActor();
		if (CanApplySplashDamage(OverlapActor))
		{
			TargetActors.Add(OverlapActor);
		}
	}

	return TargetActors;
}

bool USMGA_NoiseBreak::CanApplySplashDamage(AActor* TargetActor)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (!SourceCharacter)
	{
		return false;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	const ESMTeam TargetTeam = USMTeamBlueprintLibrary::GetTeam(TargetActor);
	if (SourceTeam == TargetTeam)
	{
		return false;
	}

	return true;
}
