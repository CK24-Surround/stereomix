// Copyright Studio Surround. All Rights Reserved.


#include "SMOverlapItem_ChangeAttribute.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
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
		NET_LOG(this, Warning, TEXT("%s에게 GE 적용"), *GetNameSafe(InActivator));
		GESpec.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::HP, Magnitude);
		ActivatorASC->BP_ApplyGameplayEffectSpecToSelf(GESpec);
	}

	Super::ActivateItem(InActivator);
}
