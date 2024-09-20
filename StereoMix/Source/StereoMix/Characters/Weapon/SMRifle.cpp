// Copyright Studio Surround. All Rights Reserved.


#include "SMRifle.h"

ASMRifle::ASMRifle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(ASMWeaponBase::WeaponMeshComponentName)) {}
