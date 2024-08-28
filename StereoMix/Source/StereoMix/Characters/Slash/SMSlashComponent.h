// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SMSlashComponent.generated.h"


class UAnimInstance;
class UGameplayEffect;
class UCapsuleComponent;
class ASMBassCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMSlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMSlashComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

	void TrySlash();

	void ColliderOrientaionForSlash();

	uint32 bIsLeftSlashNext:1 = true;
	uint32 bCanInput:1 = false;
	uint32 bCanNextAction:1 = false;
	uint32 bIsInput:1 = false;

protected:
	void SlashStart();

	void ReSlash();

	void UpdateSlash();

	void SlashEnded(UAnimMontage* AnimMontage, bool bArg);

	UFUNCTION()
	void OnSlashOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void PredictApplyDamage(AActor* TargetActor);

	UFUNCTION(Server, Reliable)
	void ServerRPCRequestDamage(AActor* TargetActor, float Amount);

	UFUNCTION(Server, Reliable)
	void ServerRPCPlaySlashStartAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlaySlashStartAnimation();

	UFUNCTION(Server, Reliable)
	void ServerRPCPlayReSlashAnimation(bool bIsLeftSlash);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlaySlashAnimation(bool bIsLeftSlash);

	UAnimInstance* GetSourceAnimInstance();

	bool IsValidTarget(AActor* TargetActor);

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<UGameplayEffect> SlashDamageGE;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UAnimMontage> SlashMontage;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Distance = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Angle = 240.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SlashTime = 0.1875f;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bShowDebug:1 = false;

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTagContainer DeactivateGameplayTags;

	TSoftObjectPtr<ASMBassCharacter> SourceCharacter;
	TSoftObjectPtr<USceneComponent> SourceSlashColliderRootComponent;
	TSoftObjectPtr<UCapsuleComponent> SourceSlashColliderComponent;

	TArray<TSoftObjectPtr<AActor>> DetectedActors;
	float SlashSpeed = 0.0f;
	float HalfAngle = 0.0f;
	uint32 bIsSlashing:1 = false;
	uint32 bNeedUpdateSlashing:1 = false;
};
