// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_BassNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_ModifyGravityUntilLanded.h"
#include "Actors/Character/Player/SMBassCharacter.h"
#include "Actors/Tiles/SMTile.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/Character/SMHIC_Character.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Utilities/SMCollision.h"

USMGA_BassNoiseBreak::USMGA_BassNoiseBreak()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	if (const FSMCharacterNoiseBreakData* NoiseBreakData = USMDataTableFunctionLibrary::GetCharacterNoiseBreakData(ESMCharacterType::Bass))
	{
		Damage = NoiseBreakData->Damage;
		MaxDistanceByTile = NoiseBreakData->DistanceByTile;
		CaptureSize = NoiseBreakData->CaptureSize;
		NoiseBreakGravityScale = NoiseBreakData->GravityScale;
		ApexHeight = NoiseBreakData->ApexHeight;
	}
}

void USMGA_BassNoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	const USMHIC_Character* SourceHIC = GetHIC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	const UCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement() : nullptr;
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

	// 몽타주를 재생합니다.
	const FName MontageTaskName = TEXT("MontageTask");
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	CachedNoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, CachedNoiseBreakMontage, 1.0f, NAME_None, false);
	PlayMontageAndWaitTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		UAbilityTask_WaitGameplayEvent* WeaponTrailActivationTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Bass::NoiseBreakWeaponTrailActivate);
		WeaponTrailActivationTask->EventReceived.AddDynamic(this, &ThisClass::OnWeaponTrailActivate);
		WeaponTrailActivationTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WeaponTrailDeactivationTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Bass::NoiseBreakWeaponTrailDeactivate);
		WeaponTrailDeactivationTask->EventReceived.AddDynamic(this, &ThisClass::OnWeaponTrailDeactivate);
		WeaponTrailDeactivationTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* SlashTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Bass::NoiseBreakSlash);
		SlashTask->EventReceived.AddDynamic(this, &ThisClass::OnSlash);
		SlashTask->ReadyForActivation();

		// 타겟 위치로 도약합니다.
		NoiseBreakStartLocation = SourceCharacter->GetActorLocation();

		const float MaxDistance = MaxDistanceByTile * 150.0f;
		SourceCharacter->GetTileLocationFromCursor(NoiseBreakTargetLocation, MaxDistance);

		ServerSendLocationData(NoiseBreakStartLocation, NoiseBreakTargetLocation);
		LeapCharacter(NoiseBreakStartLocation, NoiseBreakTargetLocation, SourceMovement->GetGravityZ());

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreak, GCParams);
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

void USMGA_BassNoiseBreak::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_BassNoiseBreak::ServerSendLocationData_Implementation(const FVector_NetQuantize10& NewSourceLocation, const FVector_NetQuantize10& NewTargetLocation)
{
	const ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	const UCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement() : nullptr;
	if (!SourceCharacter || !SourceMovement)
	{
		EndAbilityByCancel();
		return;
	}

	NoiseBreakStartLocation = NewSourceLocation;
	NoiseBreakTargetLocation = NewTargetLocation;

	LeapCharacter(NewSourceLocation, NewTargetLocation, SourceMovement->GetGravityZ());
}

void USMGA_BassNoiseBreak::LeapCharacter(const FVector& InStartLocation, const FVector& InTargetLocation, float InGravityZ)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(SourceCharacter, InStartLocation, InTargetLocation, ApexHeight, InGravityZ);
	SourceCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

