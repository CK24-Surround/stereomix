// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Launch.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilityTasks/SMAbilityTask_SpawnAndLaunchProjectile.h"
#include "NiagaraSystem.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/SMTeamComponent.h"
#include "AbilitySystem/SMTags.h"

USMGameplayAbility_Launch::USMGameplayAbility_Launch()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

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
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	// 마우스 커서 정보는 클라이언트에만 존재합니다.
	// 따라서 클라이언트의 커서정보를 기반으로 서버에 투사체 생성 요청을 하기 위해 해당 로직은 클라이언트에서만 실행되어야합니다.
	if (!ActorInfo->IsNetAuthority())
	{
		const FVector StartLocation = SourceCharacter->GetActorLocation();
		const FVector TargetLocation = SourceCharacter->GetCursorTargetingPoint();
		ServerRPCRequestProjectile(StartLocation, TargetLocation);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	SourceASC->PlayMontage(this, ActivationInfo, Montage, 1.0f);

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

	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::PlayNiagara);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USMGameplayAbility_Launch::ServerRPCRequestProjectile_Implementation(const FVector_NetQuantize10& StartLocation, const FVector_NetQuantize10& CursorLocation)
{
	const FVector Direction = (CursorLocation - StartLocation).GetSafeNormal();
	const FRotator StartRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	USMAbilityTask_SpawnAndLaunchProjectile* ProjectileTask = USMAbilityTask_SpawnAndLaunchProjectile::CreateTask(this, ProjectileClass, StartLocation, StartRotation);
	if (ProjectileTask)
	{
		ProjectileTask->ReadyForActivation();
	}
}
