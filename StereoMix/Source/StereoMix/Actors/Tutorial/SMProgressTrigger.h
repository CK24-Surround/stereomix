// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMProgressTrigger.generated.h"

class UNiagaraComponent;
class USphereComponent;

UCLASS()
class STEREOMIX_API ASMProgressTrigger : public AActor
{
	GENERATED_BODY()

public:
	ASMProgressTrigger();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<USphereComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> VFXComponent;
};
