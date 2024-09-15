// Copyright Studio Surround. All Rights Reserved.


#include "SMWeaponBase.h"


ASMWeaponBase::ASMWeaponBase()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
}
