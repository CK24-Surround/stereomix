// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_LaunchOld.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "AbilitySystem/SMTags.h"
#include "Utilities/SMLog.h"

USMGameplayAbility_LaunchOld::USMGameplayAbility_LaunchOld()
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

void USMGameplayAbility_LaunchOld::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
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

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	CachedMontage = Montage.FindOrAdd(SourceTeam, nullptr);

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
	SourceASC->PlayMontage(this, ActivationInfo, CachedMontage, 1.0f);
}

void USMGameplayAbility_LaunchOld::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
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

void USMGameplayAbility_LaunchOld::ServerRPCSendAimingData_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantizeNormal& Normal)
{
	LaunchProjectile(SourceLocation, Normal);
}

void USMGameplayAbility_LaunchOld::ExecuteLaunchFX(int32 LaunchCount)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
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

	// 발사 FX를 재생합니다. RawMagnitude에 발사횟수를 넣어 초탄에만 사운드를 적용하도록 했습니다.
	FGameplayCueParameters GCParams;
	GCParams.Location = FVector::ForwardVector * 100.0f;
	GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
	GCParams.RawMagnitude = LaunchCount;
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileLaunch, GCParams);
}