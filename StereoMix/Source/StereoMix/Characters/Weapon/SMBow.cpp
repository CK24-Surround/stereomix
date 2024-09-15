// Copyright Studio Surround. All Rights Reserved.


#include "SMBow.h"

ASMBow::ASMBow(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USkeletalMeshComponent>(ASMWeaponBase::WeaponMeshComponentName)) {}
