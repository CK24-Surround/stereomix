// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ElectricGuitarNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_ModifyGravityUntilLanded.h"
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

	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!SourceHIC || !SourceDataAsset || !SourceCapsule || !SourceMovement)
	{
		EndAbilityByCancel();
		return;
	}

	// 커서의 위치가 타일이 아닌 경우 시전되면 안됩니다.
	FVector TargetLocation;
	if (IsLocallyControlled())
	{
		const float MaxDistance = MaxDistanceByTile * 150.0f;
		if (!SourceCharacter->GetTileLocationFromCursor(TargetLocation, MaxDistance))
		{
			EndAbilityByCancel();
			return;
		}
	}

	K2_CommitAbility();

	// 노이즈 브레이크 콜라이더로 변경합니다.
	OriginalCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::NoiseBreak);

	// 노이즈 브레이크에 사용할 중력으로 변경하고 착지 시 호출할 함수를 지정합니다.
	USMAT_ModifyGravityUntilLanded* LandedTask = USMAT_ModifyGravityUntilLanded::ModifyGravityUntilLanded(this, true, NoiseBreakGravityScale);
	LandedTask->OnLanded.BindUObject(this, &ThisClass::OnLanded);
	LandedTask->ReadyForActivation();

	// 노이즈 브레이크 몽타주를 재생합니다. 노이즈 브레이크 몽타주가 종료되면 노이즈 브레이크도 종료됩니다.
	UAnimMontage* NoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[SourceCharacter->GetTeam()];
	const FName MontageTaskName = TEXT("MontageTask");
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, NoiseBreakMontage);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->ReadyForActivation();

	// 현재 위치의 타일을 점령하고 타겟위치로 도약합니다.
	if (IsLocallyControlled())
	{
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector SourceLocationWithTargetZ(SourceLocation.X, SourceLocation.Y, TargetLocation.Z);
		const FVector TargetLocationWithSourceZ(TargetLocation.X, TargetLocation.Y, SourceLocation.Z);

		DrawDebugSphere(GetWorld(), TargetLocation, 100.0f, 16, FColor::Red, false, 5.0f);
		ServerRPCOnNoiseBreak(SourceLocationWithTargetZ);

		ServerRPCLeapCharacter(SourceLocation, TargetLocationWithSourceZ);
		LeapCharacter(SourceLocation, TargetLocationWithSourceZ, SourceMovement->GetGravityZ());
	}

	// 노이즈 브레이크 시작을 타겟에게 알립니다.
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

void USMGA_ElectricGuitarNoiseBreak::ServerRPCOnNoiseBreak_Implementation(const FVector_NetQuantize10& TargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceCharacter || !ensureAlways(SourceHIC))
	{
		K2_EndAbility();
		return;
	}

	// 잡고 있는 캐릭터를 자신의 위치로 이동시킵니다.
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

	SourceHIC->SetActorIAmHolding(nullptr);
	TileCapture(TargetLocation);
	ApplySplash(TargetLocation);
}

void USMGA_ElectricGuitarNoiseBreak::TileCapture(const FVector& TargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>();
	USMTileManagerComponent* TileManager = GameState ? GameState->GetTileManager() : nullptr;
	if (!SourceCharacter || !ensureAlways(TileManager))
	{
		return;
	}

	FHitResult HitResult;
	const FVector StartLocation = TargetLocation;
	const FVector EndLocation = StartLocation + FVector::DownVector * 1000.0;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction))
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (Tile)
		{
			TileManager->TileCaptureImmediateSqaure(Tile, SourceCharacter->GetTeam(), CaptureSize);
		}
	}
}

void USMGA_ElectricGuitarNoiseBreak::LeapCharacter(const FVector& InStartLocation, const FVector& InTargetLocation, float InGravityZ)
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

void USMGA_ElectricGuitarNoiseBreak::ServerRPCLeapCharacter_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation)
{
	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	UCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement() : nullptr;
	if (!SourceCharacter || !ensureAlways(SourceMovement))
	{
		EndAbilityByCancel();
		return;
	}

	LeapCharacter(SourceLocation, TargetLocation, SourceMovement->GetGravityZ());
}

void USMGA_ElectricGuitarNoiseBreak::OnLanded()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter || !ensureAlways(SourceCapsule))
	{
		EndAbilityByCancel();
		return;
	}

	SourceCapsule->SetCollisionProfileName(OriginalCollisionProfileName);

	UAbilityTask_NetworkSyncPoint* NetworkSyncPoint = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	NetworkSyncPoint->OnSync.AddDynamic(this, &ThisClass::OnNoiseBreakEnded);
	NetworkSyncPoint->ReadyForActivation();
}

void USMGA_ElectricGuitarNoiseBreak::OnNoiseBreakEnded()
{
	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);
}
