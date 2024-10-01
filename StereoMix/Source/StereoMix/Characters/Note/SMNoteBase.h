// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMNoteBase.generated.h"

class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMNoteBase : public AActor
{
	GENERATED_BODY()

public:
	ASMNoteBase();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	USkeletalMeshComponent* GetMesh() { return MeshComponent; }

	void PlayAnimation();

	void StopAnimation();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> GlowFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> ParticleFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> FloorFXComponent;
};
