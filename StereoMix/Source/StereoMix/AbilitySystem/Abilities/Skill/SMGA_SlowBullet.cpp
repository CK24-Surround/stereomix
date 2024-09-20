// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_SlowBullet.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "Games/SMGameState.h"
#include "Projectiles/Effect/Skill/SMEP_SlowBullet.h"
#include "Projectiles/Pool/SMProjectilePoolManagerComponent.h"

USMGA_SlowBullet::USMGA_SlowBullet()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	ActivationOwnedTags.AddTag(SMTags::Character::State::SlowBullet);

	if (FSMCharacterSkillData* SkillData = GetSkillData(ESMCharacterType::ElectricGuitar))
	{
		Damage = SkillData->Damage;
		MaxDistanceByTile = SkillData->DistanceByTile;
		ProjectileSpeed = SkillData->ProjectileSpeed;
		SlowDebuffMultiplier = SkillData->Magnitude;
		SlowDebuffDuration = SkillData->Duration;
	}
}

void USMGA_SlowBullet::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	const FName MontageTaskName = TEXT("MontageTask");
	UAnimMontage* SlowBulletMontage = SourceDataAsset->SkillMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, SlowBulletMontage);
	PlayMontageAndWait->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEnded);
	PlayMontageAndWait->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnded);
	PlayMontageAndWait->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageEnded);
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
	ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>();
	USMProjectilePoolManagerComponent* ProjectilePoolManager = GameState ? GameState->GetProjectilePoolManager() : nullptr;
	if (!SourceCharacter || !ProjectilePoolManager)
	{
		EndAbilityByCancel();
		return;
	}

	ASMProjectile* Projectile = ProjectilePoolManager->GetSlowBullet(SourceCharacter->GetTeam());
	if (!Projectile)
	{
		EndAbilityByCancel();
		return;
	}

	const FVector LaunchDirection = (TargetLocation - SourceLocation).GetSafeNormal();
	const float MaxDistance = MaxDistanceByTile * 150.0f;

	FSMProjectileParameters ProjectileParams;
	ProjectileParams.Owner = SourceCharacter;
	ProjectileParams.StartLocation = SourceLocation;
	ProjectileParams.Normal = LaunchDirection;
	ProjectileParams.Damage = Damage;
	ProjectileParams.Speed = ProjectileSpeed;
	ProjectileParams.MaxDistance = MaxDistance;
	ProjectileParams.Magnitude = SlowDebuffMultiplier;
	ProjectileParams.Duration = SlowDebuffDuration;
	Projectile->Launch(ProjectileParams);

	if (USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>())
	{
		FGameplayCueParameters GCParams;
		GCParams.Location = SourceLocation + (LaunchDirection * 100.0f);
		GCParams.Normal = LaunchDirection;
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::SlowBullet, GCParams);
	}
}
