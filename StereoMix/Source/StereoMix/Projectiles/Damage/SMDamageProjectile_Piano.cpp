// Copyright Surround, Inc. All Rights Reserved.


#include "SMDamageProjectile_Piano.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Utilities/SMLog.h"


// Sets default values
ASMDamageProjectile_Piano::ASMDamageProjectile_Piano()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASMDamageProjectile_Piano::MulticastStartLifeTimeInternal_Implementation()
{
	AActor* SourceActor = GetOwner();
	Super::MulticastStartLifeTimeInternal_Implementation();

	if (HasAuthority())
	{
		USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
		if (SourceASC)
		{
			NET_LOG(SourceActor, Warning, TEXT("이펙트 요청"));
			FGameplayCueParameters GCParams;
			GCParams.TargetAttachComponent = GetRootComponent();
			SourceASC->AddGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryProjectileCharge1, GCParams);
		}
	}
}

void ASMDamageProjectile_Piano::MulticastEndLifeTimeInternal_Implementation()
{
	if (HasAuthority())
	{
		AActor* SourceActor = GetOwner();
		USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
		if (SourceASC)
		{
			SourceASC->RemoveGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryProjectileCharge1, FGameplayCueParameters());
		}
	}

	Super::MulticastEndLifeTimeInternal_Implementation();
}

void ASMDamageProjectile_Piano::PlayHitFX(AActor* InTarget)
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
	if (!InTarget || !SourceASC)
	{
		return;
	}

	FGameplayCueParameters GCParams;
	GCParams.Location = InTarget->GetActorLocation();
	SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryHitCharge1, GCParams);
}
