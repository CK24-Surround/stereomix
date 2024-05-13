// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Smash.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraSystem.h"
#include "Tiles/SMTile.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "AbilitySystem/SMTags.h"
#include "Components/SMCatchInteractionComponent_Character.h"
#include "Data/SMSpecialAction.h"
#include "FunctionLibraries/SMCatchInteractionBlueprintLibrary.h"

USMGameplayAbility_Smash::USMGameplayAbility_Smash()
{
	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Smashing);
	ActivationRequiredTags = FGameplayTagContainer(SMTags::Character::State::Catch);
}

void USMGameplayAbility_Smash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMCatchInteractionComponent_Character* SourceCIC = Cast<USMCatchInteractionComponent_Character>(SourceCharacter->GetCatchInteractionComponent());
	if (!ensureAlways(SourceCIC))
	{
		EndAbilityByCancel();
		return;
	}

	AActor* TargetActor = SourceCIC->GetActorIAmCatching();
	USMCatchInteractionComponent* TargetICI = nullptr;
	if (TargetActor)
	{
		TargetICI = Cast<USMCatchInteractionComponent>(USMCatchInteractionBlueprintLibrary::GetCatchInteractionComponent(TargetActor));
		if (!ensureAlways(TargetICI))
		{
			EndAbilityByCancel();
			return;
		}
	}

	// 스매시는 착지 시 판정이 발생하고 이에 대한 처리가 수행되어야합니다. 착지 델리게이트를 기다립니다.
	SourceCharacter->OnLanded.AddUObject(this, &ThisClass::OnSmash);

	// 스매시 애니메이션을 재생합니다.
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SmashMontage"), SmashMontage);
	if (!ensureAlways(PlayMontageAndWaitTask))
	{
		EndAbilityByCancel();
		return;
	}
	PlayMontageAndWaitTask->ReadyForActivation();

	// 타겟 측에 스매시가 시작됨을 알립니다.
	if (ActorInfo->IsNetAuthority())
	{
		TargetICI->OnSpecialActionPerformed(SourceCharacter, ESpecialAction::Smash);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	// 스매시 중 필요한 중력 값을 적용합니다. 착지 후 복구해줘야합니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensure(SourceMovement))
	{
		OriginalGravityScale = SourceMovement->GravityScale;
		SourceMovement->GravityScale = SmashGravityScale;
	}

	if (ActorInfo->IsLocallyControlled())
	{
		// 시작점과 목표 지점의 위치를 구합니다.
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector TargetLocation = CalculateMaxDistanceLocation(SourceLocation, SourceCharacter->GetCursorTargetingPoint(true));

		// 서버로 타겟 위치를전송합니다.
		SendToServerLocationData(SourceLocation, TargetLocation);

		// 정점 높이를 기준으로 타겟을 향하는 벨로시티를 구한 후 캐릭터를 도약 시킵니다.
		LaunchCharacterToTargetWithApex(SourceLocation, TargetLocation, SourceMovement->GetGravityZ());
	}

	if (ActorInfo->IsNetAuthority())
	{
		// 서버는 데이터를 수신을 준비합니다. 수신 후 알아서 바인드 해제됩니다.
		FAbilityTargetDataSetDelegate& TargetDataSetDelegate = SourceASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey());
		TargetDataSetDelegate.AddUObject(this, &ThisClass::OnReceiveTargetData);
	}
}

void USMGameplayAbility_Smash::OnReceiveTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!ensureAlways(SourceMovement))
	{
		EndAbilityByCancel();
		return;
	}

	const FGameplayAbilityTargetData* TargetData = GameplayAbilityTargetDataHandle.Get(0);
	if (!ensureAlways(TargetData))
	{
		EndAbilityByCancel();
		return;
	}

	// 서버에서도 클라이언트와 동일한 방향으로 캐릭터를 도약시킵니다.
	const FVector SourceLocation = TargetData->GetOrigin().GetLocation();
	const FVector TargetLocation = TargetData->GetEndPoint();
	LaunchCharacterToTargetWithApex(SourceLocation, TargetLocation, SourceMovement->GetGravityZ());
}

