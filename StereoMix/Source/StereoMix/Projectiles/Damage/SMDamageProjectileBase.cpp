// Copyright Surround, Inc. All Rights Reserved.


#include "SMDamageProjectileBase.h"

#include "AbilitySystem/SMTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Interfaces/SMDamageInterface.h"
#include "Utilities/SMLog.h"


ASMDamageProjectileBase::ASMDamageProjectileBase()
{
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Neutralize);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Immune);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::NoiseBreak);
}


bool ASMDamageProjectileBase::IsValidateTarget(AActor* InTarget)
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

void ASMDamageProjectileBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority())
	{
		// 유효한 타겟인지 검증합니다.
		if (!IsValidateTarget(OtherActor))
		{
			return;
		}

		HandleHit(OtherActor);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
}

void ASMDamageProjectileBase::HandleHit(AActor* InTarget)
{
	ApplyDamage(InTarget);
	PlayHitFX(InTarget);
}

void ASMDamageProjectileBase::ApplyDamage(AActor* InTarget)
{
	ASMPlayerCharacterBase* SourceCharacter = GetOwner<ASMPlayerCharacterBase>();
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceCharacter);
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget);
	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
	if (!SourceCharacter || !SourceASC || !TargetASC || !SourceDataAsset)
	{
		return;
	}

	FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(SourceDataAsset->DamageGE, 1.0f, SourceASC->MakeEffectContext());
	if (GESpecHandle.IsValid())
	{
		GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::Data::Damage, Magnitude);
		// SetByCaller를 통해 매개변수로 전달받은 Damage로 GE를 적용합니다.
		SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);
	}

	// 공격자 정보도 저장합니다.
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(InTarget);
	if (TargetDamageInterface)
	{
		TargetDamageInterface->SetLastAttackInstigator(SourceASC->GetAvatarActor());
	}
}

void ASMDamageProjectileBase::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (HasAuthority())
	{
		HandleWallHit(HitLocation);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void ASMDamageProjectileBase::HandleWallHit(const FVector& HitLocation)
{
	PlayWallHitFX(HitLocation);
}
