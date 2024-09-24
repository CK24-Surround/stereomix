// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ElectricGuitarNoiseBreak.h"


#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMElectricGuitarCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "HoldInteraction/SMHIC_Character.h"
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
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnNoiseBreakEnded);
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

	const FVector NoiseBreakTargetLocationWithSourceZ(NoiseBreakTargetLocation.X, NoiseBreakTargetLocation.Y, NoiseBreakStartLocation.Z + SourceCapsule->GetScaledCapsuleHalfHeight());
	SourceCharacter->MulitcastRPCSetLocation(NoiseBreakTargetLocationWithSourceZ);

	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, FlashDelayTime);
	WaitTask->OnFinish.AddDynamic(this, &ThisClass::OnNoiseBreak);
	WaitTask->ReadyForActivation();
}

void USMGA_ElectricGuitarNoiseBreak::OnNoiseBreak()
{
	ASMElectricGuitarCharacter* SourceCharacter = GetCharacter<ASMElectricGuitarCharacter>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceCharacter || !SourceHIC)
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
	}

	TileCapture();
	// TODO: 스플래시 데미지는 점령시도한 타일 위를 기준으로 여러개의 콜라이더를 오버랩시키고 중복을 걸러낸 뒤 대상에게 데미지를 주도록 로직을 구성해야한다.

	SourceHIC->SetActorIAmHolding(nullptr);
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

void USMGA_ElectricGuitarNoiseBreak::OnNoiseBreakEnded()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}
