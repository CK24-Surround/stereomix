// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMNoteBase.generated.h"

UCLASS()
class STEREOMIX_API ASMNoteBase : public AActor
{
	GENERATED_BODY()

public:
	ASMNoteBase();

	virtual void PostInitializeComponents() override;

	void PlayAnimation();

	void StopAnimation();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
};
