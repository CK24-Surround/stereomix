// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Archery.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Games/SMGameState.h"
#include "Projectiles/SMProjectile.h"
#include "Projectiles/SMProjectilePoolManagerComponent.h"

USMGA_Archery::USMGA_Archery()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	ActivationOwnedTags.AddTag(SMTags::Character::State::Archery);
	
	ActivationBlockedTags.AddTag(SMTags::Character::State::ImpactArrow);
}

void USMGA_Archery::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	const FName TaskName = TEXT("MontageTask");
	UAnimMontage* Montage = SourceDataAsset->AttackMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage, 1.0f, NAME_None, false);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		NET_LOG(GetAvatarActor(), Warning, TEXT("차지 시작"));
		UAbilityTask_WaitDelay* Charge1Task = UAbilityTask_WaitDelay::WaitDelay(this, Charge1Time);
		Charge1Task->OnFinish.AddDynamic(this, &ThisClass::OnCharged1);
		Charge1Task->ReadyForActivation();

		UAbilityTask_WaitDelay* Charge2Task = UAbilityTask_WaitDelay::WaitDelay(this, Charge2Time);
		Charge2Task->OnFinish.AddDynamic(this, &ThisClass::OnCharged2);
		Charge2Task->ReadyForActivation();
	}
}

void USMGA_Archery::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ChargeLevel = 0;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Archery::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);

	if (ChargeLevel == 0)
	{
		K2_EndAbility();
	}
	else if (ChargeLevel == 1)
	{
		Charge1();
	}
	else
	{
		Charge2();
	}
}

void USMGA_Archery::OnCharged1()
{
	ChargeLevel = 1;
	NET_LOG(GetAvatarActor(), Warning, TEXT("1단 차지"));
}

void USMGA_Archery::OnCharged2()
{
	ChargeLevel = 2;
	NET_LOG(GetAvatarActor(), Warning, TEXT("2단 차지"));
}

void USMGA_Archery::Charge1()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbilityCost();

	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	const FVector TargetLocation = SourceCharacter->GetCursorTargetingPoint();
	LaunchProjectile(SourceLocation, TargetLocation, 1);
}

void USMGA_Archery::Charge2()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbilityCost();
	K2_CommitAbilityCost();

	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	const FVector TargetLocation = SourceCharacter->GetCursorTargetingPoint();
	LaunchProjectile(SourceLocation, TargetLocation, 2);
}

void USMGA_Archery::LaunchProjectile_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation, int32 InChargeLevel)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>();
	if (!GameState)
	{
		EndAbilityByCancel();
		return;
	}

	USMProjectilePoolManagerComponent* ProjectilePoolManager = GameState->GetProjectilePoolManager();
	if (!ProjectilePoolManager)
	{
		EndAbilityByCancel();
		return;
	}

	ASMProjectile* Projectile = ProjectilePoolManager->GetProjectileForPiano(SourceCharacter->GetTeam());
	if (!Projectile)
	{
		EndAbilityByCancel();
		return;
	}

	float Damage;
	if (InChargeLevel == 1)
	{
		Damage = FullChargeDamage * Charge1DamageMultiply;
	}
	else
	{
		Damage = FullChargeDamage * Charge2DamageMultiply;
	}

	const FVector LaunchDirection = (TargetLocation - SourceLocation).GetSafeNormal();
	Projectile->Launch(SourceCharacter, SourceLocation, LaunchDirection, ProjectileSpeed, MaxDistance, Damage);

	K2_EndAbility();
}
