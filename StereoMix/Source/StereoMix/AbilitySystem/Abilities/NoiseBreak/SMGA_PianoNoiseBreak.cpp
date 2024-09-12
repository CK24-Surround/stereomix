// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_PianoNoiseBreak.h"

#include "GameFramework/RootMotionSource.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPianoCharacter.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPianoCharacterDataAsset.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Games/SMGameState.h"
#include "HoldInteraction/SMHIC_Character.h"
#include "Tiles/SMTile.h"
#include "Tiles/SMTileManagerComponent.h"
#include "Utilities/SMCollision.h"

USMGA_PianoNoiseBreak::USMGA_PianoNoiseBreak()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	Damage = 20.0f;
	MaxDistanceByTile = 10;
}

void USMGA_PianoNoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPianoCharacter* SourceCharacter = GetAvatarActor<ASMPianoCharacter>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	const USMPianoCharacterDataAsset* SourceDataAsset = GetDataAsset<USMPianoCharacterDataAsset>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	if (!SourceDataAsset || !SourceHIC || !SourceCapsule)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	OriginalCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::NoiseBreak);

	const FName RootMotionTaskName = TEXT("RootMotionTask");
	const FVector Direction = FVector::UpVector;
	UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, RootMotionTaskName, Direction, 300.0f, 2.0f, false, SourceDataAsset->NoiseBreakCurve, ERootMotionFinishVelocityMode::SetVelocity, FVector::ZeroVector, 0.0f, false);
	RootMotionTask->OnFinish.AddDynamic(this, &ThisClass::OnJumpEnded);
	RootMotionTask->ReadyForActivation();
	NET_LOG(GetAvatarActor(), Warning, TEXT("루트 이동 시작"));

	const FName MontageTaskName = TEXT("MontageTask");
	UAnimMontage* NoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, NoiseBreakMontage, 1.0f, NAME_None, false);
	MontageTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		const FVector CursorLocation = SourceCharacter->GetLocationFromCursor(true);
		FVector SourceLocation = SourceCharacter->GetActorLocation();
		SourceLocation.Z = CursorLocation.Z;

		const FVector SourceToCursorVector = CursorLocation - SourceLocation;
		const float MaxDistance = MaxDistanceByTile * 150.0f;

		FVector TargetLocation = CursorLocation;
		if (SourceToCursorVector.SizeSquared() >= FMath::Square(MaxDistance))
		{
			const FVector TargetDirection = SourceToCursorVector.GetSafeNormal2D();
			TargetLocation = SourceLocation + (TargetDirection * MaxDistance);
		}

		ServerRPCSendTargetLocation(TargetLocation);
	}

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::PianoNoiseBreakShoot, nullptr, true);
		EventTask->EventReceived.AddDynamic(this, &ThisClass::OnShoot);
		EventTask->ReadyForActivation();

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
	if (SourceCharacter)
	{
		UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
		if (SourceCapsule)
		{
			SourceCapsule->SetCollisionProfileName(OriginalCollisionProfileName);
		}
	}

	bHasTargetLocation = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_PianoNoiseBreak::ServerRPCSendTargetLocation_Implementation(const FVector_NetQuantize10& TargetLocation)
{
	bHasTargetLocation = true;
	NoiseBreakTargetLocation = TargetLocation;
}

void USMGA_PianoNoiseBreak::OnShoot(FGameplayEventData Payload)
{
	if (!bHasTargetLocation)
	{
		K2_EndAbility();
		return;
	}

	ASMPianoCharacter* SourceCharacter = GetAvatarActor<ASMPianoCharacter>();
	if (!SourceCharacter)
	{
		K2_EndAbility();
		return;
	}

	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceHIC)
	{
		K2_EndAbility();
		return;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(SourceHIC->GetActorIAmHolding());
	if (TargetCharacter)
	{
		FVector Offset;
		UCapsuleComponent* TargetCapsule = TargetCharacter->GetCapsuleComponent();
		if (TargetCapsule)
		{
			Offset.Z = TargetCapsule->GetScaledCapsuleHalfHeight();
		}

		TargetCharacter->MulitcastRPCSetLocation(NoiseBreakTargetLocation + Offset);
	}

	USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetCharacter);
	if (TargetHIC)
	{
		TSharedRef<FSMNoiseBreakData> TempData = MakeShared<FSMNoiseBreakData>();
		TargetHIC->OnNoiseBreakActionPerformed(SourceCharacter, TempData);
	}

	TileCapture();
	ApplySplash(NoiseBreakTargetLocation);

	SourceHIC->SetActorIAmHolding(nullptr);

	NET_LOG(GetAvatarActor(), Warning, TEXT("발사"));
	DrawDebugSphere(GetWorld(), NoiseBreakTargetLocation, CaptureSize * 75.0f, 16, FColor::Red, false, 1.0f);
}

void USMGA_PianoNoiseBreak::OnJumpEnded()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}

void USMGA_PianoNoiseBreak::TileCapture()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return;
	}

	ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>();
	if (!GameState)
	{
		return;
	}

	USMTileManagerComponent* TileManager = GameState->GetTileManager();
	if (!TileManager)
	{
		return;
	}

	FHitResult HitResult;
	const FVector StartLocation = NoiseBreakTargetLocation;
	const FVector EndLocation = StartLocation + FVector::DownVector * 1000.0;
	const bool bSuccess = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction);

	if (bSuccess)
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (Tile)
		{
			TileManager->TileCaptureImmediateSqaure(Tile, SourceCharacter->GetTeam(), CaptureSize);
		}
	}
}
