// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Archery.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Projectiles/SMProjectile.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMProjectileFunctionLibrary.h"

USMGA_Archery::USMGA_Archery()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationBlockedTags.AddTag(SMTags::Character::State::Piano::ImpactArrow);

	if (const FSMCharacterAttackData* AttackData = USMDataTableFunctionLibrary::GetCharacterAttackData(ESMCharacterType::Piano))
	{
		Damage = AttackData->Damage;
		MaxDistanceByTile = AttackData->DistanceByTile;
		ProjectileSpeed = AttackData->ProjectileSpeed;
	}
}

void USMGA_Archery::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
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
		if (const UWorld* World = GetWorld())
		{
			TWeakObjectPtr<USMGA_Archery> ThisWeakPtr = MakeWeakObjectPtr(this);
			auto OnCharged = [ThisWeakPtr](int32 Level) {
				if (ThisWeakPtr.Get())
				{
					ThisWeakPtr->ChargedLevel = Level;
					ASMPlayerCharacterBase* Character = ThisWeakPtr->GetCharacter();
					const USMAbilitySystemComponent* ASC = ThisWeakPtr->GetASC();
					if (Character && ASC)
					{
						const FGameplayTag GCTag = Level == 1 ? SMTags::GameplayCue::Piano::ArcheryCharge1 : SMTags::GameplayCue::Piano::ArcheryCharge2;
						FGameplayCueParameters GCParams;
						GCParams.SourceObject = Character;
						GCParams.Location = Character->GetActorLocation();
						ASC->ExecuteGC(Character, GCTag, GCParams);
					}
				}
			};

			World->GetTimerManager().SetTimer(Level1ChargedTimerHandle, [OnCharged] { OnCharged(1); }, TimeToChargeLevel1, false);
			World->GetTimerManager().SetTimer(Level2ChargedTimerHandle, [OnCharged] { OnCharged(2); }, TimeToChargeLevel2, false);
		}

		const ASMWeaponBase* SourceWeapon = SourceCharacter->GetWeapon();
		UMeshComponent* SourceWeaponMesh = SourceWeapon ? SourceWeapon->GetWeaponMeshComponent() : nullptr;

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.TargetAttachComponent = SourceWeaponMesh;
		SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Piano::Archery, GCParams);
	}
}

void USMGA_Archery::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsLocallyControlled())
	{
		ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
		const USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
		if (SourceCharacter && SourceASC)
		{
			// 만약에 쏘지못하고 끊기게 되는 경우의 예외처리입니다.
			FGameplayCueParameters GCParams;
			GCParams.SourceObject = SourceCharacter;
			GCParams.RawMagnitude = -1;
			SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Piano::Archery, GCParams);
		}

		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(Level1ChargedTimerHandle);
			World->GetTimerManager().ClearTimer(Level2ChargedTimerHandle);
		}
	}

	ChargedLevel = 0;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Archery::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	if (!SourceCharacter || !SourceASC)
	{
		return;
	}

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	GCParams.RawMagnitude = ChargedLevel;
	SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Piano::Archery, GCParams);

	if (ChargedLevel > 0)
	{
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector TargetLocation = SourceCharacter->GetLocationFromCursor();
		ServerLaunchProjectile(SourceLocation, TargetLocation, ChargedLevel);

		const FName EndSectionName = TEXT("End");
		MontageJumpToSection(EndSectionName);
	}
	else
	{
		const FName CancelSectionName = TEXT("Cancel");
		MontageJumpToSection(CancelSectionName);
	}

	K2_EndAbility();
}

void USMGA_Archery::ServerLaunchProjectile_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation, int32 InChargedLevel)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	ASMProjectile* Projectile = USMProjectileFunctionLibrary::GetPianoProjectile(GetWorld(), SourceTeam, InChargedLevel);
	if (!SourceCharacter || !Projectile)
	{
		EndAbilityByCancel();
		return;
	}

	for (int32 i = 0; i < InChargedLevel; ++i)
	{
		K2_CommitAbilityCost();
	}

	const float NewDamage = InChargedLevel == 1 ? Damage * Charge1DamageMultiply : Damage * Charge2DamageMultiply;
	const FVector LaunchDirection = (TargetLocation - SourceLocation).GetSafeNormal();

	FSMProjectileParameters ProjectileParams;
	ProjectileParams.Owner = SourceCharacter;
	ProjectileParams.StartLocation = SourceLocation;
	ProjectileParams.Normal = LaunchDirection;
	ProjectileParams.Damage = NewDamage;
	ProjectileParams.Speed = ProjectileSpeed;
	ProjectileParams.MaxDistance = MaxDistanceByTile * 150.0f;
	Projectile->Launch(ProjectileParams);
}
