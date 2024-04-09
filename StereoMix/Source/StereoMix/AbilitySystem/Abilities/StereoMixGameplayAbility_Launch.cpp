// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Launch.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "AbilityTasks/StereoMixAbilityTask_SpawnAndLaunchProjectile.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "Utilities/StereoMixeLog.h"

UStereoMixGameplayAbility_Launch::UStereoMixGameplayAbility_Launch()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UStereoMixGameplayAbility_Launch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AStereoMixPlayerCharacter* SourcePlayerCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (SourcePlayerCharacter)
	{
		if (!ActorInfo->IsNetAuthority())
		{
			const FVector StartLocation = SourcePlayerCharacter->GetActorLocation();
			const FVector TargetLocation = SourcePlayerCharacter->GetCursorTargetingPoint();
			ServerRPCRequestSpawnProjectile(StartLocation, TargetLocation);
		}

		CommitAbility(Handle, ActorInfo, ActivationInfo);
		
		UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
		if (SourceASC)
		{
			SourceASC->PlayMontage(this, ActivationInfo, Montage, 1.0f);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Launch::ServerRPCRequestSpawnProjectile_Implementation(const FVector_NetQuantize10& StartLocation, const FVector_NetQuantize10& CursorLocation)
{
	if (StartLocation.Z != CursorLocation.Z)
	{
		NET_LOG(CurrentActorInfo ? CurrentActorInfo->AvatarActor.Get() : nullptr, Warning, TEXT("캐릭터의 위치와 커서의 위치가 평면상에 놓이지 않았습니다. 투사체 시작점: %s 목표 위치: %s"), *StartLocation.ToString(), *CursorLocation.ToString());
	}

	const FVector Direction = (CursorLocation - StartLocation).GetSafeNormal();
	UStereoMixAbilityTask_SpawnAndLaunchProjectile* AbilityTaskSpawnProjectile = UStereoMixAbilityTask_SpawnAndLaunchProjectile::CreateTask(this, ProjectileClass, StartLocation, Direction);
	if (AbilityTaskSpawnProjectile)
	{
		AbilityTaskSpawnProjectile->ReadyForActivation();
	}
}
