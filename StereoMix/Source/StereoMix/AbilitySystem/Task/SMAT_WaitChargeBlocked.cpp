// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_WaitChargeBlocked.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/Player/SMBassCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMLog.h"

USMAT_WaitChargeBlocked* USMAT_WaitChargeBlocked::WaitChargeBlocked(UGameplayAbility* OwningAbility, const FGameplayTagContainer& IgnoreTags)
{
	USMAT_WaitChargeBlocked* MyObj = NewAbilityTask<USMAT_WaitChargeBlocked>(OwningAbility);
	MyObj->SourceCharacter = Cast<ASMBassCharacter>(OwningAbility->GetAvatarActorFromActorInfo());
	MyObj->ChargeIgnoreTags = IgnoreTags;
	return MyObj;
}

void USMAT_WaitChargeBlocked::Activate()
{
	if (SourceCharacter.Get())
	{
		UCapsuleComponent* SourceCapsuleComponent = SourceCharacter->GetCapsuleComponent();
		if (!SourceCapsuleComponent)
		{
			return;
		}

		UBoxComponent* SourceChargeCollider = SourceCharacter->GetChargeColliderComponent();
		if (!SourceChargeCollider)
		{
			return;
		}

		SourceChargeCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnChargeOverlappedCallback);
		SourceCapsuleComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnChargeBlockedCallback);
	}
}

void USMAT_WaitChargeBlocked::OnDestroy(bool bInOwnerFinished)
{
	OnChargeBlocked.Unbind();

	Super::OnDestroy(bInOwnerFinished);
}

void USMAT_WaitChargeBlocked::OnChargeOverlappedCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!SourceCharacter.Get())
	{
		return;
	}

	if (!OtherActor)
	{
		return;
	}

	ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(SourceCharacter.Get());
	ESMTeam TargetTeam = USMTeamBlueprintLibrary::GetTeam(OtherActor);

	if (SourceTeam == TargetTeam)
	{
		return;
	}


	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		if (TargetASC->HasAnyMatchingGameplayTags(ChargeIgnoreTags))
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
