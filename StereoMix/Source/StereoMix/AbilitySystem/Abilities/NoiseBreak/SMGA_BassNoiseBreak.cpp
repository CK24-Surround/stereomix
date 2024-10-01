// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_BassNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_ModifyGravityUntilLanded.h"
#include "Characters/Component/SMHIC_Character.h"
#include "Characters/Player/SMBassCharacter.h"
#include "Characters/Weapon/SMWeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Tiles/SMTile.h"
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
		PerformBurstAttack(SourceCharacter->GetActorLocation(), SMTags::GameplayCue::Bass::NoiseBreakBurstHit);
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
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const ESMTeam SourceTeam = SourceCharacter ? SourceCharacter->GetTeam() : ESMTeam::None;
	if (!SourceCharacter || SourceTeam == ESMTeam::None)
	{
		return;
	}

	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	USMTileFunctionLibrary::TileCaptureDelayedSqaure(GetWorld(), SourceLocation, SourceTeam, CaptureSize, TotalTriggerTime, true);
}

void USMGA_BassNoiseBreak::OnWeaponTrailActivate(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	ASMWeaponBase* SourceWeapon = SourceCharacter ? SourceCharacter->GetWeapon() : nullptr;
	UMeshComponent* SourceWeaponMesh = SourceWeapon ? SourceWeapon->GetWeaponMeshComponent() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceWeaponMesh)
	{
		return;
	}

	const FVector WeaponOffset(30.0, 0.0, -145.0);
	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	GCParams.TargetAttachComponent = SourceWeaponMesh;
	GCParams.Location = WeaponOffset;
	SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakWeaponTrail, GCParams);
}

void USMGA_BassNoiseBreak::OnWeaponTrailDeactivate(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	if (!SourceCharacter || !SourceASC)
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
	USMAbilitySystemComponent* SourceASC = GetASC();
	if (!SourceCharacter || !SourceASC)
	{
		return;
	}

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	GCParams.Location = NoiseBreakTargetLocation;
	GCParams.Normal = SourceCharacter->GetActorRotation().Vector();
	SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakSlash, GCParams);
}
