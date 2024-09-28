// Copyright Studio Surround. All Rights Reserved.


#include "SMEffectProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"

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

	// 대상이 데미지 인터페이스를 상속하고 있는지 확인합니다.
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(InTarget);
	if (!TargetDamageInterface)
	{
		return false;
	}

	// 투사체와 타겟이 같은 팀이라면 무시합니다. (팀킬 방지)
	const ESMTeam SourceTeam = GetTeam();
	const ESMTeam TargetTeam = USMTeamBlueprintLibrary::GetTeam(InTarget);
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
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(InTarget);
	const bool IsObstacle = TargetDamageInterface ? TargetDamageInterface->IsObstacle() : true;
	if (IsObstacle)
	{
		PlayWallHitFX(GetActorLocation());
	}
	else
	{
		PlayHitFX(InTarget);
	}
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
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(InTarget);
	if (!SourceCharacter || !TargetDamageInterface)
	{
		return;
	}

	TargetDamageInterface->ReceiveDamage(SourceCharacter, Damage);
}
