// Copyright Surround, Inc. All Rights Reserved.


#include "SMAnimNotify.h"

#include "AbilitySystemBlueprintLibrary.h"

FString USMAnimNotify::GetNotifyName_Implementation() const
{
	return TEXT("StereoMixAnimNotify");
}

void USMAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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
