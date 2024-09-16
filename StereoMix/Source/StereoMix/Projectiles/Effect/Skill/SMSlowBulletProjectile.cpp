// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlowBulletProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Utilities/SMLog.h"


void ASMSlowBulletProjectile::Init(float NewSlowDebuffMultiplier, float NewSlowDebuffDuration)
{
	if (!HasAuthority())
	{
		return;
	}

	SlowDebuffMultiplier = NewSlowDebuffMultiplier;
	SlowDebuffDuration = NewSlowDebuffDuration;
}

void ASMSlowBulletProjectile::HandleHitEffect(AActor* InTarget)
{
	Super::HandleHitEffect(InTarget);

	ApplyDamage(InTarget);
	ApplySlowEffect(InTarget);
}

void ASMSlowBulletProjectile::PlayHitFX(AActor* InTarget)
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (InTarget && SourceASC)
	{
		FGameplayCueParameters GCParams;
		GCParams.Location = InTarget->GetActorLocation();
		SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::SlowBulletHit, GCParams);
	}
}

void ASMSlowBulletProjectile::PlayWallHitFX(const FVector& HitLocation)
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

void ASMSlowBulletProjectile::ApplySlowEffect(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return;
	}

	// 대상이 캐릭터면 디버프를 적용합니다.
	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (TargetCharacter)
	{
		TargetCharacter->ClientRPCAddMoveSpeed(SlowDebuffMultiplier, SlowDebuffDuration);
	}

	NET_LOG(this, Warning, TEXT("%s에게 마비탄 적중"), *GetNameSafe(TargetActor));
}
