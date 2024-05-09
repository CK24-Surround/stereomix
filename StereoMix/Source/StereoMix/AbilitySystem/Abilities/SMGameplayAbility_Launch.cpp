// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Launch.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "AbilitySystem/SMTags.h"
#include "Utilities/SMLog.h"

USMGameplayAbility_Launch::USMGameplayAbility_Launch()
{
	FGameplayTagContainer BlockedTags;
	BlockedTags.AddTag(SMTags::Ability::Activation::Catch);
	BlockedTags.AddTag(SMTags::Character::State::Catch);
	BlockedTags.AddTag(SMTags::Character::State::Caught);
	BlockedTags.AddTag(SMTags::Character::State::Smashing);
	BlockedTags.AddTag(SMTags::Character::State::Smashed);
	BlockedTags.AddTag(SMTags::Character::State::Stun);
	BlockedTags.AddTag(SMTags::Character::State::Immune);

	ActivationBlockedTags = BlockedTags;
}

void USMGameplayAbility_Launch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// 로컬에서는 투사체의 발사위치와 방향을 저장해서 서버로 보냅니다.
	if (ActorInfo->IsLocallyControlled())
	{
		const FVector StartLocation = SourceCharacter->GetActorLocation();
		const FVector CursorLocation = SourceCharacter->GetCursorTargetingPoint();
		const FVector ProjectileDirection = (CursorLocation - StartLocation).GetSafeNormal2D();

		FGameplayAbilityTargetData_LocationInfo* ProjectileData = new FGameplayAbilityTargetData_LocationInfo;
		ProjectileData->SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		ProjectileData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		ProjectileData->SourceLocation.LiteralTransform.SetLocation(StartLocation);
		ProjectileData->TargetLocation.LiteralTransform.SetLocation(ProjectileDirection);

		FGameplayAbilityTargetDataHandle ProjectileDataHandle(ProjectileData);
		SourceASC->ServerSetReplicatedTargetData(Handle, ActivationInfo.GetActivationPredictionKey(), ProjectileDataHandle, FGameplayTag(), SourceASC->ScopedPredictionKey);
	}

	// 서버는 이를 수신 대기합니다.
	if (ActorInfo->IsNetAuthority())
	{
		FAbilityTargetDataSetDelegate& TargetDataDelegate = SourceASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey());
		TargetDataDelegate.AddUObject(this, &ThisClass::OnReceiveProjectileTargetData);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	// 발사 애니메이션을 재생합니다.
	SourceASC->PlayMontage(this, ActivationInfo, Montage, 1.0f);

	// FX를 재생합니다.
	FGameplayCueParameters GCParams;
	GCParams.Location = SourceCharacter->GetActorLocation() + SourceCharacter->GetActorForwardVector() * 100.0f;
	GCParams.Normal = FRotationMatrix(SourceCharacter->GetActorRotation()).GetUnitAxis(EAxis::X);

	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileLaunch, GCParams);
}

void USMGameplayAbility_Launch::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		const FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), 1.0f);
		if (!ensureAlways(GESpecHandle.IsValid()))
		{
			return;
		}

		FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
		if (!ensureAlways(GESpec))
		{
			return;
		}

		const float Cooldown = 1.0f / LaunchRate;
		GESpec->SetSetByCallerMagnitude(SMTags::Data::Cooldown, Cooldown);
		(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, GESpecHandle);
	}
}

void USMGameplayAbility_Launch::OnReceiveProjectileTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag)
{
	const FGameplayAbilityTargetData* ProjectileDataHandle = GameplayAbilityTargetDataHandle.Get(0);
	if (!ensureAlways(ProjectileDataHandle))
	{
		EndAbilityByCancel();
		return;
	}

	// 클라이언트에서 전송받은 위치를 저장합니다.
	const FVector StartLocation = ProjectileDataHandle->GetOrigin().GetLocation();
	const FVector ProjectileDirection = ProjectileDataHandle->GetEndPoint();

	LaunchProjectile(StartLocation, ProjectileDirection);
}
