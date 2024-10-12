// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_SlowBullet.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Projectiles/SMProjectile.h"
#include "Actors/Projectiles/Effect/SMEffectProjectileBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMProjectileFunctionLibrary.h"

USMGA_SlowBullet::USMGA_SlowBullet()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	ActivationOwnedTags.AddTag(SMTags::Character::State::ElectricGuitar::SlowBullet);

	if (const FSMCharacterSkillData* SkillData = USMDataTableFunctionLibrary::GetCharacterSkillData(ESMCharacterType::ElectricGuitar))
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
	const USMAbilitySystemComponent* SourceASC = GetASC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceASC || !SourceDataAsset)
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
		UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::ElectricGuitar::SlowBulletShoot);
		WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		WaitEventTask->ReadyForActivation();

		const ASMWeaponBase* SourceWeapon = SourceCharacter->GetWeapon();
		if (UMeshComponent* SourceWeaponMesh = SourceWeapon ? SourceWeapon->GetWeaponMeshComponent() : nullptr)
		{
			const FVector LocationOffset(5.0, -5.0, -50.0);
			const FRotator RotationOffset(90.0, -90.0, 0.0);

			FGameplayCueParameters GCParams;
			GCParams.SourceObject = SourceCharacter;
			GCParams.TargetAttachComponent = SourceWeaponMesh;
			GCParams.Location = LocationOffset;
			GCParams.Normal = RotationOffset.Vector();
			SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::SlowBulletCharge, GCParams);
		}
	}
}

void USMGA_SlowBullet::OnEventReceived(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (!SourceCharacter)
	{
		K2_EndAbility();
		return;
	}

	SourceLocation = SourceCharacter->GetActorLocation();
	TargetLocation = SourceCharacter->GetLocationFromCursor();
	ServerRPCLaunchProjectile(SourceLocation, TargetLocation);

	SyncPointEndAbility();
}

void USMGA_SlowBullet::ServerRPCLaunchProjectile_Implementation(const FVector_NetQuantize10& InSourceLocation, const FVector_NetQuantize10& InTargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	ASMEffectProjectileBase* Projectile = Cast<ASMEffectProjectileBase>(USMProjectileFunctionLibrary::GetSlowBulletProjectile(GetWorld(), SourceTeam));
	if (!SourceCharacter || !Projectile)
	{
		EndAbilityByCancel();
		return;
	}

	const FVector LaunchDirection = (InTargetLocation - InSourceLocation).GetSafeNormal(UE_SMALL_NUMBER, FVector::ForwardVector);
	const float MaxDistance = MaxDistanceByTile * 150.0f;

	Projectile->OnProjectileHitDelegate.AddDynamic(this, &ThisClass::OnProjectileHit);

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

	if (const USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>())
	{
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
		GCParams.Location = FVector(100.0, 0.0, 0.0);
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

void USMGA_SlowBullet::OnProjectileHit(AActor* HitActor)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (!CurrentActorInfo || !SourceCharacter)
	{
		return;
	}

	// 대상이 캐릭터면 디버프를 적용합니다.
	if (ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(HitActor))
	{
		TargetCharacter->ClientRPCAddMoveSpeed(SlowDebuffMultiplier, SlowDebuffDuration);

		// 디버프 VFX를 적용합니다. 이 이펙트는 스스로 종료됩니다.
		if (const USMAbilitySystemComponent* SourceASC = GetASC())
		{
			FGameplayCueParameters GCParams;
			GCParams.RawMagnitude = SlowDebuffDuration;
			GCParams.TargetAttachComponent = HitActor->GetRootComponent();
			SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::SlowBulletDebuff, GCParams);
		}

		NET_LOG(SourceCharacter, Log, TEXT("%s에게 %f초간 슬로우 적용"), *GetNameSafe(HitActor), SlowDebuffDuration);
	}

	// 데미지를 적용합니다.
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(HitActor);
	if (TargetDamageInterface && !TargetDamageInterface->CanIgnoreAttack())
	{
		TargetDamageInterface->ReceiveDamage(SourceCharacter, Damage);

		// 카메라셰이크를 적용합니다.
		if (const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset())
		{
			if (APlayerController* PlayerController = CurrentActorInfo->PlayerController.Get())
			{
				PlayerController->ClientStartCameraShake(SourceDataAsset->SkillHitCameraShake);
			}

			const APawn* HitPawn = Cast<APawn>(HitActor);
			if (APlayerController* HitPlayerController = HitPawn ? HitPawn->GetController<APlayerController>() : nullptr)
			{
				HitPlayerController->ClientStartCameraShake(SourceDataAsset->SkillHitCameraShake);
			}
		}
	}

	OnSkillHit.Broadcast();
}
