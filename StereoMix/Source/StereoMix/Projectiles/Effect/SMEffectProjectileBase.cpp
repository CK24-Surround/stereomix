// Copyright Studio Surround. All Rights Reserved.


#include "SMEffectProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Gimmick/SMFragileObstacle.h"

ASMEffectProjectileBase::ASMEffectProjectileBase()
{
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Neutralize);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Immune);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::NoiseBreak);
}

void ASMEffectProjectileBase::PreLaunch(const FSMProjectileParameters& InParameters)
{
	Damage = InParameters.Damage;
}

bool ASMEffectProjectileBase::IsValidateTarget(AActor* InTarget)
{
	if (!Super::IsValidateTarget(InTarget))
	{
		return false;
	}
	
	ASMFragileObstacle* DestroyableObstacle = Cast<ASMFragileObstacle>(InTarget);
	if (DestroyableObstacle)
	{
		return true;
	}

	// 투사체가 무소속인 경우 무시합니다.
	const ESMTeam SourceTeam = GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		return false;
	}

	// 타겟이 무소속인 경우 무시합니다.
	const ESMTeam TargetTeam = USMTeamBlueprintLibrary::GetTeam(InTarget);
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

void ASMEffectProjectileBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority())
	{
		// 유효한 타겟인지 검증합니다.
		if (!IsValidateTarget(OtherActor))
		{
			return;
		}

		HandleHitEffect(OtherActor);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
}

void ASMEffectProjectileBase::HandleHitEffect(AActor* InTarget)
{
	PlayHitFX(InTarget);
}

void ASMEffectProjectileBase::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (HasAuthority())
	{
		HandleWallHitEffect(HitLocation);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void ASMEffectProjectileBase::HandleWallHitEffect(const FVector& HitLocation)
{
	PlayWallHitFX(HitLocation);
}

void ASMEffectProjectileBase::ApplyDamage(AActor* InTarget)
{
	ASMPlayerCharacterBase* SourceCharacter = GetOwner<ASMPlayerCharacterBase>();
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceCharacter);
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget);
	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceDataAsset)
	{
		return;
	}

	ASMFragileObstacle* DestroyableObstacle = Cast<ASMFragileObstacle>(InTarget);
	if (DestroyableObstacle)
	{
		DestroyableObstacle->HandleDurability(Damage);
		return;
	}

	if (!TargetASC)
	{
		return;
	}

	FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(SourceDataAsset->DamageGE, 1.0f, SourceASC->MakeEffectContext());
	if (GESpecHandle.IsValid())
	{
		// SetByCaller를 통해 매개변수로 전달받은 Damage로 GE를 적용합니다.
		GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, Damage);
		SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);
	}

	// 공격자 정보도 저장합니다.
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(InTarget);
	if (TargetDamageInterface)
	{
		TargetDamageInterface->SetLastAttackInstigator(SourceCharacter);
	}
}
