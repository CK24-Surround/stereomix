// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify_ImmuneMaterial.h"

#include "Characters/SMPlayerCharacter.h"

bool USMGameplayCueNotify_ImmuneMaterial::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	ASMPlayerCharacter* SourceCharacter = Cast<ASMPlayerCharacter>(MyTarget);
	if (!ensureAlways(SourceCharacter))
	{
		return false;
	}

	if (bIsApply)
	{
		ApplyMaterial(SourceCharacter);
	}
	else
	{
		ResetMaterial(SourceCharacter);
	}
	
	return true;
}

void USMGameplayCueNotify_ImmuneMaterial::ApplyMaterial(ASMPlayerCharacter* SourceCharacter) const
{
	if (!ensureAlways(SourceCharacter))
	{
		return;
	}

	USkeletalMeshComponent* SourceMesh = SourceCharacter->GetMesh();
	if (!ensureAlways(SourceMesh))
	{
		return;
	}

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	if (ImmuneMaterial.Find(SourceTeam))
	{
		// 면역 머티리얼 적용이 필요한 머티리얼 슬롯에 면역 머티리얼을 적용합니다.
		for (const auto& ApplyIndexNumber : ApplyIndexNumbers)
		{
			SourceMesh->SetMaterial(ApplyIndexNumber, ImmuneMaterial[SourceTeam]);
		}
	}
}

void USMGameplayCueNotify_ImmuneMaterial::ResetMaterial(ASMPlayerCharacter* SourceCharacter) const
{
	if (!ensureAlways(SourceCharacter))
	{
		return;
	}

	USkeletalMeshComponent* SourceMesh = SourceCharacter->GetMesh();
	if (!ensureAlways(SourceMesh))
	{
		return;
	}

	const TArray<TObjectPtr<UMaterialInterface>>& OriginMaterials = SourceCharacter->GetOriginMaterials();
	for (int32 i = 0; i < OriginMaterials.Num(); ++i)
	{
		SourceMesh->SetMaterial(i, OriginMaterials[i]);
	}
}
