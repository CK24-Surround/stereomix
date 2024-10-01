// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_ColliderOrientationForSlash.h"

#include "Engine/OverlapResult.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Attack/SMGA_Slash.h"
#include "Characters/Player/SMBassCharacter.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMCollision.h"

USMAT_ColliderOrientationForSlash::USMAT_ColliderOrientationForSlash()
{
	bTickingTask = true;
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
	NewObj->SlashTime = TotalSlashTime;
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
	const float HalfRange = Range / 2.0f;
	SourceSlashColliderComponent->SetCapsuleHalfHeight(HalfRange);
	SourceSlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SourceSlashColliderComponent->SetRelativeLocation(FVector(HalfRange, 0.0, 0.0));
	SourceSlashColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::BeginOnOverlaped);

	const double Yaw = SourceAbility->bIsLeftSlashNext ? StartYaw : -StartYaw;
	TargetYaw = -Yaw;
	SourceSlashColliderRootComponent->SetRelativeRotation(FRotator(0.0, Yaw, 0.0));

	// 현재 캡슐의 중심과 끝점을 저장합니다.
	Locations.AddZeroed(2);
	const FVector RootLocation = SourceSlashColliderRootComponent->GetComponentLocation();
	const FVector Direction = SourceSlashColliderRootComponent->GetComponentRotation().Vector();
	Locations[0] = RootLocation + Direction * Range / 2.0f;
	Locations[1] = RootLocation + Direction * Range;
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

	// 캡슐의 이전 점에서 현재 점으로 라인트레이스를 수행합니다. 빈공간을 매꾸어 더욱 정확하게 판정할 수 있게됩니다.
	TArray<FVector> PreviousLocations = Locations;
	const FVector RootLocation = SourceSlashColliderRootComponent->GetComponentLocation();
	const FVector Direction = SourceSlashColliderRootComponent->GetComponentRotation().Vector();
	Locations[0] = RootLocation + Direction * Range / 2.0f;
	Locations[1] = RootLocation + Direction * Range;

	FCollisionQueryParams Params(TEXT("SlashTrace"), false, SourceCharacter.Get());
	for (int32 i = 0; i < Locations.Num(); ++i)
	{
		TArray<FOverlapResult> OverlapResults;
		const FVector CenterLocation = (PreviousLocations[i] + Locations[i]) / 2.0;
		const FVector PreviousToCurrent = Locations[i] - PreviousLocations[i];
		const FVector PreviousToCurrentDirection = PreviousToCurrent.GetSafeNormal();
		const FQuat PreviousToCurrentRotation = (PreviousToCurrentDirection.Rotation() + FRotator(90.0, 0.0, 0.0)).Quaternion();
		const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(10.0f, PreviousToCurrent.Size());
		if (GetWorld()->OverlapMultiByChannel(OverlapResults, CenterLocation, PreviousToCurrentRotation, SMCollisionTraceChannel::Action, CollisionShape, Params))
		{
			for (const FOverlapResult& OverlapResult : OverlapResults)
			{
				HandleHit(OverlapResult.GetActor());
			}
		}
	}

	if (bShowDebug)
	{
		DrawDebugCapsule(GetWorld(), SourceSlashColliderComponent->GetComponentLocation(), SourceSlashColliderComponent->GetScaledCapsuleHalfHeight(), SourceSlashColliderComponent->GetScaledCapsuleRadius(), SourceSlashColliderComponent->GetComponentRotation().Quaternion(), FColor::Red, false, 0.5f);

		{
			const FVector CenterLocation = (PreviousLocations[0] + Locations[0]) / 2.0;
			const FVector PreviousToCurrent = Locations[0] - PreviousLocations[0];
			const FVector PreviousToCurrentDirection = PreviousToCurrent.GetSafeNormal();
			const FQuat PreviousToCurrentRotation = (PreviousToCurrentDirection.Rotation() + FRotator(90.0, 0.0, 0.0)).Quaternion();
			DrawDebugCapsule(GetWorld(), CenterLocation, PreviousToCurrent.Size(), 10.0f, PreviousToCurrentRotation, FColor::Cyan, false, 0.5f);
		}

		{
			const FVector CenterLocation = (PreviousLocations[1] + Locations[1]) / 2.0;
			const FVector PreviousToCurrent = Locations[1] - PreviousLocations[1];
			const FVector PreviousToCurrentDirection = PreviousToCurrent.GetSafeNormal();
			const FQuat PreviousToCurrentRotation = (PreviousToCurrentDirection.Rotation() + FRotator(90.0, 0.0, 0.0)).Quaternion();
			DrawDebugCapsule(GetWorld(), CenterLocation, PreviousToCurrent.Size(), 10.0f, PreviousToCurrentRotation, FColor::Cyan, false, 0.5f);
		}
	}

	// 타겟 Yaw에 도달했다면 끝냅니다.
	const bool bHasReachedTargetYaw = TargetYaw >= 0 ? TargetYaw <= NewYaw : TargetYaw >= NewYaw;
	if (bHasReachedTargetYaw)
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
	HandleHit(OtherActor);
}

void USMAT_ColliderOrientationForSlash::HandleHit(AActor* OtherActor)
{
	ISMDamageInterface* OtherActorDamageInterface = Cast<ISMDamageInterface>(OtherActor);
	if (!OtherActorDamageInterface || OtherActorDamageInterface->CanIgnoreAttack())
	{
		return;
	}

	if (USMTeamBlueprintLibrary::IsSameTeam(SourceCharacter.Get(), OtherActor))
	{
		return;
	}

	if (DetectedActors.Find(OtherActor) != INDEX_NONE) // 이미 이번 베기에 포함된 타겟은 다시 처리되면 안 됩니다. 이를 방지하고자 작성된 코드입니다. 
	{
		return;
	}

	DetectedActors.Push(OtherActor);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		(void)OnSlashHit.ExecuteIfBound(OtherActor);
	}
}
