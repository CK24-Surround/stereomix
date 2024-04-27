// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Launch.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilityTasks/SMAbilityTask_SpawnAndLaunchProjectile.h"
#include "NiagaraSystem.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/SMTeamComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Games/SMGameMode.h"
#include "Projectiles/SMProjectile.h"
#include "Projectiles/SMProjectilePool.h"
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

	ActivationBlockedTags = BlockedTags;

	LaunchFX.Add(ESMTeam::None, nullptr);
	LaunchFX.Add(ESMTeam::EDM, nullptr);
	LaunchFX.Add(ESMTeam::FutureBass, nullptr);
}

void USMGameplayAbility_Launch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
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

	// 이후 이펙트를 재생합니다. 리모트의 경우는 서버에서 타겟 데이터를 수신할때 전파하여 재생하도록합니다.
	USMTeamComponent* SourceTeamComponent = SourceCharacter->GetTeamComponent();
	if (ensure(SourceTeamComponent))
	{
		const ESMTeam Team = SourceTeamComponent->GetTeam();
		FGameplayCueParameters GCParams;
		GCParams.Location = SourceCharacter->GetActorLocation() + SourceCharacter->GetActorForwardVector() * 100.0f;
		GCParams.Normal = FRotationMatrix(SourceCharacter->GetActorRotation()).GetUnitAxis(EAxis::X);
		GCParams.SourceObject = LaunchFX[Team];

		if (SourceASC)
		{
			SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::PlayNiagara, GCParams);
		}
	}
}

void USMGameplayAbility_Launch::OnReceiveProjectileTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASMGameMode* CachedSMGameMode = GetWorld()->GetAuthGameMode<ASMGameMode>();
	if (!ensureAlways(CachedSMGameMode))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const FGameplayAbilityTargetData* ProjectileDataHandle = GameplayAbilityTargetDataHandle.Get(0);
	if (!ensureAlways(ProjectileDataHandle))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 클라이언트에서 전송받은 위치를 저장합니다.
	const FVector StartLocation = ProjectileDataHandle->GetOrigin().GetLocation();
	const FVector ProjectileDirection = ProjectileDataHandle->GetEndPoint();

	USMProjectilePool* ProjectilePool = CachedSMGameMode->GetProjectilePool();
	if (!ensureAlways(ProjectilePool))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASMProjectile* NewProjectile = ProjectilePool->GetProjectile();
	if (!NewProjectile)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	NewProjectile->Launch(SourceCharacter, StartLocation, ProjectileDirection, ProjectileSpeed, MaxDistance);
	K2_EndAbility();
}
