// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_NoiseBreak.h"

#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMScoreFunctionLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Indicator/SMGA_NoiseBreakIndicator.h"
#include "Utilities/SMCollision.h"


USMGA_NoiseBreak::USMGA_NoiseBreak()
{
	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Common::NoiseBreak);
	ActivationRequiredTags = FGameplayTagContainer(SMTags::Character::State::Common::Hold);

	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Uncontrollable);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Stun);
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
		if (UAbilitySystemComponent* SourceASC = GetASC())
		{
			if (const FGameplayAbilitySpec* NoiseBreakIndicatorGASpec = SourceASC->FindAbilitySpecFromClass(USMGA_NoiseBreakIndicator::StaticClass()))
			{
				SourceASC->CancelAbilityHandle(NoiseBreakIndicatorGASpec->Handle);
			}
		}
	}

	if (K2_HasAuthority())
	{
		USMScoreFunctionLibrary::RecordNoiseBreakUsage(GetCharacter());
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

void USMGA_NoiseBreak::PerformBurstAttack(const FVector& TargetLocation, const FGameplayTag& GCTag)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	const ASMTile* Tile = GetTileFromLocation(TargetLocation);
	if (!SourceCharacter || !SourceASC || !Tile)
	{
		return;
	}

	const FVector TileLocation = Tile->GetTileLocation();
	for (AActor* BurstHitActor : GetBurstHitActors(TileLocation))
	{
		ISMDamageInterface* HitActorDamageInterface = Cast<ISMDamageInterface>(BurstHitActor);
		if (!HitActorDamageInterface || HitActorDamageInterface->CanIgnoreAttack())
		{
			continue;
		}

		if (USMTeamBlueprintLibrary::IsSameTeam(SourceCharacter, BurstHitActor))
		{
			continue;
		}

		HitActorDamageInterface->ReceiveDamage(SourceCharacter, Damage);

		const FVector SplashHitActorLocation = BurstHitActor->GetActorLocation();
		const FVector NoiseBreakPointToSplashHitActorDirection = (SplashHitActorLocation - TileLocation).GetSafeNormal();

		const APawn* TargetPawn = Cast<APawn>(BurstHitActor);
		APlayerController* TargetPlayerController = TargetPawn ? TargetPawn->GetController<APlayerController>() : nullptr;
		const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
		if (TargetPlayerController && SourceDataAsset)
		{
			TargetPlayerController->ClientStartCameraShake(SourceDataAsset->SkillHitCameraShake);
		}

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Normal = NoiseBreakPointToSplashHitActorDirection;
		GCParams.TargetAttachComponent = BurstHitActor->GetRootComponent();
		SourceASC->ExecuteGC(SourceCharacter, GCTag, GCParams);
	}
}

ASMTile* USMGA_NoiseBreak::GetTileFromLocation(const FVector& Location) const
{
	const ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return nullptr;
	}

	FHitResult HitResult;
	const FVector StartLocation = Location;
	const FVector EndLocation = StartLocation + FVector::DownVector * 1000.0;
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction))
	{
		// 만약 실패하면 타일의 2배 사이즈 크기의 원으로 스윕해 다시 타일을 찾습니다. 이번에도 실패하면 nullptr을 반환합니다.
		HitResult = FHitResult();
		const FCollisionShape Sphere = FCollisionShape::MakeSphere(USMTileFunctionLibrary::DefaultTileSize);
		if (!GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, SMCollisionTraceChannel::TileAction, Sphere))
		{
			return nullptr;
		}
	}

	return Cast<ASMTile>(HitResult.GetActor());
}

TArray<AActor*> USMGA_NoiseBreak::GetBurstHitActors(const FVector& TargetLocation)
{
	TArray<AActor*> TargetActors;

	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams CollisionParams;
	CollisionParams.AddObjectTypesToQuery(ECC_Pawn); // 플레이어의 히트박스는 플레이어의 생김새보다 훨씬 크기때문에 캡슐 콜라이더만 트리거되도록합니다.
	CollisionParams.AddObjectTypesToQuery(SMCollisionObjectChannel::Destructible);

	constexpr float TileSize = USMTileFunctionLibrary::DefaultTileSize;
	const float Size = (CaptureSize * TileSize) - (TileSize / 2.0f);
	const FVector BoxHalfExtend = FVector(Size, Size, 100.0f);

	const FCollisionShape BoxCollider = FCollisionShape::MakeBox(BoxHalfExtend);
	const FCollisionQueryParams Params(TEXT("NoiseBreakSplash"), false, GetAvatarActor());

	if (!GetWorld()->OverlapMultiByObjectType(OverlapResults, TargetLocation, FQuat::Identity, CollisionParams, BoxCollider, Params))
	{
		return TargetActors;
	}

	DrawDebugBox(GetWorld(), TargetLocation, BoxHalfExtend, FColor::Red, false, 3.0f);

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (AActor* OverlapActor = OverlapResult.GetActor())
		{
			TargetActors.Add(OverlapActor);
		}
	}

	return TargetActors;
}
