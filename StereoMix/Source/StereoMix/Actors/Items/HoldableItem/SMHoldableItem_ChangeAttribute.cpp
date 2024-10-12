// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItem_ChangeAttribute.h"

#include "Engine/OverlapResult.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/Item/SMHIC_HealItem.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMHoldableItem_ChangeAttribute::ASMHoldableItem_ChangeAttribute(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMHIC_HealItem>(HICName))
{
	bAlwaysRelevant = true;
	ColliderComponent->InitSphereRadius(65.0f);
}

void ASMHoldableItem_ChangeAttribute::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsActorEnabled);
}

void ASMHoldableItem_ChangeAttribute::ActivateItemByNoiseBreak(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& TilesToBeCaptured)
{
	Super::ActivateItemByNoiseBreak(InActivator, TilesToBeCaptured);

	bIsActivated = true;

	ActivateItem(InActivator);

	if (USMAbilitySystemComponent* ActivatorASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(InActivator))
	{
		const FGameplayEffectSpecHandle GESpec = ActivatorASC->MakeOutgoingSpec(GEForActivator, 1.0f, ActivatorASC->MakeEffectContext());
		if (GESpec.IsValid())
		{
			GESpec.Data->SetSetByCallerMagnitude(DataTagForActivator, MagnitudeForActivator);
			ActivatorASC->BP_ApplyGameplayEffectSpecToSelf(GESpec);
		}
	}

	if (Duration <= 0.0f)
	{
		return;
	}

	CachedTriggeredTiles = TilesToBeCaptured;

	TriggerCountTimerCallback();

	TWeakObjectPtr<ASMHoldableItem_ChangeAttribute> ThisWeakPtr(this);
	const float Interval = Duration / (TriggerCount - 1);
	for (int32 i = 1; i <= TriggerCount; ++i)
	{
		const bool bIsEnd = i == TriggerCount;
		auto ActivateEffect = [ThisWeakPtr, bIsEnd] {
			if (ThisWeakPtr.IsValid())
			{
				bIsEnd ? (void)ThisWeakPtr->Destroy() : ThisWeakPtr->TriggerCountTimerCallback();
			}
		};

		if (const UWorld* World = GetWorld())
		{
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, ActivateEffect, Interval * i, false);
		}
	}
}

void ASMHoldableItem_ChangeAttribute::SetIsActorEnabled(bool bNewIsActorEnabled)
{
	bIsActorEnabled = bNewIsActorEnabled;
	OnRep_bIsActorHidden();
}

void ASMHoldableItem_ChangeAttribute::TriggerCountTimerCallback()
{
	for (AActor* ConfirmedActor : GetConfirmedActorsToApplyItem())
	{
		if (USMAbilitySystemComponent* ConfirmedActorASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(ConfirmedActor))
		{
			FGameplayEffectSpecHandle GESpec = ConfirmedActorASC->MakeOutgoingSpec(GE, 1.0f, ConfirmedActorASC->MakeEffectContext());
			if (GESpec.IsValid())
			{
				GESpec.Data->SetSetByCallerMagnitude(DataTag, TotalMagnitude / TriggerCount);
				ConfirmedActorASC->BP_ApplyGameplayEffectSpecToSelf(GESpec);
			}
		}
	}

	MulticastPlayActivateTileFX(Activator, CachedTriggeredTiles);
}

TArray<AActor*> ASMHoldableItem_ChangeAttribute::GetConfirmedActorsToApplyItem()
{
	TArray<AActor*> ActorsOnTriggeredTiles = GetActorsOnTriggeredTiles(SMCollisionTraceChannel::ActionTC);

	TArray<AActor*> ConfirmedActorsToApplyItem;
	for (const auto& ActorOnTriggeredTile : ActorsOnTriggeredTiles)
	{
		const USMAbilitySystemComponent* ActorOnTriggeredTileASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(ActorOnTriggeredTile);
		if (!ActorOnTriggeredTile || !ActorOnTriggeredTileASC)
		{
			continue;
		}

		if (USMTeamBlueprintLibrary::IsSameLocalTeam(ActorOnTriggeredTile) == (TeamTypeToApply == ESMLocalTeam::Equal)) // 적용해야할 팀에 따라 필터링해줍니다.
		{
			ConfirmedActorsToApplyItem.Add(ActorOnTriggeredTile);
		}
	}

	NET_LOG(this, Log, TEXT("ConfirmedActorsToApplyItem.Num() : %d"), ConfirmedActorsToApplyItem.Num());
	return ConfirmedActorsToApplyItem;
}

TArray<AActor*> ASMHoldableItem_ChangeAttribute::GetActorsOnTriggeredTiles(ECollisionChannel TraceChannel)
{
	TArray<AActor*> Result;
	for (const auto& TriggeredTile : CachedTriggeredTiles)
	{
		if (!TriggeredTile.IsValid())
		{
			continue;
		}

		TArray<FOverlapResult> OverlapResults;
		const FVector TileLocation = TriggeredTile->GetTileLocation();
		FVector Start = TileLocation;
		const UBoxComponent* TileBoxComponent = TriggeredTile->GetBoxComponent();
		const float HalfTileHorizonSize = TileBoxComponent->GetScaledBoxExtent().X;

		FVector CollisionHalfExtend;
		if (HalfTileHorizonSize > 0.0f)
		{
			constexpr float Height = 50.0f;
			CollisionHalfExtend = FVector(HalfTileHorizonSize, HalfTileHorizonSize, Height);
			Start.Z += Height;
		}

		const FCollisionShape CollisionShape = FCollisionShape::MakeBox(CollisionHalfExtend);
		const FCollisionQueryParams Params(SCENE_QUERY_STAT(AttributeChanger), false, this);
		if (GetWorld()->OverlapMultiByChannel(OverlapResults, Start, FQuat::Identity, TraceChannel, CollisionShape, Params))
		{
			for (const auto& OverlapResult : OverlapResults)
			{
				AActor* ActorOnTriggeredTile = OverlapResult.GetActor();
				if (!ActorOnTriggeredTile)
				{
					continue;
				}

				// 트리거된 타일 위에 있는 액터를 중복되지 않게 저장합니다.
				Result.AddUnique(ActorOnTriggeredTile);
			}
		}
	}

	return Result;
}

void ASMHoldableItem_ChangeAttribute::OnRep_bIsActorHidden()
{
	SetActorHiddenInGame(!bIsActorEnabled);
	SetActorEnableCollision(bIsActorEnabled);
}

void ASMHoldableItem_ChangeAttribute::MulticastPlayActivateTileFX_Implementation(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& InTriggeredTiles)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const ESMLocalTeam LocalTeam = USMTeamBlueprintLibrary::GetLocalTeam(InActivator);

	if (ActivateVFX.Contains(LocalTeam))
	{
		for (const TWeakObjectPtr<ASMTile> TriggeredTile : InTriggeredTiles)
		{
			if (TriggeredTile.IsValid())
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ActivateVFX[LocalTeam], TriggeredTile->GetTileLocation(), FRotator::ZeroRotator, FVector(1.0), false, true, ENCPoolMethod::AutoRelease);
			}
		}
	}
}
