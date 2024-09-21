// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_PianoNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPianoCharacter.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPianoCharacterDataAsset.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "Games/SMGameState.h"
#include "HoldInteraction/SMHIC_Character.h"
#include "Tiles/SMTile.h"
#include "Tiles/SMTileManagerComponent.h"
#include "Utilities/SMCollision.h"

USMGA_PianoNoiseBreak::USMGA_PianoNoiseBreak()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	if (FSMCharacterNoiseBreakData* NoiseBreakData = GetNoiseBreakData(ESMCharacterType::Piano))
	{
		Damage = NoiseBreakData->Damage;
		MaxDistanceByTile = NoiseBreakData->DistanceByTile;
		CaptureSize = NoiseBreakData->CaptureSize;
	}
}

void USMGA_PianoNoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPianoCharacter* SourceCharacter = GetAvatarActor<ASMPianoCharacter>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	const USMPianoCharacterDataAsset* SourceDataAsset = GetDataAsset<USMPianoCharacterDataAsset>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
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
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::PianoNoiseBreakShoot, nullptr, true);
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
		const float Offset = 250.0f;
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
		USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (TargetHIC)
		{
			TargetHIC->OnNoiseBreakActionStarted(SourceCharacter);
		}
	}
}

void USMGA_PianoNoiseBreak::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASMPianoCharacter* SourceCharacter = GetAvatarActor<ASMPianoCharacter>();
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
		USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (TargetHIC)
		{
			TSharedRef<FSMNoiseBreakData> NoiseBreakData = MakeShared<FSMNoiseBreakData>();
			NoiseBreakData->NoiseBreakLocation = NoiseBreakTargetLocation;
			TargetHIC->OnNoiseBreakActionPerformed(SourceCharacter, NoiseBreakData);
		}

		TileCapture();
		ApplySplash(NoiseBreakTargetLocation, SMTags::GameplayCue::Piano::NoiseBreakBurstHit);

		SourceHIC->SetActorIAmHolding(nullptr);

		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector TargetToSourceDirection = (SourceLocation - NoiseBreakTargetLocation).GetSafeNormal();
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Location = NoiseBreakTargetLocation;
		GCParams.Normal = TargetToSourceDirection;
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Piano::NoiseBreakBurst, GCParams);
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
	ASMPianoCharacter* SourceCharacter = GetAvatarActor<ASMPianoCharacter>();
	ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>();
	USMTileManagerComponent* TileManager = GameState ? GameState->GetTileManager() : nullptr;
	if (!SourceCharacter || !ensureAlways(TileManager))
	{
		return;
	}

	ASMTile* Tile = GetTileFromLocation(NoiseBreakTargetLocation);
	if (Tile)
	{
		const ESMTeam SourceTeam = SourceCharacter->GetTeam();
		TileManager->TileCaptureImmediateSqaure(Tile, SourceTeam, CaptureSize);
	}
}
