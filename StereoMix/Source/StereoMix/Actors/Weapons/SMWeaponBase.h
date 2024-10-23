// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMWeaponBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	ASMWeaponBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;

	const static FName WeaponMeshComponentName;

	UMeshComponent* GetWeaponMeshComponent() const { return WeaponMeshComponent; }

	template<typename T>
	T* GetWeaponMeshComponent() const { return Cast<T>(GetWeaponMeshComponent()); }

	TArray<UMaterialInterface*> GetOriginalMaterials() const { return OriginalMaterials; }

	UMaterialInterface* GetOriginalOverlayMaterial() const { return OriginalOverlayMaterial; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UMeshComponent> WeaponMeshComponent;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> OriginalOverlayMaterial;
};
