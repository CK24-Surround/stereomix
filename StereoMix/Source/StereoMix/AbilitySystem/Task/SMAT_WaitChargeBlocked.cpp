// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_WaitChargeBlocked.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMBassCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMLog.h"

USMAT_WaitChargeBlocked* USMAT_WaitChargeBlocked::WaitChargeBlocked(UGameplayAbility* OwningAbility)
{
	USMAT_WaitChargeBlocked* MyObj = NewAbilityTask<USMAT_WaitChargeBlocked>(OwningAbility);
	MyObj->SourceCharacter = Cast<ASMBassCharacter>(OwningAbility->GetAvatarActorFromActorInfo());
	return MyObj;
}

void USMAT_WaitChargeBlocked::Activate()
{
	UBoxComponent* ChargeCollider = SourceCharacter.Get() ? SourceCharacter->GetChargeColliderComponent() : nullptr;
	UCapsuleComponent* CapsuleComponent = SourceCharacter.Get() ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!ChargeCollider || !CapsuleComponent)
	{
		return;
	}

	ChargeCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnChargeOverlappedCallback);
	CapsuleComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnChargeBlockedCallback);
}

void USMAT_WaitChargeBlocked::OnDestroy(bool bInOwnerFinished)
{
	OnChargeBlocked.Unbind();

	Super::OnDestroy(bInOwnerFinished);
}

void USMAT_WaitChargeBlocked::OnChargeOverlappedCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!SourceCharacter.Get() || !OtherActor)
	{
		return;
	}

	if (USMTeamBlueprintLibrary::IsSameTeam(SourceCharacter.Get(), OtherActor))
	{
		return;
	}

	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(OtherActor);
	if (!TargetDamageInterface || TargetDamageInterface->CanIgnoreAttack())
	{
		return;
	}

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		if (TargetASC->HasMatchingGameplayTag(SMTags::Character::State::Bass::Charge)) // 만약 상대도 돌진중이라면 그대로 지나갑니다.
		{
			return;
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		(void)OnChargeBlocked.ExecuteIfBound(OtherActor);
	}

	EndTask();
}

void USMAT_WaitChargeBlocked::OnChargeBlockedCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		(void)OnChargeBlocked.ExecuteIfBound(OtherActor);
	}

	EndTask();
}
