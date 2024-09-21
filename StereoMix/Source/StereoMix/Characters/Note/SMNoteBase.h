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

protected:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> MeshComponent; 
};
