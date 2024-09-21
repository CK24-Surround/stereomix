// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_BassNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_ModifyGravityUntilLanded.h"
#include "Characters/Player/SMBassCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "Games/SMGameState.h"
#include "HoldInteraction/SMHIC_Character.h"
#include "Tiles/SMTile.h"
#include "Tiles/SMTileManagerComponent.h"
#include "Utilities/SMCollision.h"

USMGA_BassNoiseBreak::USMGA_BassNoiseBreak()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	if (FSMCharacterNoiseBreakData* NoiseBreakData = USMDataTableFunctionLibrary::GetCharacterNoiseBreakData(ESMCharacterType::Bass))
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

	ASMBassCharacter* SourceCharacter = GetAvatarActor<ASMBassCharacter>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
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

	// 몽타주를 재생합니다.
	const FName MontageTaskName = TEXT("MontageTask");
	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	CachedNoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, CachedNoiseBreakMontage, 1.0f, NAME_None, false);
	PlayMontageAndWaitTask->ReadyForActivation();

	// 타겟 위치로 도약합니다.
	if (IsLocallyControlled())
	{
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		FVector TargetLocation;

		const float MaxDistance = MaxDistanceByTile * 150.0f;
		SourceCharacter->GetTileLocationFromCursor(TargetLocation, MaxDistance);

		ServerSendLocationData(SourceLocation, TargetLocation);
		LeapCharacter(SourceLocation, TargetLocation, SourceMovement->GetGravityZ());
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

void USMGA_BassNoiseBreak::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_BassNoiseBreak::ServerSendLocationData_Implementation(const FVector_NetQuantize10& NewSourceLocation, const FVector_NetQuantize10& NewTargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	UCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement() : nullptr;
	if (!SourceCharacter || !SourceMovement)
	{
		EndAbilityByCancel();
		return;
	}

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
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	USMHIC_Character* SourceHIC = Cast<USMHIC_Character>(GetHIC());
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
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::NoiseBreakEnd);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnNoiseBreakEnded);
	WaitGameplayEventTask->ReadyForActivation();

	if (K2_HasAuthority())
	{
		AActor* TargetActor = SourceHIC->GetActorIAmHolding();
		USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (TargetHIC)
		{
			TargetHIC->OnNoiseBreakActionPerformed(SourceCharacter, MakeShared<FSMNoiseBreakData>());
		}

		SourceHIC->SetActorIAmHolding(nullptr);

		TileCapture();
		ApplySplash(SourceCharacter->GetActorLocation(), SMTags::GameplayCue::Bass::NoiseBreakBurstHit);
	}

	if (IsLocallyControlled())
	{
		FGameplayCueParameters GCParams;
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		ASMTile* Tile = (GetTileFromLocation(SourceLocation));
		if (Tile)
		{
			GCParams.Location = Tile->GetTileLocation();
		}
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakBurst, GCParams);
	}

	// SourceASC->RemoveGameplayCue(SMTags::GameplayCue::SpecialAction::Smash);
}

void USMGA_BassNoiseBreak::OnNoiseBreakEnded(FGameplayEventData Payload)
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}

void USMGA_BassNoiseBreak::TileCapture()
{
	ASMBassCharacter* SourceCharacter = GetAvatarActor<ASMBassCharacter>();
	ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>();
	USMTileManagerComponent* TileManager = GameState ? GameState->GetTileManager() : nullptr;
	if (!SourceCharacter || !ensureAlways(TileManager))
	{
		return;
	}

	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	ASMTile* Tile = GetTileFromLocation(SourceLocation);
	if (Tile)
	{
		const ESMTeam SourceTeam = SourceCharacter->GetTeam();
		TileManager->TileCaptureDelayedSqaure(Tile, SourceTeam, CaptureSize, TotalTriggerTime);
	}
}
