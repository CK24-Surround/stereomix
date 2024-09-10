// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ImpactArrow.h"

#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_SkillIndicator.h"
#include "Characters/Player/SMPianoCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/SMGamePlayerController.h"
#include "Data/SMControlData.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMCollision.h"

class ASMPlayerController;

USMGA_ImpactArrow::USMGA_ImpactArrow()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	// ActivationOwnedTags.AddTag(SMTags::Character::State::ImpactArrow);
}

void USMGA_ImpactArrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPianoCharacter* SourceCharacter = GetAvatarActor<ASMPianoCharacter>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!SourceASC)
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	if (!K2_CheckAbilityCost())
	{
		EndAbilityByCancel();
		return;
	}

	bCanShoot = true;

	if (K2_HasAuthority())
	{
		SourceASC->AddTag(SMTags::Character::State::ImpactArrow);
	}

	if (IsLocallyControlled())
	{
		ASMGamePlayerController* SourcePC = SourceCharacter->GetController<ASMGamePlayerController>();
		const USMControlData* ControlData = SourcePC->GetControlData();
		if (SourcePC && ControlData)
		{
			InputComponent = NewObject<UEnhancedInputComponent>(SourcePC);
			InputComponent->RegisterComponent();
			InputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Started, this, &ThisClass::Shoot);

			SourcePC->PushInputComponent(InputComponent);
		}

		MaxDistance = static_cast<float>(MaxDistanceByTile * 150);
		SkillIndicatorTask = USMAT_SkillIndicator::SkillIndicator(this, SourceCharacter, SourceCharacter->GetImpactArrowIndicator(), MaxDistance);
		SkillIndicatorTask->ReadyForActivation();
	}

	const FName TaskName = TEXT("MontageTask");
	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	UAnimMontage* Montage = SourceDataAsset->SkillMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage, 1.0f, NAME_None, false);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::ServerRPCRemoveTag);
	MontageTask->ReadyForActivation();
}

void USMGA_ImpactArrow::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (SourceCharacter)
	{
		ASMGamePlayerController* SourcePC = SourceCharacter->GetController<ASMGamePlayerController>();
		if (SourcePC)
		{
			if (InputComponent)
			{
				SourcePC->PopInputComponent(InputComponent);
				InputComponent = nullptr;
			}
		}
	}

	if (SkillIndicatorTask)
	{
		SkillIndicatorTask->EndTask();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_ImpactArrow::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ServerRPCRemoveTag();
	K2_CancelAbility();
}

void USMGA_ImpactArrow::Shoot()
{
	if (!bCanShoot)
	{
		return;
	}

	bCanShoot = false;

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return;
	}

	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	if (!SourceCapsule)
	{
		return;
	}

	ServerRPCApplyCost();

	const FVector CursorLocation = SourceCharacter->GetCursorTargetingPoint(true);
	TargetLocation = CursorLocation;

	const float CapsuleZOffset = SourceCapsule->GetScaledCapsuleHalfHeight();
	const FVector SourceLocation = SourceCharacter->GetActorLocation() - FVector(0.0, 0.0, CapsuleZOffset);
	if ((TargetLocation - SourceLocation).SizeSquared() >= FMath::Square(MaxDistance))
	{
		const FVector TargetDirection = (CursorLocation - SourceLocation).GetSafeNormal2D();
		TargetLocation = SourceLocation + (TargetDirection * MaxDistance);
	}

	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, TravelTime);
	WaitTask->OnFinish.AddDynamic(this, &ThisClass::OnImpact);
	WaitTask->ReadyForActivation();

	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);

	if (SkillIndicatorTask)
	{
		SkillIndicatorTask->EndTask();
	}
}

void USMGA_ImpactArrow::OnImpact()
{
	ServerRPCOnImpact(TargetLocation);
}

void USMGA_ImpactArrow::ServerRPCRemoveTag_Implementation()
{
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!SourceASC)
	{
		EndAbilityByCancel();
		return;
	}

	SourceASC->RemoveTag(SMTags::Character::State::ImpactArrow);
}

void USMGA_ImpactArrow::ServerRPCOnImpact_Implementation(const FVector_NetQuantize10& NewTargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return;
	}

	NET_LOG(GetAvatarActor(), Warning, TEXT("적중"));
	DrawDebugSphere(GetWorld(), NewTargetLocation, 250.0f, 16, FColor::Red, false, 1.0f);

	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, NewTargetLocation, FQuat::Identity, SMCollisionTraceChannel::Action, FCollisionShape::MakeSphere(Radius)))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* TargetActor = OverlapResult.GetActor();
			if (TargetActor)
			{
				ESMTeam SourceTeam = SourceCharacter->GetTeam();
				ESMTeam TargetTeam = USMTeamBlueprintLibrary::GetTeam(TargetActor);

				if (SourceTeam == TargetTeam)
				{
					continue;
				}

				ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
				if (TargetCharacter)
				{
					const FVector Direction = (TargetCharacter->GetActorLocation() - NewTargetLocation).GetSafeNormal2D();
					const FRotator Rotation = Direction.Rotation() + FRotator(15.0, 0.0, 0.0);
					const FVector Velocity = Rotation.Vector() * Magnitude;
					TargetCharacter->ClientRPCCharacterPushBack(Velocity);
				}

				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
				if (TargetASC)
				{
					const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
					if (SourceDataAsset)
					{
						FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(SourceDataAsset->DamageGE);
						if (GESpecHandle.IsValid())
						{
							GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::Data::Damage, Damage);
						}

						TargetASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
					}
				}
			}
		}
	}
}

void USMGA_ImpactArrow::ServerRPCApplyCost_Implementation()
{
	K2_CommitAbilityCost();
}
