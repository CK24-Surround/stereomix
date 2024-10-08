// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItem_ChangeAttribute.h"

#include "Engine/OverlapResult.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "StereoMixLog.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Components/Item/SMHIC_HealItem.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/SMCollision.h"


ASMHoldableItem_ChangeAttribute::ASMHoldableItem_ChangeAttribute(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMHIC_HealItem>(HICName))
{
	ColliderComponent->InitSphereRadius(65.0f);

	HIC->OnHeldStateEntry.AddUObject(this, &ThisClass::OnHeldStateEntry);
}

void ASMHoldableItem_ChangeAttribute::ActivateItemByNoiseBreak(const UWorld* World, const TArray<ASMTile*>& TilesToBeCaptured, AActor* InActivator, const TOptional<ESMTeam>& TeamOption)
{
	Super::ActivateItemByNoiseBreak(World, TilesToBeCaptured, InActivator, TeamOption);

	bActivated = true;

	ActivateItem(InActivator);

	USMAbilitySystemComponent* ActivatorASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(InActivator);
	if (!ActivatorASC)
	{
		return;
	}

	FGameplayEffectSpecHandle GESpec = ActivatorASC->MakeOutgoingSpec(SelfGE, 1.0f, ActivatorASC->MakeEffectContext());
	if (GESpec.IsValid())
	{
		GESpec.Data->SetSetByCallerMagnitude(SelfDataTag, SelfMagnitude);
		ActivatorASC->BP_ApplyGameplayEffectSpecToSelf(GESpec);
	}

	if (Duration <= 0.0f)
	{
		return;
	}

	for (const auto& Tile : TilesToBeCaptured)
	{
		if (!Tile)
		{
			continue;
		}

		CachedTriggeredTiles.Add(MakeWeakObjectPtr(Tile));
	}

	TriggerCountTimerCallback();

	TWeakObjectPtr<ASMHoldableItem_ChangeAttribute> ThisWeakPtr(this);
	World->GetTimerManager().SetTimer(TriggerCountTimerHandle, [World, ThisWeakPtr] {
		if (ThisWeakPtr.IsValid())
		{
			ThisWeakPtr->CurrentTriggerCount += 1;
			if (ThisWeakPtr->CurrentTriggerCount < ThisWeakPtr->TriggerCount)
			{
				ThisWeakPtr->TriggerCountTimerCallback();
			}
			else
			{
				if (World)
				{
					World->GetTimerManager().ClearTimer(ThisWeakPtr->TriggerCountTimerHandle);
				}
				ThisWeakPtr->Destroy();
			}
		}
	}, Duration / TriggerCount, true);
}

void ASMHoldableItem_ChangeAttribute::OnHeldStateEntry()
{
	MeshComponent->SetVisibility(false);
	NiagaraComponent->SetVisibility(false);
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	if (ASMPlayerCharacterBase* HoldingMePlayer = Cast<ASMPlayerCharacterBase>(HIC->GetActorHoldingMe()))
	{
		HoldingMePlayer->GetHoldInteractionComponent()->OnHeldStateExit.AddUObject(this, &ThisClass::OnHeldStateExit);
	}
}

void ASMHoldableItem_ChangeAttribute::OnHeldStateExit()
{
	if (!bActivated)
	{
		Destroy();
	}
}

void ASMHoldableItem_ChangeAttribute::TriggerCountTimerCallback()
{
	for (AActor* ConfirmedActor : GetConfirmedActorsToApplyItem())
	{
		if (!ConfirmedActor)
		{
			continue;
		}

		USMAbilitySystemComponent* ConfirmedActorASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(ConfirmedActor);
		if (!ConfirmedActorASC)
		{
			continue;
		}

		FGameplayEffectSpecHandle GESpec = ConfirmedActorASC->MakeOutgoingSpec(GE, 1.0f, ConfirmedActorASC->MakeEffectContext());
		if (GESpec.IsValid())
		{
			GESpec.Data->SetSetByCallerMagnitude(DataTag, TotalMagnitude / TriggerCount);
			ConfirmedActorASC->BP_ApplyGameplayEffectSpecToSelf(GESpec);
		}
	}

	MulticastPlayActivateTileFX(Activator, CachedTriggeredTiles);
}

