// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixAnimNotify.h"

#include "AbilitySystemBlueprintLibrary.h"

FString UStereoMixAnimNotify::GetNotifyName_Implementation() const
{
	return TEXT("StereoMixAnimNotify");
}

void UStereoMixAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (OwnerActor)
	{
		const UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
		if (!ASC || !TriggerEventTag.IsValid())
		{
			return;
		}

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerEventTag, FGameplayEventData());
	}
}
