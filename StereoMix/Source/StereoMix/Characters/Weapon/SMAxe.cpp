// Copyright Studio Surround. All Rights Reserved.


#include "SMAxe.h"

ASMAxe::ASMAxe(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(ASMWeaponBase::WeaponMeshComponentName)) {}
