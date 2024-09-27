// Copyright Studio Surround. All Rights Reserved.


#include "SMDestroyableObstacle.h"

#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"


ASMDestroyableObstacle::ASMDestroyableObstacle()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
}

void ASMDestroyableObstacle::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMDestroyableObstacle, CurrentHealth);
	DOREPLIFETIME(ASMDestroyableObstacle, MaxHealth);
}

void ASMDestroyableObstacle::HandleDamage(float InDamage)
{
	NET_VLOG(this, INDEX_NONE, 4.0f, TEXT("HandleDamage: %f"), InDamage);
}
