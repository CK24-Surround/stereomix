// Copyright Surround, Inc. All Rights Reserved.


#include "SMDamageProjectile.h"

#include "AbilitySystem/SMTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "Utilities/SMLog.h"


ASMDamageProjectile::ASMDamageProjectile()
{
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Neutralize);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Immune);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::NoiseBreaking);
}

void ASMDamageProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASMDamageProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASMDamageProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority())
	{
		// 유효한 타겟인지 검증합니다.
		if (!IsValidateTarget(OtherActor))
		{
			return;
		}

		ApplyDamage(OtherActor);
		ExecuteHitFX(OtherActor);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
}

void ASMDamageProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (HasAuthority())
	{
		ExecuteWallHitFX();

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

bool ASMDamageProjectile::IsValidateTarget(AActor* InTarget)
{
	if (!Super::IsValidateTarget(InTarget))
	{
		return false;
	}

	// 투사체가 무소속인 경우 무시합니다.
	const ESMTeam SourceTeam = GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		return false;
	}

	ISMTeamInterface* TargetTeamInterface = Cast<ISMTeamInterface>(InTarget);
	if (!TargetTeamInterface)
	{
		return false;
	}

	// 타겟이 무소속인 경우 무시합니다.
	const ESMTeam TargetTeam = TargetTeamInterface->GetTeam();
	if (TargetTeam == ESMTeam::None)
	{
		return false;
	}

	// 투사체와 타겟이 같은 팀이라면 무시합니다. (팀킬 방지)
	if (SourceTeam == TargetTeam)
	{
		return false;
	}

	return true;
}

void ASMDamageProjectile::ApplyDamage(AActor* InTarget)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget);
	if (!ensureAlways(TargetASC))
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(DamageGE, 1.0f, SourceASC->MakeEffectContext());
	if (!ensureAlways(GESpecHandle.IsValid()))
	{
		return;
	}

	// SetByCaller를 통해 매개변수로 전달받은 Damage로 GE를 적용합니다.
	GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::Data::Damage, Magnitude);
	SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);

	// 공격자 정보도 저장합니다.
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(TargetASC->GetAvatarActor());
	if (ensureAlways(TargetDamageInterface))
	{
		TargetDamageInterface->SetLastAttackInstigator(SourceASC->GetAvatarActor());
	}
}

void ASMDamageProjectile::ExecuteHitFX(AActor* InTarget)
{
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget);
	if (!ensureAlways(TargetASC))
	{
		return;
	}

	const AActor* TargetActor = TargetASC->GetAvatarActor();
	if (!ensureAlways(TargetActor))
	{
		return;
	}

	// 적중한 타겟에게 FX를 재생시킵니다.
	FGameplayCueParameters GCParams;
	GCParams.Location = TargetActor->GetActorLocation();
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileHit, GCParams);
}

void ASMDamageProjectile::ExecuteWallHitFX()
{
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	// 적중한 타겟에게 FX를 재생시킵니다.
	FGameplayCueParameters GCParams;
	GCParams.Location = GetActorLocation();
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileWallHit, GCParams);
}
