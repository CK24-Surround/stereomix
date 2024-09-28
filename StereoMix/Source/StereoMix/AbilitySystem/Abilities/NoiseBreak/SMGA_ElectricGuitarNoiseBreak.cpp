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
#include "Characters/Component/SMHIC_Character.h"
#include "Characters/Player/SMElectricGuitarCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
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

	ASMElectricGuitarCharacter* SourceCharacter = GetCharacter<ASMElectricGuitarCharacter>();
	USMAbilitySystemComponent* SourceASC = GetASC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
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
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
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

void USMGA_ElectricGuitarNoiseBreak::OnReceivedFlashEvent(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter || !SourceCapsule)
	{
		K2_EndAbility();
		return;
	}

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

	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, FlashDelayTime);
	WaitTask->OnFinish.AddDynamic(this, &ThisClass::OnNoiseBreakBurst);
	WaitTask->ReadyForActivation();
}

void USMGA_ElectricGuitarNoiseBreak::OnNoiseBreakBurst()
{
	ASMElectricGuitarCharacter* SourceCharacter = GetCharacter<ASMElectricGuitarCharacter>();
	USMAbilitySystemComponent* SourceASC = GetASC();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceCharacter || !SourceHIC)
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
		TargetHIC->OnNoiseBreakActionPerformed(SourceCharacter, NoiseBreakData);
	}

	TileCapture();
	ApplySplashForElectricGuitar();

	SourceHIC->SetActorIAmHolding(nullptr);

	const FVector StartToTarget = NoiseBreakTargetLocation - NoiseBreakStartLocation;

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	GCParams.Location = NoiseBreakStartLocation;
	GCParams.Normal = StartToTarget.GetSafeNormal();
	GCParams.RawMagnitude = StartToTarget.Size();
	SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::NoiseBreakBurst, GCParams);

	SyncPointNoiseBreakEnded();
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
	TArray<ASMTile*> CaptureTiles = USMTileFunctionLibrary::GetTilesFromLocationByCapsule(World, NoiseBreakStartLocation, NoiseBreakTargetLocation, USMTileFunctionLibrary::DefaultTileSize, true);

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

void USMGA_ElectricGuitarNoiseBreak::ApplySplashForElectricGuitar()
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceASC || !SourceDataAsset)
	{
		return;
	}

	TArray<AActor*> SplashHitActors = GetSplashHitActorsForElectricGuitar();
	const FVector NoiseBreakDirection = (NoiseBreakTargetLocation - NoiseBreakStartLocation).GetSafeNormal();

	for (AActor* SplashHitActor : SplashHitActors)
	{
		ISMDamageInterface* HitActorDamageInterface = Cast<ISMDamageInterface>(SplashHitActor);
		if (!HitActorDamageInterface)
		{
			continue;
		}

		HitActorDamageInterface->ReceiveDamage(SourceCharacter, Damage);

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Normal = NoiseBreakDirection;
		GCParams.TargetAttachComponent = SplashHitActor->GetRootComponent();
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::ElectricGuitar::NoiseBreakBurstHit, GCParams);
	}
}

TArray<AActor*> USMGA_ElectricGuitarNoiseBreak::GetSplashHitActorsForElectricGuitar()
{
	TArray<AActor*> Results;

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (!SourceCharacter)
	{
		return Results;
	}

	TArray<FOverlapResult> OverlapResults;
	const FVector CapsuleLocation = (NoiseBreakStartLocation + NoiseBreakTargetLocation) / 2.0f;
	const FVector CapsuleDirection = (NoiseBreakTargetLocation - NoiseBreakStartLocation).GetSafeNormal();
	const FRotator CapsuleRotation = CapsuleDirection.Rotation() + FRotator(90.0, 0.0, 0.0);
	const FQuat CapsuleQuat = CapsuleRotation.Quaternion();
	const float Radius = USMTileFunctionLibrary::DefaultTileSize;
	const float CapsuleHalfHeight = (FVector::Dist(NoiseBreakStartLocation, NoiseBreakTargetLocation) / 2.0f) + Radius;
	const FCollisionShape CapsuleCollider = FCollisionShape::MakeCapsule(Radius, CapsuleHalfHeight);
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, CapsuleLocation, CapsuleQuat, SMCollisionTraceChannel::Action, CapsuleCollider))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* SplashHitActor = OverlapResult.GetActor();
			if (SplashHitActor)
			{
				const ESMTeam SourceTeam = SourceCharacter->GetTeam();
				const ESMTeam TargetTeam = USMTeamBlueprintLibrary::GetTeam(SplashHitActor);
				if (SourceTeam == TargetTeam)
				{
					continue;
				}

				Results.Add(SplashHitActor);
			}
		}
	}

	return Results;
}

void USMGA_ElectricGuitarNoiseBreak::SyncPointNoiseBreakEnded()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::OnNoiseBreakEnded);
	SyncTask->ReadyForActivation();
}

void USMGA_ElectricGuitarNoiseBreak::OnNoiseBreakEnded()
{
	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);
}
