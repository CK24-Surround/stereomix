// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_SlowBullet.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
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

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	const FName MontageTaskName = TEXT("MontageTask");
	UAnimMontage* SlowBulletMontage = SourceDataAsset->SkillMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, SlowBulletMontage, 1.0f, NAME_None, false);
	PlayMontageAndWait->ReadyForActivation();

	if (IsLocallyControlled())
	{
		SourceLocation = SourceCharacter->GetActorLocation();
		TargetLocation = SourceCharacter->GetLocationFromCursor();

		UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::SlowBulletShoot);
		WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		WaitEventTask->ReadyForActivation();
	}
}

void USMGA_SlowBullet::OnEventReceived(FGameplayEventData Payload)
{
	ServerRPCLaunchProjectile(SourceLocation, TargetLocation);

	SyncPointEndAbility();
}

void USMGA_SlowBullet::ServerRPCLaunchProjectile_Implementation(const FVector_NetQuantize10& InSourceLocation, const FVector_NetQuantize10& InTargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	UWorld* World = GetWorld();
	ASMGameState* GameState = World ? GetWorld()->GetGameState<ASMGameState>() : nullptr;
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

	const FVector LaunchDirection = (InTargetLocation - InSourceLocation).GetSafeNormal();
	const float MaxDistance = MaxDistanceByTile * 150.0f;

	FSMProjectileParameters ProjectileParams;
	ProjectileParams.Owner = SourceCharacter;
	ProjectileParams.StartLocation = InSourceLocation;
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
		GCParams.SourceObject = SourceCharacter;
		GCParams.Location = InSourceLocation + (LaunchDirection * 100.0f);
		GCParams.Normal = LaunchDirection;
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::SlowBullet, GCParams);
	}

	SyncPointEndAbility();
}

void USMGA_SlowBullet::SyncPointEndAbility()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}
