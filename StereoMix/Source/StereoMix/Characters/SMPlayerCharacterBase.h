// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "SMCharacterBase.h"
#include "Interfaces/SMTeamInterface.h"
#include "Data/SMActiveAbility.h"
#include "SMPlayerCharacterBase.generated.h"

class ASMGamePlayerController;
class USMAbilitySystemComponent;
class USMPlayerCharacterDataAsset;
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class UNiagaraComponent;
class USMUserWidget_CharacterState;

UENUM(BlueprintType)
enum class EMoveTrailState : uint8
{
	Default,
	Catch,
	Immune
};

UCLASS(Abstract)
class STEREOMIX_API ASMPlayerCharacterBase : public ASMCharacterBase, public IAbilitySystemInterface, public ISMTeamInterface
{
	GENERATED_BODY()

public:
	ASMPlayerCharacterBase();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnRep_Controller() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; };

	virtual ESMTeam GetTeam() const override;

	/** 현재 마우스커서가 위치한 곳의 좌표를 반환합니다.
	 * 기본값은 플레이어 캐릭터의 중심을 기준으로 하는 평면으로 계산됩니다. 만약 bIsZeroBasis가 true라면 캐릭터가 서있는 바닥을 기준으로 하는 평면으로 계산됩니다. */
	FVector GetCursorTargetingPoint(bool bIsZeroBasis = false);

protected:
	void Move(const FInputActionValue& InputActionValue);

	void InitASC();

	void GiveDefaultAbilities();

	void GAInputPressed(EActiveAbility InInputID);

	void GAInputReleased(EActiveAbility InInputID);

	void UpdateCameraLocation();

	void FocusToCursor();

	void BindCharacterStateWidget(USMUserWidget_CharacterState* CharacterStateWidget);

	UPROPERTY(EditAnywhere, Category = "Design|Data")
	TObjectPtr<const USMPlayerCharacterDataAsset> DataAsset;

	TWeakObjectPtr<USMAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, Category = "HitBox")
	TObjectPtr<UCapsuleComponent> HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "Listener")
	TObjectPtr<USceneComponent> ListenerComponent;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	TObjectPtr<UWidgetComponent> CharacterStateWidgetComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> DefaultMoveTrailFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> CatchMoveTrailFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> ImmuneMoveTrailFXComponent;

	TWeakObjectPtr<ASMGamePlayerController> SMPlayerController;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer LockAimTags;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer LockMovementTags;

	/** 캐릭터의 원본 머티리얼입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Design|Material")
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

	/** 캐릭터의 원본 오버레이 머티리얼입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Design|Material")
	TObjectPtr<UMaterialInterface> OriginalOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design|Socket")
	FName TrailSocket = TEXT("Trail-Point");

	/** 카메라 이동에 사용될 마우스 인식 범위입니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Camera")
	float CameraMoveMouseThreshold = 1150.0f;

	/** 카메라의 최대 이동 거리입니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Camera")
	float CameraMoveMaxDistance = 250.0f;
};
