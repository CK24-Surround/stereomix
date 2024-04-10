// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Smashed.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/StereoMixLog.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Smashed::UStereoMixGameplayAbility_Smashed()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags = FGameplayTagContainer(StereoMixTag::Ability::Smashed);
}

void UStereoMixGameplayAbility_Smashed::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* CachedAvatarActor = GetAvatarActorFromActorInfo();
	if (!CachedAvatarActor)
	{
		NET_LOG(nullptr, Warning, TEXT("아바타 액터가 유효하지 않습니다."));
	}
	
	AStereoMixPlayerCharacter* CachedAvatarCharacter = Cast<AStereoMixPlayerCharacter>(CachedAvatarActor);
	if (!CachedAvatarCharacter)
	{
		NET_LOG(CachedAvatarActor, Warning, TEXT("캐스팅에 실패했습니다."))
		return;
	}

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CachedAvatarCharacter);
	if (!SourceASC)
	{
		return;
	}

	AStereoMixPlayerCharacter* CatchCasterCharacter = Cast<AStereoMixPlayerCharacter>(TriggerEventData->Instigator);
	if (!CatchCasterCharacter)
	{
		return;
	}

	UAbilitySystemComponent* CatchCasterASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CatchCasterCharacter);
	if (!CatchCasterASC)
	{
		return;
	}

	NET_LOG(ActorInfo->AvatarActor.Get(), Log, TEXT("풀기(디태치) 시전"));
	CachedAvatarCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CachedAvatarCharacter->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;
	
	if (RemoveCaughtStateGE)
	{
		BP_ApplyGameplayEffectToOwner(RemoveCaughtStateGE);
	}

	if (RemoveCatchStateGE)
	{
		const FGameplayEffectContextHandle GEContextHandle = CatchCasterASC->MakeEffectContext();
		if (GEContextHandle.IsValid())
		{
			CatchCasterASC->BP_ApplyGameplayEffectToSelf(RemoveCatchStateGE, 1.0f, GEContextHandle);
		}
	}

	SourceASC->PlayMontage(this, ActivationInfo, SmashedMontage, 1.0f);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
