// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ElectricGuitarNoiseBreak.h"


#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMElectricGuitarCharacter.h"
#include "Actors/Tiles/SMTile.h"
#include "Components/CapsuleComponent.h"
#include "Components/Character/SMHIC_Character.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Utilities/SMCollision.h"

USMGA_ElectricGuitarNoiseBreak::USMGA_ElectricGuitarNoiseBreak()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	if (FSMCharacterNoiseBreakData* NoiseBreakData = USMDataTableFunctionLibrary::GetCharacterNoiseBreakData(ESMCharacterType::ElectricGuitar))
	{
		Damage = NoiseBreakData->Damage;
		MaxDistanceByTile = NoiseBreakData->DistanceByTile;
		CaptureSize = NoiseBreakData->CaptureSize;
	}
}

void USMGA_ElectricGuitarNoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	USMHIC_Character* SourceHIC = GetHIC();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter || !SourceDataAsset || !SourceCapsule)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	// 노이즈 브레이크 콜라이더로 변경합니다.
	OriginalCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::NoiseBreak);

	// 노이즈 브레이크 몽타주를 재생합니다.
	UAnimMontage* NoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[SourceCharacter->GetTeam()];
	const FName MontageTaskName = TEXT("MontageTask");
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, NoiseBreakMontage, 1.0f, NAME_None, false);
	MontageTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		ASMTile* StartTile = GetTileFromLocation(SourceCharacter->GetActorLocation());
		if (!StartTile)
		{
			K2_EndAbility();
			return;
		}

		UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::ElectricGuitar::NoiseBreakFlash);
		WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnReceivedFlashEvent);
		WaitEventTask->ReadyForActivation();

		NoiseBreakStartLocation = StartTile->GetTileLocation();
		const float MaxDistance = MaxDistanceByTile * USMTileFunctionLibrary::DefaultTileSize;
		SourceCharacter->GetTileLocationFromCursor(NoiseBreakTargetLocation, MaxDistance);

		if (!K2_HasAuthority())
		{
			SyncPointNoiseBreakEnded();
		}

		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector SourceToTarget = NoiseBreakTargetLocation - SourceLocation;
		const FVector SourceToTargetDirection = SourceToTarget.GetSafeNormal();

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Location = SourceLocation;
		GCParams.Normal = SourceToTargetDirection;
		GCParams.RawMagnitude = SourceToTarget.Size();
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::NoiseBreakFlash, GCParams);
	}

	// 노이즈 브레이크 시작을 타겟에게 알립니다.
	if (K2_HasAuthority())
	{
		AActor* TargetActor = SourceHIC->GetActorIAmHolding();
		USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (TargetHIC)
		{
			TargetHIC->OnNoiseBreakStarted(SourceCharacter);
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

void USMGA_ElectricGuitarNoiseBreak::OnReceivedFlashEvent(FGameplayEventData Payload)
{
	ServerSendTargetLocation(NoiseBreakStartLocation, NoiseBreakTargetLocation);
}

void USMGA_ElectricGuitarNoiseBreak::ServerSendTargetLocation_Implementation(const FVector_NetQuantize10& StartLocation, const FVector_NetQuantize10& TargetLocation)
{
	NoiseBreakStartLocation = StartLocation;
	NoiseBreakTargetLocation = TargetLocation;

	OnFlash();
}

void USMGA_ElectricGuitarNoiseBreak::OnFlash()
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter || !SourceCapsule)
	{
		K2_EndAbility();
		return;
	}

	FVector NoiseBreakTargetLocationWithSourceZ;
	NoiseBreakTargetLocationWithSourceZ.X = NoiseBreakTargetLocation.X;
	NoiseBreakTargetLocationWithSourceZ.Y = NoiseBreakTargetLocation.Y;
	NoiseBreakTargetLocationWithSourceZ.Z = NoiseBreakStartLocation.Z + SourceCapsule->GetScaledCapsuleHalfHeight();
	SourceCharacter->MulticastRPCSetLocation(NoiseBreakTargetLocationWithSourceZ);

	UAbilityTask_WaitDelay* WaitBurstTask = UAbilityTask_WaitDelay::WaitDelay(this, BurstDelayAfterFlash);
	WaitBurstTask->OnFinish.AddDynamic(this, &ThisClass::OnNoiseBreakBurst);
	WaitBurstTask->ReadyForActivation();

	UAbilityTask_WaitDelay* WaitEndTask = UAbilityTask_WaitDelay::WaitDelay(this, EndDelayAfterFlash);
	WaitEndTask->OnFinish.AddDynamic(this, &ThisClass::SyncPointNoiseBreakEnded);
	WaitEndTask->ReadyForActivation();
}

void USMGA_ElectricGuitarNoiseBreak::OnNoiseBreakBurst()
{
	ASMElectricGuitarCharacter* SourceCharacter = GetCharacter<ASMElectricGuitarCharacter>();
	USMAbilitySystemComponent* SourceASC = GetASC();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceCharacter || !SourceASC || !SourceHIC)
	{
		K2_EndAbility();
		return;
	}

	AActor* TargetActor = SourceHIC->GetActorIAmHolding();
	USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
	if (TargetHIC)
	{
		const FVector TargetToStartDirection = (NoiseBreakStartLocation - NoiseBreakTargetLocation).GetSafeNormal();
		TSharedRef<FSMNoiseBreakData> NoiseBreakData = MakeShared<FSMNoiseBreakData>();
		NoiseBreakData->NoiseBreakLocation = NoiseBreakTargetLocation + (TargetToStartDirection * 70.0f);
		TargetHIC->OnNoiseBreakApplied(SourceCharacter, NoiseBreakData);
	}

	TileCapture();
	PerformElectricGuitarBurstAttack();

	SourceHIC->SetActorIAmHolding(nullptr);

	const FVector StartToTarget = NoiseBreakTargetLocation - NoiseBreakStartLocation;

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	GCParams.Location = NoiseBreakStartLocation;
	GCParams.Normal = StartToTarget.GetSafeNormal();
	GCParams.RawMagnitude = StartToTarget.Size();
	SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::NoiseBreakBurst, GCParams);
}

