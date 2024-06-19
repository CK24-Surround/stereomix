// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Launch_Single.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacter.h"

void USMGameplayAbility_Launch_Single::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// 투사체의 발사위치와 방향을 서버로 보냅니다.
	if (ActorInfo->IsLocallyControlled())
	{
		const FVector StartLocation = SourceCharacter->GetActorLocation();
		const FVector CursorLocation = SourceCharacter->GetCursorTargetingPoint();
		const FVector ProjectileDirection = (CursorLocation - StartLocation).GetSafeNormal2D();
		ServerRPCSendAimingData(StartLocation, ProjectileDirection);
	}

}

void USMGameplayAbility_Launch_Single::ServerRPCSendAimingData_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantizeNormal& Normal)
{
	LaunchProjectile(SourceLocation, Normal);
}

void USMGameplayAbility_Launch_Single::ExecuteLaunchFX(int32 LaunchCount)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// 발사 FX를 재생합니다. RawMagnitude에 발사횟수를 넣어 초탄에만 사운드를 적용할수도 있도록했습니다.
	FGameplayCueParameters GCParams;
	GCParams.Location = FVector::ForwardVector * 100.0f;
	GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
	GCParams.RawMagnitude = LaunchCount;
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileLaunch, GCParams);
}