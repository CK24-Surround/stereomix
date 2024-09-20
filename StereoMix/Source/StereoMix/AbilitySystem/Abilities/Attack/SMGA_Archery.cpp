// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Archery.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Characters/Weapon/SMWeaponBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "Games/SMGameState.h"
#include "Projectiles/SMProjectile.h"
#include "Projectiles/Pool/SMProjectilePoolManagerComponent.h"

USMGA_Archery::USMGA_Archery()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationBlockedTags.AddTag(SMTags::Character::State::ImpactArrow);

	Damage = 34.0f;

	if (FSMCharacterAttackData* AttackData = GetAttackData(ESMCharacterType::Piano))
	{
		Damage = AttackData->Damage;
		MaxDistanceByTile = AttackData->DistanceByTile;
		ProjectileSpeed = AttackData->ProjectileSpeed;
	}
}

void USMGA_Archery::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceASC || !SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	if (!K2_CheckAbilityCost())
	{
		return;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	const FName TaskName = TEXT("MontageTask");
	UAnimMontage* Montage = SourceDataAsset->AttackMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage, 1.0f, NAME_None, false);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		UAbilityTask_WaitDelay* Charge1Task = UAbilityTask_WaitDelay::WaitDelay(this, Charge1Time);
		Charge1Task->OnFinish.AddDynamic(this, &ThisClass::OnCharged1);
		Charge1Task->ReadyForActivation();

		UAbilityTask_WaitDelay* Charge2Task = UAbilityTask_WaitDelay::WaitDelay(this, Charge2Time);
		Charge2Task->OnFinish.AddDynamic(this, &ThisClass::OnCharged2);
		Charge2Task->ReadyForActivation();

		FGameplayCueParameters GCParams;
		ASMWeaponBase* SourceWeapon = SourceCharacter->GetWeapon();
		if (SourceWeapon)
		{
			GCParams.TargetAttachComponent = SourceWeapon->GetWeaponMeshComponent();
		}
		SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Piano::Archery, GCParams);
	}
}

void USMGA_Archery::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>())
	{
		if (IsLocallyControlled())
		{
			SourceASC->RemoveGC(GetAvatarActor(), SMTags::GameplayCue::Piano::Archery, FGameplayCueParameters());
		}
	}

	ChargingLevel = 0;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Archery::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!SourceCharacter || !SourceASC)
	{
		return;
	}

	SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Piano::Archery, FGameplayCueParameters());

	if (ChargingLevel == 0)
	{
		const FName CancelSectionName = TEXT("Cancel");
		MontageJumpToSection(CancelSectionName);
		K2_EndAbility();
	}
	else if (ChargingLevel == 1)
	{
		Charge1();
	}
	else
	{
		Charge2();
	}

	const FName EndSectionName = TEXT("End");
	MontageJumpToSection(EndSectionName);
}

void USMGA_Archery::OnCharged1()
{
	ChargingLevel = 1;
}

void USMGA_Archery::OnCharged2()
{
	ChargingLevel = 2;
}

void USMGA_Archery::Charge1()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	ServerApplyCost();

	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	const FVector TargetLocation = SourceCharacter->GetLocationFromCursor();
	ServerRPCLaunchProjectile(SourceLocation, TargetLocation, 1);
}

void USMGA_Archery::Charge2()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	ServerApplyCost();
	ServerApplyCost();

	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	const FVector TargetLocation = SourceCharacter->GetLocationFromCursor();
	ServerRPCLaunchProjectile(SourceLocation, TargetLocation, 2);
}

void USMGA_Archery::ServerRPCLaunchProjectile_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation, int32 InChargingLevel)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>();
	USMProjectilePoolManagerComponent* ProjectilePoolManager = GameState ? GameState->GetProjectilePoolManager() : nullptr;
	if (!SourceCharacter || !ProjectilePoolManager)
	{
		EndAbilityByCancel();
		return;
	}

	float NewDamage;
	ASMProjectile* Projectile;
	if (InChargingLevel == 1)
	{
		NewDamage = Damage * Charge1DamageMultiply;
		Projectile = ProjectilePoolManager->GetProjectileForPiano1(SourceCharacter->GetTeam());
	}
	else
	{
		NewDamage = Damage * Charge2DamageMultiply;
		Projectile = ProjectilePoolManager->GetProjectileForPiano2(SourceCharacter->GetTeam());
	}

	if (!Projectile)
	{
		EndAbilityByCancel();
		return;
	}

	const FVector LaunchDirection = (TargetLocation - SourceLocation).GetSafeNormal();

	FSMProjectileParameters ProjectileParams;
	ProjectileParams.Owner = SourceCharacter;
	ProjectileParams.StartLocation = SourceLocation;
	ProjectileParams.Normal = LaunchDirection;
	ProjectileParams.Damage = NewDamage;
	ProjectileParams.Speed = ProjectileSpeed;
	ProjectileParams.MaxDistance = MaxDistanceByTile * 150.0f;
	Projectile->Launch(ProjectileParams);

	K2_EndAbility();
}

void USMGA_Archery::ServerApplyCost_Implementation()
{
	K2_CommitAbilityCost();
}
