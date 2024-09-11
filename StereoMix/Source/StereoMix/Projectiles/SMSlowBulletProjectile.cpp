// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlowBulletProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Games/SMGameState.h"
#include "Utilities/SMLog.h"


ASMSlowBulletProjectile::ASMSlowBulletProjectile() {}

void ASMSlowBulletProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority())
	{
		// 유효한 타겟인지 검증합니다.
		if (!IsValidateTarget(OtherActor))
		{
			return;
		}

		ApplyEffect(OtherActor);
		// ExecuteHitFX(OtherActor);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
}

void ASMSlowBulletProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (HasAuthority())
	{
		// ExecuteWallHitFX();

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

bool ASMSlowBulletProjectile::IsValidateTarget(AActor* InTarget)
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

void ASMSlowBulletProjectile::ApplyEffect(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
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

	NET_LOG(this, Warning, TEXT("%s에게 마비탄 적중"), *GetNameSafe(TargetActor));
}
