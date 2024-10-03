// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "FMODEvent.h"

#include "SMPreviewCharacter.generated.h"

class ASMPlayerState;

UCLASS()
class STEREOMIX_API ASMPreviewCharacter : public AActor
{
	GENERATED_BODY()

public:
	ASMPreviewCharacter();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual void InitPlayerState(ASMPlayerState* PlayerState);

	ASMPlayerState* GetOwningPlayerState() const { return OwningPlayerState; }

	ESMTeam GetTeam() const { return Team; }

	ESMCharacterType GetCharacterType() const { return CharacterType; }

	void PlaySelectAnimation() const;

protected:
	virtual void BeginPlay() override;

private:
	// =============================================================================
	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess))
	TObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess))
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;


	// =============================================================================
	// Variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	ESMTeam Team;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	ESMCharacterType CharacterType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	TObjectPtr<USkeletalMesh> CharacterMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	TSubclassOf<UAnimInstance> CharacterAnimInstanceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> CharacterSelectAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	TObjectPtr<UTextureRenderTarget2D> CharacterRenderTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	TObjectPtr<UFMODEvent> CharacterSelectSound;

	UPROPERTY()
	ASMPlayerState* OwningPlayerState;
};