bool ASMHoldableItem_ChangeAttribute::IsSameTeamWithLocalTeam(AActor* TargetActor) const
{
	if (!TargetActor)
	{
		UE_LOG(LogStereoMix, Warning, TEXT("시전자가 유효하지 않습니다."));
		return false;
	}

	ISMTeamInterface* TargetTeamInterface = Cast<ISMTeamInterface>(TargetActor);
	if (!TargetTeamInterface)
	{
		return false;
	}

	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!LocalPlayerController)
	{
		return false;
	}

	ISMTeamInterface* LocalTeamInterface = Cast<ISMTeamInterface>(LocalPlayerController->GetPawn());
	if (!LocalTeamInterface)
	{
		return false;
	}

	ESMTeam SourceTeam = TargetTeamInterface->GetTeam();
	ESMTeam LocalTeam = LocalTeamInterface->GetTeam();

	if (SourceTeam != LocalTeam)
	{
		return false;
	}

	return true;
}

TArray<AActor*> ASMHoldableItem_ChangeAttribute::GetConfirmedActorsToApplyItem()
{
	TArray<AActor*> ActorsOnTriggeredTiles = GetActorsOnTriggeredTiles(SMCollisionTraceChannel::Action);

	TArray<AActor*> ConfirmedActorsToApplyItem;
	for (const auto& ActorOnTriggeredTile : ActorsOnTriggeredTiles)
	{
		if (!ActorOnTriggeredTile || !ActorOnTriggeredTile->IsA<ASMPlayerCharacterBase>())
		{
			continue;
		}
		
		bool CheckSameTeamWithLocalTeam = IsSameTeamWithLocalTeam(ActorOnTriggeredTile);
		if (CheckSameTeamWithLocalTeam == (ApplyTeamType == ESMLocalTeam::Equal))
		{
			ConfirmedActorsToApplyItem.Add(ActorOnTriggeredTile);
		}
	}
	UE_LOG(LogStereoMix, Warning, TEXT("ConfirmedActorsToApplyItem.Num() : %d"), ConfirmedActorsToApplyItem.Num());
	return ConfirmedActorsToApplyItem;
}

TArray<AActor*> ASMHoldableItem_ChangeAttribute::GetActorsOnTriggeredTiles(ECollisionChannel TraceChannel)
{
	TArray<AActor*> Result;
	for (const auto& TriggeredTile : CachedTriggeredTiles)
	{
		if (!ensureAlways(TriggeredTile.Get()))
		{
			continue;
		}

		TArray<FOverlapResult> OverlapResults;
		const FVector TileLocation = TriggeredTile->GetTileLocation();
		FVector Start = TileLocation;
		UBoxComponent* TileBoxComponent = TriggeredTile->GetBoxComponent();
		float HalfTileHorizonSize = TileBoxComponent->GetScaledBoxExtent().X;

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

void ASMHoldableItem_ChangeAttribute::MulticastPlayActivateTileFX_Implementation(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& InTriggeredTiles)
{
	if (HasAuthority())
	{
		return;
	}

	ESMLocalTeam LocalTeam = ESMLocalTeam::Different;
	if (IsSameTeamWithLocalTeam(InActivator))
	{
		LocalTeam = ESMLocalTeam::Equal;
	}

	if (ActivateEffect.Find(LocalTeam))
	{
		for (const auto& TriggeredTile : InTriggeredTiles)
		{
			if (!ensureAlways(TriggeredTile.Get()))
			{
				continue;
			}

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ActivateEffect[LocalTeam], TriggeredTile->GetTileLocation(), FRotator::ZeroRotator, FVector(1.0), false, true, ENCPoolMethod::AutoRelease);
		}
	}
}
