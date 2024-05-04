// Copyright Surround, Inc. All Rights Reserved.


#include "SMDamageProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Components/SMTeamComponent.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMTeamComponentInterface.h"


ASMDamageProjectile::ASMDamageProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
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

	// 유효한 타겟인지 검증합니다.
	if (!IsValidateTarget(OtherActor))
	{
		return;
	}

	if (HasAuthority())
	{
		ApplyDamage(OtherActor);
		ApplyFX(OtherActor);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		MulticastRPCEndLifeTime();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void ASMDamageProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (HasAuthority())
	{
		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		MulticastRPCEndLifeTime();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

bool ASMDamageProjectile::IsValidateTarget(AActor* InTarget)
{
	// 자신이 발사한 투사체에 적중되지 않도록 합니다. (자살 방지)
	if (InTarget == GetOwner())
	{
		return false;
	}

	// 투사체가 무소속인 경우 무시합니다.
	const ESMTeam SourceTeam = TeamComponent->GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		return false;
	}

	ISMTeamComponentInterface* TargetTeamComponentInterface = Cast<ISMTeamComponentInterface>(InTarget);
	if (!TargetTeamComponentInterface)
	{
		return false;
	}

	USMTeamComponent* OtherTeamComponent = TargetTeamComponentInterface->GetTeamComponent();
	if (!ensureAlways(OtherTeamComponent))
	{
		return false;
	}

	// 타겟이 무소속인 경우 무시합니다.
	const ESMTeam OtherTeam = OtherTeamComponent->GetTeam();
	if (OtherTeam == ESMTeam::None)
	{
		return false;
	}

	// 투사체와 타겟이 같은 팀이라면 무시합니다. (팀킬 방지)
	if (SourceTeam == OtherTeam)
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget);
	if (!ensureAlways(TargetASC))
	{
		return false;
	}

	// 타겟이 되지 않아야하는 상태라면 무시합니다.
	if (TargetASC->HasAnyMatchingGameplayTags(IgnoreTargetStateTags))
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
	GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::Data::Damage, Damage);
	SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);

	// 공격자 정보도 저장합니다.
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(TargetASC->GetAvatarActor());
	if (ensureAlways(TargetDamageInterface))
	{
		TargetDamageInterface->SetLastAttackInstigator(SourceASC->GetAvatarActor());
	}
}

void ASMDamageProjectile::ApplyFX(AActor* InTarget)
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
