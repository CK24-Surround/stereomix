// Copyright Studio Surround. All Rights Reserved.

#include "SMOverlapItem_ChangeMoveSpeed.h"
#include "Characters/Component/SMCharacterMovementComponent.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Utilities/SMLog.h"

void ASMOverlapItem_ChangeMoveSpeed::ActivateItem(AActor* InActivator)
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(InActivator);
	USMCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement<USMCharacterMovementComponent>() : nullptr;
	if (!SourceCharacter || !SourceMovement)
	{
		return;
	}

	SourceMovement->ClientAddMoveSpeedBuff(MoveSpeedMultiplier, Duration);

	Super::ActivateItem(InActivator);
}
