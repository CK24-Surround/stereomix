// Copyright Studio Surround. All Rights Reserved.


#include "SMBow.h"


ASMBow::ASMBow()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
}
