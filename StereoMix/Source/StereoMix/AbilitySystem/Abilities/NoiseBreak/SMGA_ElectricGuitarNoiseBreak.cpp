// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ElectricGuitarNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/Player/SMElectricGuitarCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "Games/SMGameState.h"
#include "HoldInteraction/SMHIC_Character.h"
#include "Tiles/SMTile.h"
#include "Tiles/SMTileManagerComponent.h"
#include "Utilities/SMCollision.h"

USMGA_ElectricGuitarNoiseBreak::USMGA_ElectricGuitarNoiseBreak()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void USMGA_ElectricGuitarNoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	if (!SourceCapsule)
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceHIC)
	{
		EndAbilityByCancel();
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!SourceMovement)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	OriginalCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	NET_LOG(SourceCharacter, Warning, TEXT("%s"), *OriginalCollisionProfileName.ToString());
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::NoiseBreak);

	SourceCharacter->OnCharacterLanded.AddUObject(this, &ThisClass::OnLanded);

	OriginalGravityScale = SourceMovement->GravityScale;
	SourceMovement->GravityScale = NoiseBreakGravityScale;

	UAnimMontage* NoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[SourceCharacter->GetTeam()];
	const FName MontageTaskName = TEXT("MontageTask");
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, NoiseBreakMontage);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		const FVector CursorLocation = SourceCharacter->GetCursorTargetingPoint(true);
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector SourceLocationWithZeroBasis = FVector(SourceLocation.X, SourceLocation.Y, CursorLocation.Z);

		const FVector SourceToCursorVector = CursorLocation - SourceLocationWithZeroBasis;
		const float MaxDistance = MaxDistanceByTile * 150.0f;

		FVector TargetLocation = CursorLocation;
		DrawDebugSphere(GetWorld(), TargetLocation, 100.0f, 16, FColor::Red, false, 5.0f);
		if (SourceToCursorVector.SizeSquared() >= FMath::Square(MaxDistance))
		{
			const FVector TargetDirection = SourceToCursorVector.GetSafeNormal2D();
			TargetLocation = SourceLocationWithZeroBasis + (TargetDirection * MaxDistance);
			DrawDebugSphere(GetWorld(), TargetLocation, 100.0f, 16, FColor::Blue, false, 5.0f);
		}

		const FVector TargetLocationWithCapsuleHeight = FVector(TargetLocation.X, TargetLocation.Y, SourceLocation.Z);
		ServerRPCSendTargetLocation(SourceLocationWithZeroBasis);
		ServerRPCLaunchCharacter(SourceLocation, TargetLocationWithCapsuleHeight);

		LaunchCharacter(SourceLocation, TargetLocationWithCapsuleHeight, SourceMovement->GetGravityZ());
	}

	if (K2_HasAuthority())
	{
		AActor* TargetActor = SourceHIC->GetActorIAmHolding();
		USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (TargetHIC)
		{
			TargetHIC->OnNoiseBreakActionStarted(SourceCharacter);
			TargetHIC->OnNoiseBreakActionPerformed(SourceCharacter, MakeShared<FSMNoiseBreakData>());
		}
	}
}

void USMGA_ElectricGuitarNoiseBreak::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_ElectricGuitarNoiseBreak::ServerRPCSendTargetLocation_Implementation(const FVector_NetQuantize10& TargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
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

		TargetCharacter->MulitcastRPCSetLocation(TargetLocation + Offset);
	}

	TileCapture(TargetLocation);
	SourceHIC->SetActorIAmHolding(nullptr);
}

void USMGA_ElectricGuitarNoiseBreak::TileCapture(const FVector& TargetLocation)
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
	const FVector StartLocation = TargetLocation;
	const FVector EndLocation = StartLocation + FVector::DownVector * 1000.0;
	const bool bSuccess = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction);

	if (bSuccess)
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (Tile)
		{
			TileManager->TileCaptureImmediateSqaure(Tile, SourceCharacter->GetTeam(), CaptureCount);
		}
	}
}

void USMGA_ElectricGuitarNoiseBreak::LaunchCharacter(const FVector& InStartLocation, const FVector& InTargetLocation, float InGravityZ)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(SourceCharacter, InStartLocation, InTargetLocation, ApexHegith, InGravityZ);
	SourceCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

void USMGA_ElectricGuitarNoiseBreak::ServerRPCLaunchCharacter_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation)
{
	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!SourceMovement)
	{
		EndAbilityByCancel();
		return;
	}

	LaunchCharacter(SourceLocation, TargetLocation, SourceMovement->GetGravityZ());
}

void USMGA_ElectricGuitarNoiseBreak::OnLanded(ASMPlayerCharacterBase* LandedCharacter)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (SourceMovement)
	{
		SourceMovement->GravityScale = OriginalGravityScale;
		SourceMovement->StopMovementImmediately();
	}

	SourceCharacter->OnCharacterLanded.RemoveAll(this);

	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	if (SourceCapsule)
	{
		SourceCapsule->SetCollisionProfileName(OriginalCollisionProfileName);
	}

	UAbilityTask_NetworkSyncPoint* NetworkSyncPoint = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	NetworkSyncPoint->OnSync.AddDynamic(this, &ThisClass::OnNoiseBreakEnded);
	NetworkSyncPoint->ReadyForActivation();
}

void USMGA_ElectricGuitarNoiseBreak::OnNoiseBreakEnded()
{
	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);
}
