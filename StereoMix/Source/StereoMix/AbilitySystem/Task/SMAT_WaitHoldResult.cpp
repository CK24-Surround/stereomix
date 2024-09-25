// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_WaitHoldResult.h"

#include "Engine/OverlapResult.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


USMAT_WaitHoldResult* USMAT_WaitHoldResult::WaitHoldResult(UGameplayAbility* OwningAbility, const FVector& NewStartLocation, const FVector& NewCursorLocation, float NewMaxDistance, float NewLimitAngle, float UnconditionalHoldDistance)
{
	USMAT_WaitHoldResult* NewObj = NewAbilityTask<USMAT_WaitHoldResult>(OwningAbility);
	NewObj->SourceCharacter = Cast<ASMPlayerCharacterBase>(OwningAbility->GetAvatarActorFromActorInfo());
	NewObj->StartLocation = NewStartLocation;
	NewObj->CursorLocation = NewCursorLocation;
	NewObj->MaxDistance = NewMaxDistance;
	NewObj->LimitAngle = NewLimitAngle;
	NewObj->UnconditionalHoldDistance = UnconditionalHoldDistance;
	return NewObj;
}

void USMAT_WaitHoldResult::Activate()
{
	AActor* HoldedActor = nullptr;

	TArray<FOverlapResult> OverlapResults;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(MaxDistance);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Hold), false, SourceCharacter.Get());
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, StartLocation, FQuat::Identity, SMCollisionTraceChannel::Action, CollisionShape, Params))
	{
		HoldedActor = GetOptimalHoldableActor(OverlapResults);
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnFinish.ExecuteIfBound(HoldedActor);
		EndTask();
	}
}

AActor* USMAT_WaitHoldResult::GetOptimalHoldableActor(const TArray<FOverlapResult>& InOverlapResults)
{
	if (!SourceCharacter.Get())
	{
		return nullptr;
	}

	// OverlapResults에서 잡을 수 있는 캐릭터만 추려냅니다.
	TArray<AActor*> HoldableActors;
	for (const auto& OverlapResult : InOverlapResults)
	{
		AActor* TargetActor = OverlapResult.GetActor();
		USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (!TargetHIC)
		{
			continue;
		}

		// 유효한 각도내에 있는지, 잡을 수 있는 상태인지 체크합니다.
		if (!IsValidateAngle(TargetActor) || !TargetHIC->CanHolded(SourceCharacter.Get()))
		{
			continue;
		}

		HoldableActors.Add(TargetActor);
	}

	if (HoldableActors.IsEmpty())
	{
		return nullptr;
	}

	// 커서 위치에 가장 가까운 액터를 반환합니다.
	AActor* TargetActor = GetClosestActorFromLocation(HoldableActors, CursorLocation);
	return TargetActor;
}

bool USMAT_WaitHoldResult::IsValidateAngle(const AActor* TargetActor)
{
	// 무조건 잡기 거리 이내에 타겟이 존재하면 각도 조건을 무시하고 유효한 각도로 취급합니다.
	const float DistanceSqaured = FVector::DistSquared(StartLocation, TargetActor->GetActorLocation());
	if (DistanceSqaured <= FMath::Square(UnconditionalHoldDistance))
	{
		return true;
	}

	// 각도 조건을 통해 잡을 수 있는지 없는지 추려냅니다.
	const FVector SourceToCursorDirection = (CursorLocation - StartLocation).GetSafeNormal();
	const FVector SourceToTargetDirection = (TargetActor->GetActorLocation() - StartLocation).GetSafeNormal();

	const float DotProduct = SourceToCursorDirection.Dot(SourceToTargetDirection);
	const float RadianDifference = FMath::Acos(DotProduct);

	// LimitDegree각도 내부라면 유효한 각도입니다.
	const float HalfLimitRadian = FMath::DegreesToRadians(LimitAngle / 2.0f);
	if (RadianDifference <= HalfLimitRadian)
	{
		return true;
	}

	return false;
}

AActor* USMAT_WaitHoldResult::GetClosestActorFromLocation(const TArray<AActor*>& InActors, const FVector& InLocation)
{
	AActor* ClosestActor = nullptr;
	float ClosestDistanceSquaredToCursor = MAX_FLT;
	for (const auto& Actor : InActors)
	{
		if (!Actor)
		{
			continue;
		}

		const FVector ActorLocation = Actor->GetActorLocation();
		const float DistanceSquared = FVector::DistSquared(ActorLocation, InLocation);
		if (DistanceSquared < ClosestDistanceSquaredToCursor)
		{
			ClosestActor = Actor;
			ClosestDistanceSquaredToCursor = DistanceSquared;
		}
	}

	return ClosestActor;
}