void USMGA_BassNoiseBreak::OnLanded()
{
	ASMBassCharacter* SourceCharacter = GetAvatarActor<ASMBassCharacter>();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	USMHIC_Character* SourceHIC = GetHIC();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceHIC || !SourceCapsule)
	{
		EndAbilityByCancel();
		return;
	}

	// 콜라이더를 복구합니다.
	SourceCapsule->SetCollisionProfileName(OriginalCollisionProfileName);

	// 착지 몽타주로으로 점프합니다.
	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);

	// 노이즈 브레이크 종료 노티파이를 기다립니다.
	// 몽타주 종료에 노이즈 브레이크 종료를 바인드 하지 않는 이유는 노이즈 브레이크 애니메이션이 끝나기 전에 움직일 수 있도록 하기 위함입니다.
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Common::NoiseBreakEnd);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnNoiseBreakEnded);
	WaitGameplayEventTask->ReadyForActivation();

	if (K2_HasAuthority())
	{
		AActor* TargetActor = SourceHIC->GetActorIAmHolding();

		if (USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor))
		{
			const TArray<ASMTile*> TilesToBeCaptured = GetTilesToBeCaptured();
			const TSharedRef<FSMNoiseBreakData> NoiseBreakData = MakeShared<FSMNoiseBreakData>();
			NoiseBreakData->TilesToBeTriggered = USMTileFunctionLibrary::ConvertToWeakPtrArray(TilesToBeCaptured);
			TargetHIC->OnNoiseBreakApplied(SourceCharacter, NoiseBreakData);

			if (TargetHIC->ShouldCaptureTilesFromNoiseBreak())
			{
				USMTileFunctionLibrary::CaptureTiles(GetWorld(), TilesToBeCaptured, SourceCharacter);
			}

			if (TargetHIC->ShouldApplyDamageFromNoiseBreak())
			{
				PerformBurstAttack(SourceCharacter->GetActorLocation(), SMTags::GameplayCue::Bass::NoiseBreakBurstHit);
			}
		}

		const APawn* TargetPawn = Cast<APawn>(TargetActor);
		APlayerController* TargetPlayerController = TargetPawn ? TargetPawn->GetController<APlayerController>() : nullptr;
		const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
		if (TargetPlayerController && SourceDataAsset)
		{
			TargetPlayerController->ClientStartCameraShake(SourceDataAsset->NoiseBreakCameraShake);
		}

		SourceHIC->SetActorIAmHolding(nullptr);
	}

	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = SourceCharacter ? SourceCharacter->GetController<APlayerController>() : nullptr;
		const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset();
		if (PlayerController && SourceDataAsset)
		{
			PlayerController->ClientStartCameraShake(SourceDataAsset->NoiseBreakCameraShake);
		}

		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const ASMTile* Tile = GetTileFromLocation(SourceLocation);
		const FVector TileLocation = Tile ? Tile->GetTileLocation() : FVector::ZeroVector;
		const FVector Offset(0.0, 0.0, 10.0); // 타일과 겹치게 되면 현재 이펙트가 잘 안 보이는데 이를 방지하는 오프셋입니다.

		const AActor* TargetActor = SourceHIC->GetActorIAmHolding();
		const float HoldingType = TargetActor ? (Cast<ASMPlayerCharacterBase>(TargetActor) ? 0.0f : 1.0f) : 0.0f; // 0이면 캐릭터, 1이면 아이템입니다.

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.Location = TileLocation + Offset;
		GCParams.RawMagnitude = HoldingType;
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakBurst, GCParams);
	}
}

void USMGA_BassNoiseBreak::OnNoiseBreakEnded(FGameplayEventData Payload)
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}

TArray<ASMTile*> USMGA_BassNoiseBreak::GetTilesToBeCaptured()
{
	const UWorld* World = GetWorld();
	const ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (!World || !SourceCharacter)
	{
		return TArray<ASMTile*>();
	}

	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	constexpr float Offset = USMTileFunctionLibrary::DefaultTileSize / 4.0f;
	const float HalfSize = Offset + (USMTileFunctionLibrary::DefaultTileSize * (CaptureSize - 1));
	const FVector BoxExtend(HalfSize);

	return USMTileFunctionLibrary::GetTilesInBox(World, SourceLocation, BoxExtend);
}

void USMGA_BassNoiseBreak::OnWeaponTrailActivate(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	const USMHIC_Character* SourceHIC = GetHIC();
	const ASMWeaponBase* SourceWeapon = SourceCharacter ? SourceCharacter->GetWeapon() : nullptr;
	UMeshComponent* SourceWeaponMesh = SourceWeapon ? SourceWeapon->GetWeaponMeshComponent() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceHIC || !SourceWeaponMesh)
	{
		return;
	}

	const FVector WeaponOffset(30.0, 0.0, -145.0);

	const auto TargetActor = SourceHIC->GetActorIAmHolding();
	const float HoldingType = TargetActor ? (Cast<ASMPlayerCharacterBase>(TargetActor) ? 0.0f : 1.0f) : 0.0f; // 0이면 캐릭터, 1이면 아이템입니다.

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	GCParams.TargetAttachComponent = SourceWeaponMesh;
	GCParams.Location = WeaponOffset;
	GCParams.RawMagnitude = HoldingType;
	SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakWeaponTrail, GCParams);
}

void USMGA_BassNoiseBreak::OnWeaponTrailDeactivate(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	const USMHIC_Character* SourceHIC = GetHIC();
	if (!SourceCharacter || !SourceASC || !SourceHIC)
	{
		return;
	}

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakWeaponTrail, GCParams);
}

void USMGA_BassNoiseBreak::OnSlash(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	const USMHIC_Character* SourceHIC = GetHIC();
	if (!SourceCharacter || !SourceASC || !SourceHIC)
	{
		return;
	}

	const auto TargetActor = SourceHIC->GetActorIAmHolding();
	const float HoldingType = TargetActor ? (Cast<ASMPlayerCharacterBase>(TargetActor) ? 0.0f : 1.0f) : 0.0f; // 0이면 캐릭터, 1이면 아이템입니다.

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	GCParams.Location = NoiseBreakTargetLocation;
	GCParams.Normal = SourceCharacter->GetActorRotation().Vector();
	GCParams.RawMagnitude = HoldingType;
	SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakSlash, GCParams);
}
