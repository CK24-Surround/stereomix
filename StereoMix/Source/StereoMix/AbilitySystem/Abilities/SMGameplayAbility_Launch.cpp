// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Launch.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "AbilitySystem/SMTags.h"
#include "Utilities/SMLog.h"

USMGameplayAbility_Launch::USMGameplayAbility_Launch()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	FGameplayTagContainer BlockedTags;
	BlockedTags.AddTag(SMTags::Ability::Activation::Catch);
	BlockedTags.AddTag(SMTags::Character::State::Catch);
	BlockedTags.AddTag(SMTags::Character::State::Caught);
	BlockedTags.AddTag(SMTags::Character::State::Smashing);
	BlockedTags.AddTag(SMTags::Character::State::Smashed);
	BlockedTags.AddTag(SMTags::Character::State::Stun);
	BlockedTags.AddTag(SMTags::Character::State::Immune);

	ActivationBlockedTags = BlockedTags;
}

void USMGameplayAbility_Launch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// 투사체의 발사위치와 방향을 서버로 보냅니다.
	if (ActorInfo->IsLocallyControlled())
	{
		const FVector StartLocation = SourceCharacter->GetActorLocation();
		const FVector CursorLocation = SourceCharacter->GetCursorTargetingPoint();
		const FVector ProjectileDirection = (CursorLocation - StartLocation).GetSafeNormal2D();
		ServerRPCSendAimingData(StartLocation, ProjectileDirection);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	// 발사 애니메이션을 재생합니다.
	SourceASC->PlayMontage(this, ActivationInfo, Montage, 1.0f);
}

void USMGameplayAbility_Launch::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		const FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), 1.0f);
		if (!ensureAlways(GESpecHandle.IsValid()))
		{
			return;
		}

		FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
		if (!ensureAlways(GESpec))
		{
			return;
		}

		const float Cooldown = 1.0f / LaunchRate;
		GESpec->SetSetByCallerMagnitude(SMTags::Data::Cooldown, Cooldown);
		(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, GESpecHandle);
	}
}

void USMGameplayAbility_Launch::ServerRPCSendAimingData_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantizeNormal& Normal)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// 발사 FX를 재생합니다.
	FGameplayCueParameters GCParams;
	GCParams.Location = SourceCharacter->GetActorLocation() + SourceCharacter->GetActorForwardVector() * 100.0f;
	GCParams.Normal = FRotationMatrix(SourceCharacter->GetActorRotation()).GetUnitAxis(EAxis::X);
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileLaunch, GCParams);

	LaunchProjectile(SourceLocation, Normal);
}
