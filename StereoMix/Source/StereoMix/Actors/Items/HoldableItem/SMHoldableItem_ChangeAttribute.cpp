// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItem_ChangeAttribute.h"

#include "NiagaraComponent.h"
#include "StereoMixLog.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/SphereComponent.h"
#include "Components/Item/SMHIC_HealItem.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
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

	USMAbilitySystemComponent* ActivatorASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(InActivator);
	if (!ActivatorASC)
	{
		return;
	}

	bActivated = true;

	ActivateItem(InActivator);

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

	CachedTiles = TilesToBeCaptured;
	UE_LOG(LogStereoMix, Warning, TEXT("CachedTiles.Num(): %d"), CachedTiles.Num());

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

void ASMHoldableItem_ChangeAttribute::BeginDestroy()
{
	UE_LOG(LogStereoMix, Warning, TEXT("Destroy called"));
	Super::BeginDestroy();
}

void ASMHoldableItem_ChangeAttribute::TriggerCountTimerCallback()
{
	UE_LOG(LogStereoMix, Warning, TEXT("TriggerCountTimerCallback"));
}
