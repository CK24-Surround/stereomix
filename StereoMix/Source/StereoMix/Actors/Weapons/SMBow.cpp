// Copyright Studio Surround. All Rights Reserved.


#include "SMBow.h"

#include "NiagaraComponent.h"
#include "Utilities/SMLog.h"

ASMBow::ASMBow(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USkeletalMeshComponent>(ASMWeaponBase::WeaponMeshComponentName))
{
	VFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFXComponent"));
	VFXComponent->SetupAttachment(RootComponent);
	VFXComponent->SetRelativeLocation(FVector(10.0, 0.0, 22.0));
}

void ASMBow::SetVFXEnabled(bool bNewIsVFXEnabled)
{
	VFXComponent->SetVisibility(bNewIsVFXEnabled);
}
