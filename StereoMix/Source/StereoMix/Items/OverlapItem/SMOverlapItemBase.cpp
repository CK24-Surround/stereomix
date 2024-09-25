// Copyright Studio Surround. All Rights Reserved.


#include "SMOverlapItemBase.h"

#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Components/SphereComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMOverlapItemBase::ASMOverlapItemBase()
{
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMOverlapItemBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::OverlapItem);
	}
}

void ASMOverlapItemBase::ActivateItem(AActor* InActivator)
{
	Super::ActivateItem(InActivator);
}

void ASMOverlapItemBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ASMPlayerCharacterBase* OtherCharacter = Cast<ASMPlayerCharacterBase>(OtherActor);
	if (!OtherCharacter)
	{
		return;
	}

	NET_LOG(this, Warning, TEXT("%s가 아이템을 획득 했습니다."), *GetNameSafe(OtherActor));
	ActivateItem(OtherActor);

	Destroy();
}
