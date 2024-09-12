// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Shoot.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Games/SMGameState.h"
#include "Projectiles/SMProjectile.h"
#include "Projectiles/SMProjectilePoolManagerComponent.h"

USMGA_Shoot::USMGA_Shoot()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationBlockedTags.AddTag(SMTags::Character::State::SlowBullet);
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
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
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

	const int RandomValue = FMath::Rand() % 100;
	float RandomYaw;
	if (RandomValue < AccuracyShootRate)
	{
		RandomYaw = (FMath::Rand() % 10) - 5.0f; // 10도 범위
	}
	else
	{
		RandomYaw = (FMath::Rand() % ShootAngle) - (ShootAngle / 2.0f);
	}

	const FRotator LaunchRotation = (TargetLocation - SourceLocation).Rotation() + FRotator(0.0, RandomYaw, 0.0);
	const float MaxDistance = MaxDistanceByTile * 150.0f;
	Projectile->Launch(SourceCharacter, SourceLocation, LaunchRotation.Vector(), ProjectileSpeed, MaxDistance, Damage);
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
	const FVector TargetLocation = SourceCharacter->GetLocationFromCursor();
	ServerRPCLaunchProjectile(SourceLocation, TargetLocation);
	ServerRPCApplyCost();

	const FName SectionName = TEXT("Default");
	MontageJumpToSection(SectionName);
}

void USMGA_Shoot::ServerRPCApplyCost_Implementation()
{
	K2_CommitAbilityCost();
}
