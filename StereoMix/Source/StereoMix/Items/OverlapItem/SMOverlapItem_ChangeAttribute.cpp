// Copyright Studio Surround. All Rights Reserved.


#include "SMOverlapItem_ChangeAttribute.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
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

	FGameplayEffectSpecHandle GESpec = ActivatorASC->MakeOutgoingSpec(GE, 1.0f, ActivatorASC->MakeEffectContext());
	if (GESpec.IsValid())
	{
		NET_VLOG(this, INDEX_NONE, Magnitude < 0.0f ? 3.0f : Magnitude, TEXT("%s 버프 적용 중"), *GetName());
		GESpec.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
		ActivatorASC->BP_ApplyGameplayEffectSpecToSelf(GESpec);
	}

	Super::ActivateItem(InActivator);
}
