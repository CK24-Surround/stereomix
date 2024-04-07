// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Launch.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystems/AttributeSets/StereoMixCharacterAttributeSet.h"
#include "AbilityTasks/StereoMixAbilityTask_SpawnAndLaunchProjectile.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "Utilities/StereoMixeLog.h"

UStereoMixGameplayAbility_Launch::UStereoMixGameplayAbility_Launch()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UStereoMixGameplayAbility_Launch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		NET_LOG(ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr, Error, TEXT("ActorInfo 혹은 AbilitySystemComponent가 유효하지 않습니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	const UStereoMixCharacterAttributeSet* CharacterAttributeSet = ASC->GetSet<UStereoMixCharacterAttributeSet>();
	if (!CharacterAttributeSet)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("호환되지 않는 GA입니다. UStereoMixCharacterAttributeSet를 갖고 있어야합니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AStereoMixPlayerCharacter* StereoMixPlayerCharacter = Cast<AStereoMixPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!StereoMixPlayerCharacter)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("아바타는 AStereoMixPlayerCharacter여야 합니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	if (!ActorInfo->IsNetAuthority())
	{
		const FVector StartLocation = StereoMixPlayerCharacter->GetActorLocation();
		const FVector TargetLocation = StereoMixPlayerCharacter->GetCursorTargetingPoint();
		ServerRPCRequestSpawnProjectile(StartLocation, TargetLocation);
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (!AnimInstance)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("애님 인스턴스가 존재하지 않습니다."));
	}

	ASC->PlayMontage(this, ActivationInfo, Montage, 1.0f);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UStereoMixGameplayAbility_Launch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UStereoMixGameplayAbility_Launch::OnFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
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
