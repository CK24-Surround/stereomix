// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchInteractionComponent_CatchableItem.h"

#include "Characters/SMPlayerCharacter.h"

void USMCatchInteractionComponent_CatchableItem::BeginPlay()
{
	Super::BeginPlay();

	SourceActor = GetOwner();
	check(SourceActor.Get());
}

bool USMCatchInteractionComponent_CatchableItem::IsCatchable(AActor* TargetActor) const
{
	if (CatchingMeActor.Get())
	{
		return false;
	}

	return true;
}

void USMCatchInteractionComponent_CatchableItem::OnCaught(AActor* TargetActor)
{
	InternalOnCaught(TargetActor);
}

void USMCatchInteractionComponent_CatchableItem::OnCaughtReleased(AActor* TargetActor, bool bIsStunTimeOut)
{
	InternalOnCaughtReleased(TargetActor);
}

void USMCatchInteractionComponent_CatchableItem::InternalOnCaught(AActor* TargetActor)
{
	if (!SourceActor->HasAuthority())
	{
		return;
	}

	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(TargetActor);
	if (!ensureAlways(TargetActor))
	{
		return;
	}

	// 대상에게 어태치합니다.
	const bool bAttachSuccess = SourceActor->AttachToComponent(TargetCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetCharacter->CatchSocket);
	if (!ensureAlways(bAttachSuccess))
	{
		return;
	}

	// 자신을 잡은 캐릭터를 저장합니다.
	SetActorCatchingMe(TargetActor);
}

void USMCatchInteractionComponent_CatchableItem::InternalOnCaughtReleased(AActor* TargetActor)
{
	if (!SourceActor->HasAuthority())
	{
		return;
	}

	// 디태치합니다.
	SourceActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 나를 잡고 있는 대상을 제거합니다.
	SetActorCatchingMe(nullptr);
}
