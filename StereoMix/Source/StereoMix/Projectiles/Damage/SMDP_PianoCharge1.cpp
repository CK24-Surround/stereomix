// Copyright Studio Surround. All Rights Reserved.


#include "SMDP_PianoCharge1.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Utilities/SMLog.h"


void ASMDP_PianoCharge1::AddProjectileFX()
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (SourceASC)
	{
		NET_LOG(SourceActor, Warning, TEXT("1차지 이펙트 요청"));
		FGameplayCueParameters GCParams;
		GCParams.TargetAttachComponent = GetRootComponent();
		SourceASC->AddGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryProjectileCharge1, GCParams);
	}
}

void ASMDP_PianoCharge1::RemoveProjectileFX()
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (SourceASC)
	{
		SourceASC->RemoveGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryProjectileCharge1, FGameplayCueParameters());
	}
}

void ASMDP_PianoCharge1::PlayHitFX(AActor* InTarget)
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (InTarget && SourceASC)
	{
		FGameplayCueParameters GCParams;
		GCParams.Location = InTarget->GetActorLocation();
		SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryHitCharge1, GCParams);
	}
}

void ASMDP_PianoCharge1::PlayWallHitFX(const FVector& HitLocation)
{
	AActor* SourceActor = GetOwner();
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (SourceASC)
	{
		FGameplayCueParameters GCParams;
		GCParams.Location = HitLocation;
		SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryHitCharge1, GCParams);
	}
}
