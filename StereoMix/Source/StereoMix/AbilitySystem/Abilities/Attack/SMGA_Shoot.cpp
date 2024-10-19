// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Shoot.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Projectiles/SMProjectile.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMProjectileFunctionLibrary.h"

USMGA_Shoot::USMGA_Shoot()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationBlockedTags.AddTag(SMTags::Character::State::ElectricGuitar::SlowBullet);

	if (const FSMCharacterAttackData* AttackData = USMDataTableFunctionLibrary::GetCharacterAttackData(ESMCharacterType::ElectricGuitar))
	{
		Damage = AttackData->Damage;
		MaxDistanceByTile = AttackData->DistanceByTile;
		AttackPerSecond = AttackData->AttackPerSecond;
		ProjectileSpeed = AttackData->ProjectileSpeed;
		SpreadAngle = AttackData->SpreadAngle;
		AccuracyShootRate = AttackData->AccuracyAttackRate;
	}
}

void USMGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	const FName MontageTaskName = TEXT("MontageTask");
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	UAnimMontage* ShootMontage = SourceDataAsset->AttackMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, ShootMontage, 1.0f, NAME_None, false);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(ShootTimerHandle, this, &USMGA_Shoot::Shoot, 1.0f / AttackPerSecond, true);
			Shoot();
		}
	}
}

void USMGA_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bIsInputReleased = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	bIsInputReleased = true;
}

void USMGA_Shoot::Shoot()
{
	if (bIsInputReleased) // 입력을 종료하면 기본 공격이 멈춥니다.
	{
		bIsInputReleased = false;

		K2_EndAbility();
		return;
	}

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (SourceCharacter && K2_CheckAbilityCost())
	{
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector TargetLocation = SourceCharacter->GetLocationFromCursor();
		ServerRPCLaunchProjectile(SourceLocation, TargetLocation);
		ServerRPCApplyCost();

		const FName SectionName = TEXT("Default");
		MontageJumpToSection(SectionName);
	}
}

void USMGA_Shoot::ServerRPCApplyCost_Implementation()
{
	K2_CommitAbilityCost();
}

void USMGA_Shoot::ServerRPCLaunchProjectile_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	ASMProjectile* Projectile = USMProjectileFunctionLibrary::GetElectricGuitarProjectile(GetWorld(), SourceTeam);
	if (!SourceCharacter || !Projectile)
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
		RandomYaw = (FMath::Rand() % static_cast<int32>(SpreadAngle)) - (SpreadAngle / 2.0f);
	}

	const FVector LaunchDirection = ((TargetLocation - SourceLocation).Rotation() + FRotator(0.0, RandomYaw, 0.0)).Vector();
	const float MaxDistance = MaxDistanceByTile * 150.0f;

	FSMProjectileParameters ProjectileParams;
	ProjectileParams.Owner = SourceCharacter;
	ProjectileParams.StartLocation = SourceLocation;
	ProjectileParams.Normal = LaunchDirection;
	ProjectileParams.Damage = Damage;
	ProjectileParams.Speed = ProjectileSpeed;
	ProjectileParams.MaxDistance = MaxDistance;
	Projectile->Launch(ProjectileParams);

	if (const USMAbilitySystemComponent* SourceASC = GetASC())
	{
		FGameplayCueParameters GCParams;
		GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
		GCParams.Location = FVector(150.0, 0.0, 0.0);
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::Shoot, GCParams);
	}
}
