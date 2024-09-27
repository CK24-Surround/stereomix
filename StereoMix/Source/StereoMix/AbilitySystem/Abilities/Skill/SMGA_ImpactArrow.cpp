// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ImpactArrow.h"

#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_SkillIndicator.h"
#include "Characters/Player/SMPianoCharacter.h"
#include "Characters/Weapon/SMWeaponBase.h"
#include "Controllers/SMGamePlayerController.h"
#include "Data/SMControlData.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMCollision.h"


USMGA_ImpactArrow::USMGA_ImpactArrow()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	ActivationOwnedTags.AddTag(SMTags::Character::State::ImpactArrow);

	if (FSMCharacterSkillData* SkillData = USMDataTableFunctionLibrary::GetCharacterSkillData(ESMCharacterType::Piano))
	{
		Damage = SkillData->Damage;
		MaxDistanceByTile = SkillData->DistanceByTile;
		StartUpTime = SkillData->StartUpTime;
		Radius = SkillData->Range;
		KnockbackMagnitude = SkillData->Magnitude;
	}
}

void USMGA_ImpactArrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPianoCharacter* SourceCharacter = GetCharacter<ASMPianoCharacter>();
	USMAbilitySystemComponent* SourceASC = GetASC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	ASMGamePlayerController* SourcePC = SourceCharacter ? SourceCharacter->GetController<ASMGamePlayerController>() : nullptr;
	const USMControlData* ControlData = SourcePC ? SourcePC->GetControlData() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceDataAsset || !SourcePC || !ControlData)
	{
		EndAbilityByCancel();
		return;
	}

	if (IsLocallyControlled())
	{
		InputComponent = NewObject<UEnhancedInputComponent>(SourcePC);
		InputComponent->RegisterComponent();
		InputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Started, this, &ThisClass::Shoot);
		SourcePC->PushInputComponent(InputComponent);

		const float MaxDistance = MaxDistanceByTile * 150.0f;
		SkillIndicatorTask = USMAT_SkillIndicator::SkillIndicator(this, SourceCharacter->GetImpactArrowIndicator(), MaxDistance);
		SkillIndicatorTask->ReadyForActivation();

		ASMWeaponBase* SourceWeapon = SourceCharacter->GetWeapon();
		UMeshComponent* SourceWeaponMesh = SourceWeapon ? SourceWeapon->GetWeaponMeshComponent() : nullptr;
		if (SourceWeaponMesh)
		{
			const FVector WeaponOffset(0.0, 0.0, 20.0);

			FGameplayCueParameters GCParams;
			GCParams.SourceObject = SourceCharacter;
			GCParams.TargetAttachComponent = SourceWeaponMesh;
			GCParams.Location = WeaponOffset;
			SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Piano::ImpactArrowReady, GCParams);
		}
	}

	const FName TaskName = TEXT("MontageTask");
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	UAnimMontage* Montage = SourceDataAsset->SkillMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage, 1.0f, NAME_None, false);
	MontageTask->ReadyForActivation();
}

void USMGA_ImpactArrow::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	ASMGamePlayerController* SourcePC = SourceCharacter ? SourceCharacter->GetController<ASMGamePlayerController>() : nullptr;
	if (SourcePC && InputComponent)
	{
		SourcePC->PopInputComponent(InputComponent);
		InputComponent = nullptr;
	}

	if (SkillIndicatorTask)
	{
		SkillIndicatorTask->EndTask();
	}

	bHasClicked = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_ImpactArrow::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	const FName SectionName = TEXT("Cancel");
	MontageJumpToSection(SectionName);

	if (USMAbilitySystemComponent* SourceASC = GetASC())
	{
		AActor* SourceActor = GetAvatarActor();
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceActor;
		SourceASC->RemoveGC(SourceActor, SMTags::GameplayCue::Piano::ImpactArrowReady, GCParams);
	}

	K2_EndAbility();
}

void USMGA_ImpactArrow::Shoot()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!SourceCharacter || !SourceASC)
	{
		return;
	}

	if (bHasClicked)
	{
		return;
	}

	bHasClicked = true;

	ServerRPCApplyCost();

	const float MaxDistance = MaxDistanceByTile * 150.0f;
	TargetLocation = SourceCharacter->GetLocationFromCursor(true, MaxDistance);

	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);

	if (SkillIndicatorTask)
	{
		SkillIndicatorTask->EndTask();
	}

	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, StartUpTime);
	WaitTask->OnFinish.AddDynamic(this, &ThisClass::OnImpact);
	WaitTask->ReadyForActivation();

	FGameplayCueParameters ReadyEndGCParams;
	ReadyEndGCParams.SourceObject = SourceCharacter;
	SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Piano::ImpactArrowReady, ReadyEndGCParams);

	const FVector Direciton = FRotator(60.0, 0.0, 0.0).Vector();
	const FVector OffsetLocation = Direciton * 300.0;
	FGameplayCueParameters ShootGCParams;
	ShootGCParams.SourceObject = SourceCharacter;
	ShootGCParams.Location = OffsetLocation;
	ShootGCParams.Normal = Direciton;
	ShootGCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
	SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Piano::ImpactArrow, ShootGCParams);

	SyncPointImpactEnd();
}

void USMGA_ImpactArrow::ServerRPCApplyCost_Implementation()
{
	K2_CommitAbilityCost();
}

void USMGA_ImpactArrow::OnImpact()
{
	ServerRPCOnImpact(TargetLocation);
}

void USMGA_ImpactArrow::ServerRPCOnImpact_Implementation(const FVector_NetQuantize10& NewTargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceASC || !SourceDataAsset)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, NewTargetLocation, FQuat::Identity, SMCollisionTraceChannel::Action, FCollisionShape::MakeSphere(Radius)))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* TargetActor = OverlapResult.GetActor();
			if (!TargetActor)
			{
				continue;
			}

			const ESMTeam SourceTeam = SourceCharacter->GetTeam();
			const ESMTeam TargetTeam = USMTeamBlueprintLibrary::GetTeam(TargetActor);

			if (SourceTeam == TargetTeam)
			{
				continue;
			}

			ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
			if (TargetCharacter)
			{
				const FVector Direction = (TargetCharacter->GetActorLocation() - NewTargetLocation).GetSafeNormal2D();
				const FRotator Rotation = Direction.Rotation() + FRotator(15.0, 0.0, 0.0);
				const FVector Velocity = Rotation.Vector() * KnockbackMagnitude;
				TargetCharacter->ClientRPCCharacterPushBack(Velocity);

				// 방향에 맞게 이펙트를 재생합니다.
				FGameplayCueParameters GCParams;
				GCParams.SourceObject = SourceCharacter;
				GCParams.TargetAttachComponent = TargetActor->GetRootComponent();
				GCParams.Normal = Direction;
				SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Piano::ImpactArrowHit, GCParams);
			}

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (TargetASC)
			{
				FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(SourceDataAsset->DamageGE);
				if (GESpecHandle.IsValid())
				{
					GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, Damage);
				}

				TargetASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
			}
		}
	}

	FGameplayCueParameters GCParams;
	GCParams.Location = NewTargetLocation;
	SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Piano::ImpactArrowExplosion, GCParams);

	if (bUseDebug)
	{
		DrawDebugSphere(GetWorld(), NewTargetLocation, Radius, 16, FColor::Red, false, 1.0f);
	}

	SyncPointImpactEnd();
}

void USMGA_ImpactArrow::SyncPointImpactEnd()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}
