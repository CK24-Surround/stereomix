// Copyright Studio Surround. All Rights Reserved.


#include "SMOverlapItemBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Components/SphereComponent.h"
#include "UI/SMTags.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMOverlapItemBase::ASMOverlapItemBase()
{
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	UnavailableTags.AddTag(SMTags::Character::State::Immune);
	UnavailableTags.AddTag(SMTags::Character::State::Neutralize);
}

void ASMOverlapItemBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if (HasAuthority() && World)
	{
		TWeakObjectPtr<ASMOverlapItemBase> ThisWeakPtr(this);
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr] {
			if (ThisWeakPtr.IsValid())
			{
				ThisWeakPtr->ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::OverlapItem);
			}
		}, ActivateDelay, false);
	}
}

void ASMOverlapItemBase::ActivateItem(AActor* InActivator)
{
	Super::ActivateItem(InActivator);
}

void ASMOverlapItemBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (!SourceASC || SourceASC->HasAnyMatchingGameplayTags(UnavailableTags))
	{
		return;
	}
	
	NET_LOG(this, Warning, TEXT("%s가 아이템을 획득 했습니다."), *GetNameSafe(OtherActor));
	ActivateItem(OtherActor);

	Destroy();
}
