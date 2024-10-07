// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_PianoNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPianoCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/Character/SMHIC_Character.h"
#include "Data/Character/SMPianoCharacterDataAsset.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Utilities/SMCollision.h"

USMGA_PianoNoiseBreak::USMGA_PianoNoiseBreak()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	if (const FSMCharacterNoiseBreakData* NoiseBreakData = USMDataTableFunctionLibrary::GetCharacterNoiseBreakData(ESMCharacterType::Piano))
	{
		Damage = NoiseBreakData->Damage;
		MaxDistanceByTile = NoiseBreakData->DistanceByTile;
		CaptureSize = NoiseBreakData->CaptureSize;
	}
}

void USMGA_PianoNoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	const USMPianoCharacterDataAsset* SourceDataAsset = GetDataAsset<USMPianoCharacterDataAsset>();
	const USMHIC_Character* SourceHIC = GetHIC();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	UCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement() : nullptr;
	if (!SourceCharacter || !SourceDataAsset || !SourceHIC || !SourceCapsule || !SourceMovement)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	// 노이즈 브레이크 콜라이더로 변경합니다.
	OriginalCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::NoiseBreak);

	// Z축 노이즈 브레이크를 적용하기위해 잠시 Fly로 바꿉니다.
	SourceMovement->SetMovementMode(MOVE_Flying);

	// 발사 노티파이를 기다립니다.
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Piano::PianoNoiseBreakShoot, nullptr, true);
	EventTask->EventReceived.AddDynamic(this, &ThisClass::OnShoot);
	EventTask->ReadyForActivation();

	// 몽타주를 재생합니다.
	const FName MontageTaskName = TEXT("MontageTask");
	UAnimMontage* NoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, NoiseBreakMontage, 1.0f, NAME_None, false);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnNoiseBreakEnded);
	MontageTask->ReadyForActivation();

	// 타겟 위치를 조준합니다.
	if (IsLocallyControlled())
	{
		const float MaxDistance = MaxDistanceByTile * 150.0f;
		SourceCharacter->GetTileLocationFromCursor(NoiseBreakTargetLocation, MaxDistance);

		ServerSendLocationData(NoiseBreakTargetLocation);

		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector LaunchLocation(SourceLocation.X, SourceLocation.Y, SourceLocation.Z + 325.0);
		const FVector LaunchPointToTargetDirection = (NoiseBreakTargetLocation - LaunchLocation).GetSafeNormal();
		constexpr float Offset = 250.0f;
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Location = LaunchLocation + (LaunchPointToTargetDirection * Offset);
		GCParams.Normal = LaunchPointToTargetDirection;
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Piano::NoiseBreak, GCParams);
	}

	// 노이즈 브레이크 시작을 타겟에게 알립니다.
	if (K2_HasAuthority())
	{
		AActor* TargetActor = SourceHIC->GetActorIAmHolding();
		if (USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor))
		{
			TargetHIC->OnNoiseBreakStarted(SourceCharacter);
		}
	}
}

void USMGA_PianoNoiseBreak::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	const ASMPianoCharacter* SourceCharacter = GetAvatarActor<ASMPianoCharacter>();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	UCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement() : nullptr;
	if (SourceCapsule)
	{
		SourceCapsule->SetCollisionProfileName(OriginalCollisionProfileName);
	}

	if (SourceMovement)
	{
		// 무브먼트 모드를 복구합니다.
		SourceMovement->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_PianoNoiseBreak::ServerSendLocationData_Implementation(const FVector_NetQuantize10& TargetLocation)
{
	NoiseBreakTargetLocation = TargetLocation;
}

void USMGA_PianoNoiseBreak::OnShoot(FGameplayEventData Payload)
{
	ASMPianoCharacter* SourceCharacter = GetAvatarActor<ASMPianoCharacter>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceCharacter || !SourceASC || !SourceHIC)
	{
		K2_EndAbility();
		return;
	}

	if (K2_HasAuthority())
	{
		AActor* TargetActor = SourceHIC->GetActorIAmHolding();
		if (USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor))
		{
			const TSharedRef<FSMNoiseBreakData> NoiseBreakData = MakeShared<FSMNoiseBreakData>();
			NoiseBreakData->NoiseBreakLocation = NoiseBreakTargetLocation;
			TargetHIC->OnNoiseBreakApplied(SourceCharacter, NoiseBreakData);
		}

		TileCapture();
		PerformBurstAttack(NoiseBreakTargetLocation, SMTags::GameplayCue::Piano::NoiseBreakBurstHit);

		const APawn* TargetPawn = Cast<APawn>(TargetActor);
		APlayerController* TargetPlayerController = TargetPawn ? TargetPawn->GetController<APlayerController>() : nullptr;
		APlayerController* PlayerController = SourceCharacter ? SourceCharacter->GetController<APlayerController>() : nullptr;
		const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
		if (PlayerController && SourceDataAsset)
		{
			PlayerController->ClientStartCameraShake(SourceDataAsset->NoiseBreakCameraShake);

			if (TargetPlayerController)
			{
				TargetPlayerController->ClientStartCameraShake(SourceDataAsset->NoiseBreakCameraShake);
			}
		}

		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector TargetToSourceDirection = (SourceLocation - NoiseBreakTargetLocation).GetSafeNormal();
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Location = NoiseBreakTargetLocation;
		GCParams.Normal = TargetToSourceDirection;
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Piano::NoiseBreakBurst, GCParams);

		SourceHIC->SetActorIAmHolding(nullptr);
	}
}

void USMGA_PianoNoiseBreak::OnNoiseBreakEnded()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}

void USMGA_PianoNoiseBreak::TileCapture()
{
	const ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const ESMTeam SourceTeam = SourceCharacter ? SourceCharacter->GetTeam() : ESMTeam::None;
	if (!SourceCharacter || SourceTeam == ESMTeam::None)
	{
		return;
	}

	USMTileFunctionLibrary::CaptureTilesInSquare(GetWorld(), NoiseBreakTargetLocation, SourceCharacter, CaptureSize);
}
