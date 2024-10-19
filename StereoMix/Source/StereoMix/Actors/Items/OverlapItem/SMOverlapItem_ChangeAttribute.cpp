// Copyright Studio Surround. All Rights Reserved.


#include "SMOverlapItem_ChangeAttribute.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "Utilities/SMLog.h"

void ASMOverlapItem_ChangeAttribute::ActivateItem(AActor* InActivator)
{
	USMAbilitySystemComponent* ActivatorASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(InActivator);
	if (!ActivatorASC)
	{
		return;
	}

	if (!DataAsset)
	{
		return;
	}

	const FGameplayEffectSpecHandle GESpec = ActivatorASC->MakeOutgoingSpec(GE, 1.0f, ActivatorASC->MakeEffectContext());
	if (GESpec.IsValid())
	{
		NET_VLOG(this, INDEX_NONE, Magnitude < 0.0f ? 3.0f : Magnitude, TEXT("%s 버프 적용 중"), *GetName());
		GESpec.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
		ActivatorASC->BP_ApplyGameplayEffectSpecToSelf(GESpec);

		FGameplayCueParameters AddGCParams;
		AddGCParams.SourceObject = InActivator;
		ActivatorASC->AddGC(InActivator, GCTag, AddGCParams);

		if (const UWorld* World = GetWorld())
		{
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, [ActivatorWeakPtr = MakeWeakObjectPtr(InActivator), ActivatorASCWeakPtr = MakeWeakObjectPtr(ActivatorASC), CopiedGCTag = GCTag] {
				if (ActivatorWeakPtr.IsValid() && ActivatorASCWeakPtr.IsValid())
				{
					FGameplayCueParameters RemoveGCParams;
					RemoveGCParams.SourceObject = ActivatorWeakPtr;
					ActivatorASCWeakPtr->RemoveGC(ActivatorWeakPtr.Get(), CopiedGCTag, RemoveGCParams);
				}
			}, Magnitude, false);
		}
	}

	Super::ActivateItem(InActivator);
}
