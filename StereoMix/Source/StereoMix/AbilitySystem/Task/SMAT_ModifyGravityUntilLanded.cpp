// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_ModifyGravityUntilLanded.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/Player/SMPlayerCharacterBase.h"

USMAT_ModifyGravityUntilLanded* USMAT_ModifyGravityUntilLanded::ModifyGravityUntilLanded(UGameplayAbility* OwningAbility, bool bUseGravityModifier, float GravityModifierValue)
{
	USMAT_ModifyGravityUntilLanded* NewObj = NewAbilityTask<USMAT_ModifyGravityUntilLanded>(OwningAbility);
	NewObj->SourceCharacter = Cast<ASMPlayerCharacterBase>(OwningAbility->GetAvatarActorFromActorInfo());
	NewObj->bChangeGravity = bUseGravityModifier;
	NewObj->TargetGravity = GravityModifierValue;
	return NewObj;
}

void USMAT_ModifyGravityUntilLanded::Activate()
{
	if (!SourceCharacter.Get())
	{
		return;
	}

	SourceCharacter->OnCharacterLanded.AddUObject(this, &ThisClass::OnLandedCallback);

	if (bChangeGravity)
	{
		UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
		if (SourceMovement)
		{
			OriginalGravity = SourceMovement->GravityScale;
			SourceMovement->GravityScale = TargetGravity;
		}
	}
}

void USMAT_ModifyGravityUntilLanded::OnDestroy(bool bInOwnerFinished)
{
	if (SourceCharacter.Get())
	{
		SourceCharacter->OnCharacterLanded.RemoveAll(this);

		if (bChangeGravity)
		{
			UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
			if (SourceMovement)
			{
				SourceMovement->GravityScale = OriginalGravity;
				SourceMovement->StopMovementImmediately();
			}
		}
	}

	Super::OnDestroy(bInOwnerFinished);
}

void USMAT_ModifyGravityUntilLanded::OnLandedCallback(ASMPlayerCharacterBase* LandedCharacter)
{
	if (!SourceCharacter.Get())
	{
		return;
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		(void)OnLanded.ExecuteIfBound();
	}

	EndTask();
}