void USMGameplayAbility_Smash::OnSmash()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
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

	USMCatchInteractionComponent_Character* SourceCIC = Cast<USMCatchInteractionComponent_Character>(SourceCharacter->GetCatchInteractionComponent());
	if (!ensureAlways(SourceCIC))
	{
		EndAbilityByCancel();
		return;
	}

	AActor* TargetActor = SourceCIC->GetActorIAmCatching();
	USMCatchInteractionComponent* TargetCIC = nullptr;
	if (TargetActor)
	{
		TargetCIC = Cast<USMCatchInteractionComponent>(USMCatchInteractionBlueprintLibrary::GetCatchInteractionComponent(TargetActor));
		if (!ensureAlways(TargetCIC))
		{
			EndAbilityByCancel();
			return;
		}
	}

	// 착지 완료 했으니 델리게이트는 제거해줍니다.
	SourceCharacter->OnLanded.RemoveAll(this);

	// 기존 중력 스케일로 재설정합니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensureAlways(SourceMovement))
	{
		SourceMovement->GravityScale = OriginalGravityScale;
	}

	// 스매시 종료 애니메이션을 재생합니다. 이 애니메이션이 종료되면 스매시가 종료됩니다. 종료는 서버에서 수행하게 됩니다.
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SmashEndMontage"), SmashMontage, 1.0f, TEXT("End"));
	if (!ensureAlways(PlayMontageAndWaitTask))
	{
		EndAbilityByCancel();
		return;
	}
	if (CurrentActorInfo->IsNetAuthority())
	{
		PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	}
	PlayMontageAndWaitTask->ReadyForActivation();

	if (CurrentActorInfo->IsNetAuthority())
	{
		TargetCIC->OnSpecialActionEnded(SourceCharacter, ESpecialAction::Smash, MaxTriggerCount, DamageGE, SmashDamage);

		SourceCIC->SetActorIAmCatching(nullptr);
	}
}

FVector USMGameplayAbility_Smash::CalculateMaxDistanceLocation(const FVector& InStartLocation, const FVector& InTargetLocation)
{
	// 시작 위치의 Z값을 타겟의 Z값으로 정렬합니다. 타겟의 Z는 바닥과 동일한 높이이기 때문입니다. 이렇게하면 소스 위치가 소스의 중심에서 소스의 바닥으로 재정렬됩니다.
	FVector AlignedSourceZ = InStartLocation;
	AlignedSourceZ.Z = InTargetLocation.Z;

	// 사거리를 제한 합니다.
	// TODO: 임시로 150으로 타일 사이즈를 하드코딩 해두었습니다.
	const float TileSize = 150.0f;
	const float Range = TileSize * SmashRangeByTile;
	if (FVector::DistSquared(AlignedSourceZ, InTargetLocation) > FMath::Square(Range))
	{
		const FVector SourceToTargetDirection = (InTargetLocation - AlignedSourceZ).GetSafeNormal();

		const FVector Result = AlignedSourceZ + (SourceToTargetDirection * Range);
		return Result;
	}

	return InTargetLocation;
}

void USMGameplayAbility_Smash::SendToServerLocationData(const FVector& InStartLocation, const FVector& InTargetLocation)
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// 필요한 위치 데이터들을 타겟 데이터에 저장합니다.
	FGameplayAbilityTargetData_LocationInfo* TargetData = new FGameplayAbilityTargetData_LocationInfo();
	TargetData->SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	TargetData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	TargetData->SourceLocation.LiteralTransform.SetLocation(InStartLocation);
	TargetData->TargetLocation.LiteralTransform.SetLocation(InTargetLocation);

	// 타겟 데이터를 핸들에 담고 서버로 전송합니다.
	FGameplayAbilityTargetDataHandle TargetDatahandle(TargetData);
	SourceASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), TargetDatahandle, FGameplayTag(), SourceASC->ScopedPredictionKey);
}

void USMGameplayAbility_Smash::LaunchCharacterToTargetWithApex(const FVector& InStartLocation, const FVector& InTargetLocation, float InGravityZ)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(SourceCharacter, InStartLocation, InTargetLocation, ApexHeight, InGravityZ);
	SourceCharacter->LaunchCharacter(LaunchVelocity, true, true);
}
