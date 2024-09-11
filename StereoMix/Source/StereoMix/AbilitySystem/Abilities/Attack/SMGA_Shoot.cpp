// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Shoot.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Games/SMGameState.h"
#include "Projectiles/SMProjectile.h"
#include "Projectiles/SMProjectilePoolManagerComponent.h"

USMGA_Shoot::USMGA_Shoot()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	ActivationOwnedTags.AddTag(SMTags::Character::State::Shoot);

	ActivationBlockedTags.AddTag(SMTags::Character::State::StunBullet);
	Damage = 4.0f;
}

void USMGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	UAnimMontage* ShootMontage = SourceDataAsset->AttackMontage[SourceTeam];

	const FName MontageTaskName = TEXT("MontageTask");
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, ShootMontage, 1.0f, NAME_None, false);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		Shoot();
		GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, this, &USMGA_Shoot::Shoot, 1.0f / ShootRate, true);
	}
}

void USMGA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
	K2_EndAbility();
}

void USMGA_Shoot::ServerRPCLaunchProjectile_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation)
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

	ASMProjectile* Projectile = ProjectilePoolManager->GetProjectileForElectricGuitar(SourceCharacter->GetTeam());
	if (!Projectile)
	{
		EndAbilityByCancel();
		return;
	}

	const float MaxDistance = MaxDistanceByTile * 150.0f;
	const FVector LaunchDirection = (TargetLocation - SourceLocation).GetSafeNormal();
	Projectile->Launch(SourceCharacter, SourceLocation, LaunchDirection, ProjectileSpeed, MaxDistance, Damage);
}

void USMGA_Shoot::Shoot()
{
	if (!K2_CheckAbilityCost())
	{
		return;
	}

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return;
	}


	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	const FVector TargetLocation = SourceCharacter->GetCursorTargetingPoint();
	ServerRPCLaunchProjectile(SourceLocation, TargetLocation);
	ServerRPCApplyCost();

	const FName SectionName = TEXT("Default");
	MontageJumpToSection(SectionName);
}

void USMGA_Shoot::ServerRPCApplyCost_Implementation()
{
	K2_CommitAbilityCost();
}
