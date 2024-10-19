// Copyright Studio Surround. All Rights Reserved.

#include "SMOverlapItem_ChangeMoveSpeed.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/Character/SMCharacterMovementComponent.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
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

	if (const USMAbilitySystemComponent* ActivatorASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(SourceCharacter))
	{
		FGameplayCueParameters AddGCParams;
		AddGCParams.SourceObject = InActivator;
		ActivatorASC->AddGC(InActivator, SMTags::GameplayCue::Common::Item::MoveSpeedUp, AddGCParams);

		if (const UWorld* World = GetWorld())
		{
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, [ActivatorWeakPtr = MakeWeakObjectPtr(InActivator), ActivatorASCWeakPtr = MakeWeakObjectPtr(ActivatorASC)] {
				if (ActivatorWeakPtr.IsValid() && ActivatorASCWeakPtr.IsValid())
				{
					FGameplayCueParameters RemoveGCParams;
					RemoveGCParams.SourceObject = ActivatorWeakPtr;
					ActivatorASCWeakPtr->RemoveGC(ActivatorWeakPtr.Get(), SMTags::GameplayCue::Common::Item::MoveSpeedUp, RemoveGCParams);
				}
			}, Duration, false);
		}
	}

	Super::ActivateItem(InActivator);
}
