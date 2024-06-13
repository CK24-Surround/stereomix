// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "SMJumpPad.generated.h"

class UFMODEvent;
class ASMPlayerCharacter;
class UBoxComponent;

UCLASS()
class STEREOMIX_API ASMJumpPad : public AActor
{
	GENERATED_BODY()

public:
	ASMJumpPad();

protected:
	virtual void PostInitializeComponents() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void Jump(ACharacter* InCharacter, const FVector& TargetLocation);

	void TargetLanded(ASMPlayerCharacter* LandedCharacter);

	/** 만약 점프대 사용 중 캐릭터가 파괴된 경우 예외처리를 위한 함수입니다. */
	UFUNCTION()
	void OnDestroyedUsingJumpPadCharacter(AActor* DestroyedActor);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPlayUseJumpPadFX(ASMPlayerCharacter* SourceCharacter);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPlayLandedFX(ASMPlayerCharacter* SourceCharacter);

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "JumpTarget")
	TObjectPtr<USceneComponent> JumpTarget;

	UPROPERTY(EditAnywhere, Category = "Design|Jump")
	float GravityScale = 4.0f;

	TMap<TWeakObjectPtr<AActor>, float> OriginGravityScale;

	TMap<TWeakObjectPtr<AActor>, float> OriginAirControl;

	UPROPERTY(EditAnywhere, Category = "Design|Jump")
	float ApexHeight = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tags")
	FGameplayTagContainer DenineTags;
};
