// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_ColliderOrientationForSlash.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Abilities/Attack/SMGA_Slash.h"
#include "Characters/Player/SMBassCharacter.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"

USMAT_ColliderOrientationForSlash::USMAT_ColliderOrientationForSlash()
{
	bTickingTask = true;

	InvalidTargetTag.AddTag(SMTags::Character::State::Immune);
	InvalidTargetTag.AddTag(SMTags::Character::State::Neutralize);
}

USMAT_ColliderOrientationForSlash* USMAT_ColliderOrientationForSlash::ColliderOrientationForSlash(UGameplayAbility* OwningAbility, float Range, float Angle, float TotalSlashTime, bool bShowDebug)
{
	USMAT_ColliderOrientationForSlash* NewObj = NewAbilityTask<USMAT_ColliderOrientationForSlash>(OwningAbility);
	NewObj->SourceAbility = Cast<USMGA_Slash>(OwningAbility);
	NewObj->SourceCharacter = Cast<ASMBassCharacter>(OwningAbility->GetAvatarActorFromActorInfo());

	ASMBassCharacter* CachedSourceCharacter = NewObj->SourceCharacter.Get();
	if (CachedSourceCharacter)
	{
		NewObj->SourceSlashColliderRootComponent = CachedSourceCharacter->GetSlashColliderRootComponent();
		NewObj->SourceSlashColliderComponent = CachedSourceCharacter->GetSlashColliderComponent();
	}

	NewObj->Range = Range;
	NewObj->Angle = Angle;
	NewObj->SlashTime = 0.1875f;
	NewObj->bShowDebug = bShowDebug;

	return NewObj;
}

void USMAT_ColliderOrientationForSlash::Activate()
{
	if (!SourceAbility.Get())
	{
		return;
	}

	if (!ensureAlways(SourceSlashColliderComponent.Get()))
	{
		return;
	}

	StartYaw = Angle / 2.0f;
	SlashSpeed = Angle * (1.0f / SlashTime);
	SourceSlashColliderComponent->SetCapsuleHalfHeight(Range / 2.0f);
	SourceSlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SourceSlashColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::BeginOnOverlaped);

	const double Yaw = SourceAbility->bIsLeftSlashNext ? StartYaw : -StartYaw;
	TargetYaw = -Yaw;
	SourceSlashColliderRootComponent->SetRelativeRotation(FRotator(0.0, Yaw, 0.0));
}

void USMAT_ColliderOrientationForSlash::TickTask(float DeltaTime)
{
	if (!SourceCharacter.Get())
	{
		return;
	}

	// 타겟 Yaw를 향해 상수속도로 보간합니다.
	const FRotator SourceRotation = SourceSlashColliderRootComponent->GetRelativeRotation();
	const double NewYaw = FMath::FInterpConstantTo(SourceRotation.Yaw, TargetYaw, GetWorld()->GetDeltaSeconds(), SlashSpeed);
	SourceSlashColliderRootComponent->SetRelativeRotation(FRotator(0.0, NewYaw, 0.0));

	if (bShowDebug)
	{
		DrawDebugCapsule(GetWorld(), SourceSlashColliderComponent->GetComponentLocation(), SourceSlashColliderComponent->GetScaledCapsuleHalfHeight(), SourceSlashColliderComponent->GetScaledCapsuleRadius(), SourceSlashColliderComponent->GetComponentRotation().Quaternion(), FColor::Red, false, 0.5f);
	}

	// 타겟 Yaw에 도달했다면 끝냅니다.
	if (FMath::IsNearlyEqual(SourceRotation.Yaw, NewYaw))
	{
		EndTask();
	}
}

void USMAT_ColliderOrientationForSlash::OnDestroy(bool bInOwnerFinished)
{
	if (SourceSlashColliderComponent.Get())
	{
		SourceSlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SourceSlashColliderComponent->OnComponentBeginOverlap.RemoveAll(this);
	}

	DetectedActors.Reset();
	OnSlashHit.Unbind();

	Super::OnDestroy(bInOwnerFinished);
}

void USMAT_ColliderOrientationForSlash::BeginOnOverlaped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == SourceCharacter)
	{
		return;
	}

	if (!IsValidTarget(OtherActor))
	{
		return;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(OtherActor);
	if (TargetCharacter)
	{
		// 이미 이번 베기에 감지된 타겟은 다시 처리되면 안 됩니다. 이를 방지하고자 작성된 코드입니다. 
		if (DetectedActors.Find(TargetCharacter) == INDEX_NONE)
		{
			DetectedActors.Push(TargetCharacter);

			if (ShouldBroadcastAbilityTaskDelegates())
			{
				(void)OnSlashHit.ExecuteIfBound(TargetCharacter);
			}
		}
	}
}

bool USMAT_ColliderOrientationForSlash::IsValidTarget(AActor* OtherActor)
{
	ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(SourceCharacter.Get());
	ESMTeam TargetTeam = USMTeamBlueprintLibrary::GetTeam(OtherActor);
	if (SourceTeam == TargetTeam)
	{
		return false;
	}

	// 대상이 면역상태라면 무시합니다.
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (TargetASC)
	{
		if (TargetASC->HasAnyMatchingGameplayTags(InvalidTargetTag))
		{
			return false;
		}
	}

	return true;
}
