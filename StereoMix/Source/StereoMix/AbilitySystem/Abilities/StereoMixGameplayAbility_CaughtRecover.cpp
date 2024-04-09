// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_CaughtRecover.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/StereoMixeLog.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_CaughtRecover::UStereoMixGameplayAbility_CaughtRecover()
{
	AbilityTags = FGameplayTagContainer(StereoMixTag::Ability::CaughtRecover);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UStereoMixGameplayAbility_CaughtRecover::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ensure(ActorInfo))
	{
		return;
	}

	AStereoMixPlayerCharacter* StereoMixCharacter = Cast<AStereoMixPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!StereoMixCharacter)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("AStereoMixPlayerCharacter로 캐스팅에 실패했습니다."));
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("ASC가 유효하지 않습니다."));
		return;
	}

	// TODO: 이벤트로 캐릭터 정보를 넘겨받도록 수정
	// 이사이에 매치기를 통해 디태치 하게 되면 이 값이 유효하지 않게 됩니다.
	AActor* CatchCaster = StereoMixCharacter->GetAttachParentActor();
	if (!CatchCaster)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("잡기 시전 액터가 유효하지 않습니다."));
		return;
	}

	UAbilitySystemComponent* CatchCasterASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CatchCaster);
	if (!CatchCasterASC)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("잡기 시전 캐릭터의 ASC가 유효하지 않습니다."));
		return;
	}

	if (AddUncatchableStateGE)
	{
		BP_ApplyGameplayEffectToOwner(AddUncatchableStateGE);
	}

	NET_LOG(ActorInfo->AvatarActor.Get(), Log, TEXT("풀기(디태치) 시전"));
	StereoMixCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	StereoMixCharacter->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;
	
	if (RemoveCaughtStateGE)
	{
		BP_ApplyGameplayEffectToOwner(RemoveCaughtStateGE);
	}

	if (RemoveCatchStateGE)
	{
		FGameplayEffectContextHandle GEContextHandle = CatchCasterASC->MakeEffectContext();
		if (GEContextHandle.IsValid())
		{
			CatchCasterASC->BP_ApplyGameplayEffectToSelf(RemoveCatchStateGE, 1.0f, GEContextHandle);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
