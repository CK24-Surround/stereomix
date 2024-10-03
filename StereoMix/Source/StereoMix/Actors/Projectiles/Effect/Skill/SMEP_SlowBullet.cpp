// Copyright Surround, Inc. All Rights Reserved.


#include "SMEP_SlowBullet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Utilities/SMLog.h"


void ASMEP_SlowBullet::PreLaunch(const FSMProjectileParameters& InParameters)
{
	Super::PreLaunch(InParameters);

	SlowDebuffMultiplier = InParameters.Magnitude;
	SlowDebuffDuration = InParameters.Duration;
}

void ASMEP_SlowBullet::AddProjectileFX()
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (SourceASC)
	{
		FGameplayCueParameters GCParams;
		GCParams.TargetAttachComponent = GetRootComponent();
		SourceASC->AddGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletProjectile, GCParams);
	}
}

void ASMEP_SlowBullet::RemoveProjectileFX()
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (SourceASC)
	{
		SourceASC->RemoveGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletProjectile, FGameplayCueParameters());
	}
}

void ASMEP_SlowBullet::HandleHitEffect(AActor* InTarget)
{
	Super::HandleHitEffect(InTarget);

	ApplyDamage(InTarget);
	ApplySlowEffect(InTarget);
}

void ASMEP_SlowBullet::PlayHitFX(AActor* InTarget)
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (InTarget && SourceASC)
	{
		FGameplayCueParameters GCParams;
		GCParams.TargetAttachComponent = InTarget->GetRootComponent();
		SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletHit, GCParams);
	}
}

void ASMEP_SlowBullet::PlayWallHitFX(const FVector& HitLocation)
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (SourceASC)
	{
		FGameplayCueParameters GCParams;
		GCParams.Location = HitLocation;
		SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletHit, GCParams);
	}
}

void ASMEP_SlowBullet::ApplySlowEffect(AActor* TargetActor)
{
	// 대상이 캐릭터면 디버프를 적용합니다.
	if (ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor))
	{
		TargetCharacter->ClientRPCAddMoveSpeed(SlowDebuffMultiplier, SlowDebuffDuration);

		// 디버프 VFX를 적용합니다. 이 이펙트는 스스로 종료됩니다.
		AActor* SourceActor = GetOwner();
		USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
		if (SourceASC)
		{
			FGameplayCueParameters GCParams;
			GCParams.RawMagnitude = SlowDebuffDuration;
			GCParams.TargetAttachComponent = TargetActor->GetRootComponent();
			SourceASC->AddGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletDebuff, GCParams);
		}
	}

	NET_LOG(this, Log, TEXT("%s에게 마비탄 적중"), *GetNameSafe(TargetActor));
}