void USMGA_ElectricGuitarNoiseBreak::TileCapture()
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	UWorld* World = GetWorld();
	if (!SourceCharacter || !World)
	{
		return;
	}

	// 캡슐을 통해 시작 지점에서 끝지점까지 점령할 타일을 저장합니다.
	TArray<ASMTile*> CaptureTiles = USMTileFunctionLibrary::GetTilesFromLocationByCapsule(World, NoiseBreakStartLocation, NoiseBreakTargetLocation, USMTileFunctionLibrary::DefaultTileSize);

	// 시작과 종료 지점 앞 뒤 타일을 제거합니다.
	const FVector CachedNoiseBreakStartLocation = NoiseBreakStartLocation;
	const FVector CachedNoiseBreakTargetLocation = NoiseBreakTargetLocation;
	const FVector StartToTargetDirection = (NoiseBreakTargetLocation - NoiseBreakStartLocation).GetSafeNormal();
	CaptureTiles.RemoveAll([CachedNoiseBreakStartLocation, CachedNoiseBreakTargetLocation, StartToTargetDirection](ASMTile* Tile) {
		const FVector TileLocation = Tile->GetTileLocation();

		const FVector StartToTileDirection = (TileLocation - CachedNoiseBreakStartLocation).GetSafeNormal();
		const FVector EndToTileDirection = (TileLocation - CachedNoiseBreakTargetLocation).GetSafeNormal();
		if (StartToTileDirection.IsNearlyZero() || EndToTileDirection.IsNearlyZero())
		{
			return false;
		}

		float DotProductFromStart = FVector::DotProduct(StartToTargetDirection, StartToTileDirection);
		float DotProductFromEnd = FVector::DotProduct(StartToTargetDirection, EndToTileDirection);

		const float Degrees = 60.0f;
		const float Radians = FMath::DegreesToRadians(Degrees);
		const float Cos = FMath::Cos(Radians);
		return DotProductFromStart < -Cos || DotProductFromEnd > Cos;
	});

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	for (ASMTile* CaptureTile : CaptureTiles)
	{
		USMTileFunctionLibrary::TileCaptureImmediateSqaure(World, CaptureTile, SourceTeam, 1);
	}

	NET_LOG(GetAvatarActor(), Log, TEXT("노이즈 브레이크로 점령 시도한 타일 개수: %d"), CaptureTiles.Num());
}

void USMGA_ElectricGuitarNoiseBreak::PerformElectricGuitarBurstAttack()
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	if (!SourceCharacter || !SourceASC)
	{
		return;
	}

	const FVector NoiseBreakDirection = (NoiseBreakTargetLocation - NoiseBreakStartLocation).GetSafeNormal();

	for (AActor* BurstHitActor : GetElectricGuitarBurstHitActors())
	{
		ISMDamageInterface* BurstHitActorDamageInterface = Cast<ISMDamageInterface>(BurstHitActor);
		if (!BurstHitActorDamageInterface || BurstHitActorDamageInterface->CanIgnoreAttack())
		{
			continue;
		}

		if (USMTeamBlueprintLibrary::IsSameTeam(SourceCharacter, BurstHitActor))
		{
			continue;
		}

		BurstHitActorDamageInterface->ReceiveDamage(SourceCharacter, Damage);

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Normal = NoiseBreakDirection;
		GCParams.TargetAttachComponent = BurstHitActor->GetRootComponent();
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::NoiseBreakBurstHit, GCParams);
	}
}

TArray<AActor*> USMGA_ElectricGuitarNoiseBreak::GetElectricGuitarBurstHitActors()
{
	TArray<AActor*> Results;

	TArray<FOverlapResult> OverlapResults;
	const FVector CapsuleCenterLocation = (NoiseBreakStartLocation + NoiseBreakTargetLocation) / 2.0f;

	const FVector CapsuleDirection = (NoiseBreakTargetLocation - NoiseBreakStartLocation).GetSafeNormal();
	const FRotator CapsuleRotation = CapsuleDirection.Rotation() + FRotator(90.0, 0.0, 0.0);
	const FQuat CapsuleQuat = CapsuleRotation.Quaternion();

	const float Radius = USMTileFunctionLibrary::DefaultTileSize;
	const float CapsuleHalfHeight = (FVector::Dist(NoiseBreakStartLocation, NoiseBreakTargetLocation) / 2.0f) + Radius;
	const FCollisionShape CapsuleCollider = FCollisionShape::MakeCapsule(Radius, CapsuleHalfHeight);

	if (!GetWorld()->OverlapMultiByChannel(OverlapResults, CapsuleCenterLocation, CapsuleQuat, SMCollisionTraceChannel::Action, CapsuleCollider))
	{
		return Results;
	}

	for (FOverlapResult& OverlapResult : OverlapResults)
	{
		if (AActor* BurstHitActor = OverlapResult.GetActor())
		{
			Results.Add(BurstHitActor);
		}
	}

	return Results;
}

void USMGA_ElectricGuitarNoiseBreak::SyncPointNoiseBreakEnded()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}
