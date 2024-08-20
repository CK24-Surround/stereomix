// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Slash.h"

#include "Engine/OverlapResult.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

USMGA_Slash::USMGA_Slash()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void USMGA_Slash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	K2_CommitAbility();

	Smash();
}

void USMGA_Slash::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		const FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass());
		if (ensureAlways(GESpecHandle.IsValid()))
		{
			FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
			GESpec->SetSetByCallerMagnitude(SMTags::Data::Cooldown, Cooldown);
			(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, GESpecHandle);
		}
	}
}

void USMGA_Slash::Smash()
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ensureAlways(SourceCharacter))
	{
		K2_EndAbility();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	const USMPlayerCharacterDataAsset* CharacterDataAsset = SourceCharacter->GetDataAsset();
	if (CharacterDataAsset)
	{
		SourceASC->PlayMontage(this, CurrentActivationInfo, CharacterDataAsset->AttackMontage[SourceTeam], 1.0f);
	}

	if (IsLocallyControlled())
	{
		CalculateCurrentYaw();
	}
}

void USMGA_Slash::CalculateCurrentYaw()
{
	SlashData.SlashYawOffsets.SetNumZeroed(SlashData.SegmentCount);
	SlashData.Count = 0;

	const float HalfAngle = Angle / 2.0f;
	const float StartYaw = -HalfAngle;
	const float YawSegment = Angle / (SlashData.SegmentCount - 1);

	for (int32 i = 0; i < SlashData.SegmentCount; ++i)
	{
		SlashData.SlashYawOffsets[i] = StartYaw + (YawSegment * i);
	}

	SlashData.Rate = SlashTime / (SlashData.SegmentCount - 1.0f);

	SendCurrentYaw();
}

void USMGA_Slash::SendCurrentYaw()
{
	if (SlashData.Count < SlashData.SegmentCount)
	{
		ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(GetAvatarActorFromActorInfo());
		if (!ensureAlways(SourceCharacter))
		{
			return;
		}

		const FVector ForwardDirection = (SourceCharacter->GetCursorTargetingPoint() - SourceCharacter->GetActorLocation()).GetSafeNormal();
		float ForwardYaw = ForwardDirection.Rotation().Yaw + SlashData.SlashYawOffsets[SlashData.Count];
		ServerRPCSendCurrentYaw(ForwardYaw);
		++SlashData.Count;

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::SendCurrentYaw, SlashData.Rate, false);
	}
	else
	{
		K2_EndAbility();
	}
}

void USMGA_Slash::ServerRPCSendCurrentYaw_Implementation(float Yaw)
{
	GetDetectedTarget(Yaw);
}

TArray<AActor*> USMGA_Slash::GetDetectedTarget(float Yaw)
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ensureAlways(SourceCharacter))
	{
		return TArray<AActor*>();
	}

	const FRotator Rotation = FRotator(0.0, Yaw, 0.0);
	const float HalfDistance = Distance / 2.0f;

	TArray<FOverlapResult> OverlapResults;
	const FVector StartLocation = SourceCharacter->GetActorLocation() + (Rotation.Vector() * HalfDistance);
	const FQuat StartRotation = FRotator(-90.0, Yaw, 0.0).Quaternion();
	const float Radius = 100.0f;
	const float HalfHeight = HalfDistance - Radius;
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Slash), false, SourceCharacter);
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, StartLocation, StartRotation, SMCollisionTraceChannel::Action, Shape, Params)) {}
	DrawDebugCapsule(GetWorld(), StartLocation, HalfHeight, Radius, StartRotation, FColor::Red, false, 1.0f);

	return TArray<AActor*>();
}
