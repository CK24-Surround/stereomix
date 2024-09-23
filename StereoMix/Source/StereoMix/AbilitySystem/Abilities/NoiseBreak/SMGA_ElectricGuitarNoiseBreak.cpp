// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ElectricGuitarNoiseBreak.h"


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

	const FVector NoiseBreakTargetLocationWithSourceZ(NoiseBreakTargetLocation.X, NoiseBreakTargetLocation.Y, NoiseBreakStartLocation.Z + SourceCapsule->GetScaledCapsuleHalfHeight());
	SourceCharacter->SetActorLocation(NoiseBreakTargetLocationWithSourceZ);
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
	TileCapture();
}

void USMGA_ElectricGuitarNoiseBreak::TileCapture()
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (!SourceCharacter)
	{
		return;
	}

	TArray<ASMTile*> CaptureTiles;
	const FVector StartToTarget = NoiseBreakTargetLocation - NoiseBreakStartLocation;
	const float Distance = StartToTarget.Size();

	const FVector StartToTargetDirection = StartToTarget.GetSafeNormal();
	const float DividedDistance = Distance / DivideCount;
	for (int32 i = 0; i <= DivideCount; ++i)
	{
		const FVector TargetLocation = NoiseBreakStartLocation + (StartToTargetDirection * DividedDistance * i);
		TArray<ASMTile*> Tiles = (USMTileFunctionLibrary::GetTilesFromLocationSphere(GetWorld(), TargetLocation, USMTileFunctionLibrary::DefaultTileSize));
		for (ASMTile* Tile : Tiles)
		{
			CaptureTiles.AddUnique(Tile);
		}
	}

	// 시작과 종료 지점 앞 뒤 타일을 제거합니다.
	CaptureTiles.RemoveAll([&](ASMTile* Tile) {
		const FVector TileLocation = Tile->GetTileLocation();

		const FVector StartToTileDirection = (TileLocation - NoiseBreakStartLocation).GetSafeNormal();
		const FVector EndToTileDirection = (TileLocation - NoiseBreakTargetLocation).GetSafeNormal();
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

	// 기준점을 시작점의 뒤 1타일로 잡고 가까운 타일부터 순서대로 정렬합니다.
	const FVector NoiseBreakStartLocationWithOffset = NoiseBreakStartLocation + (StartToTargetDirection * -USMTileFunctionLibrary::DefaultTileSize);
	CaptureTiles.Sort([NoiseBreakStartLocationWithOffset](ASMTile& Lhs, ASMTile& Rhs) {
		const float LhsDistSquared = FVector::DistSquared(NoiseBreakStartLocationWithOffset, Lhs.GetTileLocation());
		const float RhsDistSquared = FVector::DistSquared(NoiseBreakStartLocationWithOffset, Rhs.GetTileLocation());
		return LhsDistSquared < RhsDistSquared;
	});

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	for (int32 i = 0; i < CaptureTiles.Num(); ++i)
	{
		TArray<ASMTile*> Tiles;
		Tiles.Add(CaptureTiles[i]);
		for (int32 j = 0; j < 2; ++j)
		{
			if (i + 1 < CaptureTiles.Num())
			{
				Tiles.Add(CaptureTiles[i + 1]);
				++i;
			}
		}

		TWeakObjectPtr<USMGA_ElectricGuitarNoiseBreak> ThisWeakPtr = MakeWeakObjectPtr(this);
		auto Lambda = [ThisWeakPtr, Tiles, SourceTeam]() {
			if (ThisWeakPtr.Get())
			{
				for (ASMTile* Tile : Tiles)
				{
					USMTileFunctionLibrary::TileCaptureImmediateSqaure(ThisWeakPtr->GetWorld(), Tile, SourceTeam, 1);
				}
			}
		};

		if (i == 0)
		{
			Lambda();
		}
		else
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, Lambda, 0.01f * i, false);
		}
	}

	NET_LOG(GetAvatarActor(), Log, TEXT("점령 시도한 타일 개수: %d"), CaptureTiles.Num());
}
