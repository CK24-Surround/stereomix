// Copyright Studio Surround. All Rights Reserved.


#include "SMEP_ElectricGuitar.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"

void ASMEP_ElectricGuitar::HandleHitEffect(AActor* InTarget)
{
	Super::HandleHitEffect(InTarget);

	ApplyDamage(InTarget);
}

void ASMEP_ElectricGuitar::AddProjectileFX()
{
	AActor* SourceActor = GetOwner();

	const FGameplayTag GCTag = SMTags::GameplayCue::ElectricGuitar::ShootProjectile;

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = this;
	GCParams.TargetAttachComponent = GetRootComponent();

	if (const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		SourceASC->AddGC(SourceActor, GCTag, GCParams);
	}
	else
	{
		UGameplayCueManager::AddGameplayCue_NonReplicated(SourceActor, GCTag, GCParams);
	}
}

void ASMEP_ElectricGuitar::RemoveProjectileFX()
{
	AActor* SourceActor = GetOwner();

	const FGameplayTag GCTag = SMTags::GameplayCue::ElectricGuitar::ShootProjectile;

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = this;
	if (const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		SourceASC->RemoveGC(SourceActor, GCTag, GCParams);
	}
	else
	{
		UGameplayCueManager::RemoveGameplayCue_NonReplicated(SourceActor, GCTag, GCParams);
	}
}

void ASMEP_ElectricGuitar::PlayHitFX(AActor* InTarget)
{
	AActor* SourceActor = GetOwner();
	if (!InTarget)
	{
		return;
	}

	const ASMPlayerCharacterBase* SourceCharacter = GetOwner<ASMPlayerCharacterBase>();
	const APawn* TargetPawn = Cast<APawn>(InTarget);
	APlayerController* TargetPlayerController = TargetPawn ? TargetPawn->GetController<APlayerController>() : nullptr;
	APlayerController* PlayerController = SourceCharacter ? SourceCharacter->GetController<APlayerController>() : nullptr;
	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
	if (PlayerController && SourceDataAsset)
	{
		PlayerController->ClientStartCameraShake(SourceDataAsset->HitCameraShake);

		if (TargetPlayerController)
		{
			TargetPlayerController->ClientStartCameraShake(SourceDataAsset->HitCameraShake);
		}
	}

	const FGameplayTag GCTag = SMTags::GameplayCue::ElectricGuitar::ShootHit;

	FGameplayCueParameters GCParams;
	GCParams.Normal = GetActorRotation().Vector();
	GCParams.TargetAttachComponent = InTarget->GetRootComponent();
	if (const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		SourceASC->ExecuteGC(SourceActor, GCTag, GCParams);
	}
	else
	{
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(SourceActor, GCTag, GCParams);
	}
}

void ASMEP_ElectricGuitar::PlayWallHitFX(const FVector& HitLocation)
{
	AActor* SourceActor = GetOwner();
	if (USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		FGameplayCueParameters GCParams;
		GCParams.Location = HitLocation;
		GCParams.Normal = GetActorRotation().Vector();
		SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::ElectricGuitar::ShootWallHit, GCParams);
	}
}
