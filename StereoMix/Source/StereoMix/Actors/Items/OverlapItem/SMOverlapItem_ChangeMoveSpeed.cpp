// Copyright Studio Surround. All Rights Reserved.

#include "SMOverlapItem_ChangeMoveSpeed.h"

#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/Character/SMCharacterMovementComponent.h"
#include "Utilities/SMLog.h"

void ASMOverlapItem_ChangeMoveSpeed::ActivateItem(AActor* InActivator)
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(InActivator);
	USMCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement<USMCharacterMovementComponent>() : nullptr;
	if (!SourceCharacter || !SourceMovement)
	{
		return;
	}

	NET_VLOG(this, INDEX_NONE, Duration < 0.0f ? 3.0f : Duration, TEXT("%s 버프 적용 중"), *GetName());
	SourceMovement->ClientAddMoveSpeedBuff(MoveSpeedMultiplier, Duration);

	Super::ActivateItem(InActivator);
}
