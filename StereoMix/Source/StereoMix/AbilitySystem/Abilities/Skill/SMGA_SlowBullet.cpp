// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_SlowBullet.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Games/SMGameState.h"
#include "Projectiles/SMProjectile.h"
#include "Projectiles/SMProjectilePoolManagerComponent.h"
#include "Projectiles/SMSlowBulletProjectile.h"

USMGA_SlowBullet::USMGA_SlowBullet()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	ActivationOwnedTags.AddTag(SMTags::Character::State::SlowBullet);
}

void USMGA_SlowBullet::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	const FName MontageTaskName = TEXT("MontageTask");
	UAnimMontage* SlowBulletMontage = SourceDataAsset->SkillMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, SlowBulletMontage);
	PlayMontageAndWait->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnded);
	PlayMontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::OnMontageEnded);
	PlayMontageAndWait->ReadyForActivation();

	if (IsLocallyControlled())
	{
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector TargetLocation = SourceCharacter->GetLocationFromCursor();
		ServerRPCLaunchProjectile(SourceLocation, TargetLocation);
	}
}

void USMGA_SlowBullet::OnMontageEnded()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}

void USMGA_SlowBullet::ServerRPCLaunchProjectile_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation)
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

	ASMSlowBulletProjectile* Projectile = Cast<ASMSlowBulletProjectile>(ProjectilePoolManager->GetSlowBullet(SourceCharacter->GetTeam()));
	if (!Projectile)
	{
		EndAbilityByCancel();
		return;
	}

	const FVector LaunchRotation = (TargetLocation - SourceLocation).GetSafeNormal();
	const float MaxDistance = MaxDistanceByTile * 150.0f;
	Projectile->Init(SlowDebuffMultiplier, SlowDebuffDuration);
	Projectile->Launch(SourceCharacter, SourceLocation, LaunchRotation, ProjectileSpeed, MaxDistance, Damage);
}
