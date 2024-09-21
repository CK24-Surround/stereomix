// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ElectricGuitarNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_ModifyGravityUntilLanded.h"
#include "Characters/Player/SMElectricGuitarCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "Games/SMGameState.h"
#include "HoldInteraction/SMHIC_Character.h"
#include "Tiles/SMTile.h"
#include "Tiles/SMTileManagerComponent.h"
#include "Utilities/SMCollision.h"

USMGA_ElectricGuitarNoiseBreak::USMGA_ElectricGuitarNoiseBreak()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	if (FSMCharacterNoiseBreakData* NoiseBreakData = GetNoiseBreakData(ESMCharacterType::ElectricGuitar))
	{
		Damage = NoiseBreakData->Damage;
		MaxDistanceByTile = NoiseBreakData->DistanceByTile;
		CaptureSize = NoiseBreakData->CaptureSize;
		NoiseBreakGravityScale = NoiseBreakData->GravityScale;
		ApexHeight = NoiseBreakData->ApexHeight;
	}
}

void USMGA_ElectricGuitarNoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	USMAbilitySystemComponent* SourceASC = GetASC();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	UCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceHIC || !SourceDataAsset || !SourceCapsule || !SourceMovement)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	// 노이즈 브레이크 콜라이더로 변경합니다.
	OriginalCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::NoiseBreak);

	// 노이즈 브레이크에 사용할 중력으로 변경하고 착지 시 호출할 함수를 지정합니다.
	USMAT_ModifyGravityUntilLanded* LandedTask = USMAT_ModifyGravityUntilLanded::ModifyGravityUntilLanded(this, true, NoiseBreakGravityScale);
	LandedTask->OnLanded.BindUObject(this, &ThisClass::OnLanded);
	LandedTask->ReadyForActivation();

	// 노이즈 브레이크 몽타주를 재생합니다.
	// TODO: 나중에 착지시 노브 종료로 바꿀 것.
	UAnimMontage* NoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[SourceCharacter->GetTeam()];
	const FName MontageTaskName = TEXT("MontageTask");
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, NoiseBreakMontage);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->ReadyForActivation();

	// 현재 위치의 타일을 점령하고 타겟위치로 도약합니다.
	if (IsLocallyControlled())
	{
		FVector TargetLocation;
		const float MaxDistance = MaxDistanceByTile * 150.0f;
		SourceCharacter->GetTileLocationFromCursor(TargetLocation, MaxDistance);

		const FVector SourceLocation = SourceCharacter->GetActorLocation();

		DrawDebugSphere(GetWorld(), TargetLocation, 100.0f, 16, FColor::Red, false, 5.0f);

		ServerSendLocationData(SourceLocation, TargetLocation);
		LeapCharacter(SourceLocation, TargetLocation, SourceMovement->GetGravityZ());

		ASMTile* Tile = GetTileFromLocation(SourceLocation);
		const FVector TileLocation = Tile ? Tile->GetTileLocation() : FVector::ZeroVector;
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Location = TileLocation;
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::NoiseBreakBurst, GCParams);
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

void USMGA_ElectricGuitarNoiseBreak::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (SourceCapsule)
	{
		SourceCapsule->SetCollisionProfileName(OriginalCollisionProfileName);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_ElectricGuitarNoiseBreak::ServerSendLocationData_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation)
{
	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	UCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement() : nullptr;
	if (!SourceCharacter || !ensureAlways(SourceMovement))
	{
		EndAbilityByCancel();
		return;
	}

	LeapCharacter(SourceLocation, TargetLocation, SourceMovement->GetGravityZ());

	// 바닥에 맞게 높이를 보정합니다.
	const FVector SourceLocationWithTargetZ(SourceLocation.X, SourceLocation.Y, TargetLocation.Z);
	OnBurst(SourceLocationWithTargetZ);
}

void USMGA_ElectricGuitarNoiseBreak::LeapCharacter(const FVector& InStartLocation, const FVector& InTargetLocation, float InGravityZ)
{
	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	// 시작위치와 동일한 높이로 보정합니다.
	const FVector TargetLocationWithSourceZ(InTargetLocation.X, InTargetLocation.Y, InStartLocation.Z);
	const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(SourceCharacter, InStartLocation, TargetLocationWithSourceZ, ApexHeight, InGravityZ);
	SourceCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

void USMGA_ElectricGuitarNoiseBreak::OnBurst(const FVector& TargetLocation)
{
	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceCharacter || !ensureAlways(SourceHIC))
	{
		K2_EndAbility();
		return;
	}

	AActor* TargetActor = SourceHIC->GetActorIAmHolding();
	USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
	if (TargetHIC)
	{
		TSharedRef<FSMNoiseBreakData> NoiseBreakData = MakeShared<FSMNoiseBreakData>();
		NoiseBreakData->NoiseBreakLocation = TargetLocation;
		TargetHIC->OnNoiseBreakActionPerformed(SourceCharacter, NoiseBreakData);
	}

	TileCapture(TargetLocation);
	ApplySplash(TargetLocation, SMTags::GameplayCue::ElectricGuitar::NoiseBreakBurstHit);

	SourceHIC->SetActorIAmHolding(nullptr);
}

void USMGA_ElectricGuitarNoiseBreak::OnLanded()
{
	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
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

void USMGA_ElectricGuitarNoiseBreak::TileCapture(const FVector& TargetLocation)
{
	ASMElectricGuitarCharacter* SourceCharacter = GetAvatarActor<ASMElectricGuitarCharacter>();
	ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>();
	USMTileManagerComponent* TileManager = GameState ? GameState->GetTileManager() : nullptr;
	if (!SourceCharacter || !ensureAlways(TileManager))
	{
		return;
	}

	ASMTile* Tile = GetTileFromLocation(TargetLocation);
	if (Tile)
	{
		const ESMTeam SourceTeam = SourceCharacter->GetTeam();
		TileManager->TileCaptureImmediateSqaure(Tile, SourceTeam, CaptureSize);
	}
}
