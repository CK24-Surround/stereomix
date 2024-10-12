// Copyright Surround, Inc. All Rights Reserved.


#include "SMEP_SlowBullet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Utilities/SMLog.h"


void ASMEP_SlowBullet::AddProjectileFX()
{
	AActor* SourceActor = GetOwner();
	if (const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceActor;
		GCParams.TargetAttachComponent = GetRootComponent();
		SourceASC->AddGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletProjectile, GCParams);
	}
}

void ASMEP_SlowBullet::RemoveProjectileFX()
{
	AActor* SourceActor = GetOwner();
	if (const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceActor;
		SourceASC->RemoveGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletProjectile, GCParams);
	}
}

void ASMEP_SlowBullet::HandleHitEffect(AActor* InTarget)
{
	Super::HandleHitEffect(InTarget);

	if (ASMPlayerCharacterBase* SourceCharacter = GetOwner<ASMPlayerCharacterBase>())
	{
		SourceCharacter->OnSkillHitSuccess();
	}
}

void ASMEP_SlowBullet::PlayHitFX(AActor* InTarget)
{
	AActor* SourceActor = GetOwner();
	const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (InTarget && SourceASC)
	{
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceActor;
		GCParams.TargetAttachComponent = InTarget->GetRootComponent();
		SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletHit, GCParams);
	}
}

void ASMEP_SlowBullet::PlayWallHitFX(const FVector& HitLocation)
{
	AActor* SourceActor = GetOwner();
	if (const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		FGameplayCueParameters GCParams;
		GCParams.Location = HitLocation;
		SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletHit, GCParams);
	}
}
