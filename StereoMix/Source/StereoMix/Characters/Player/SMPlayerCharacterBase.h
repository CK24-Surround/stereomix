// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "Characters/SMCharacterBase.h"
#include "Interfaces/SMTeamInterface.h"
#include "Data/SMActiveAbility.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "SMPlayerCharacterBase.generated.h"

class USMHIC_Character;
class ASMGamePlayerController;
class USMAbilitySystemComponent;
class USMPlayerCharacterDataAsset;
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class UNiagaraComponent;
class USMUserWidget_CharacterState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterLandedSignature, ASMPlayerCharacterBase* /*LandedCharacter*/);

UENUM(BlueprintType)
enum class EMoveTrailState : uint8
{
	Default,
	Catch,
	Immune
};

UCLASS(Abstract)
class STEREOMIX_API ASMPlayerCharacterBase : public ASMCharacterBase, public IAbilitySystemInterface, public ISMTeamInterface, public ISMHoldInteractionInterface, public ISMDamageInterface
{
	GENERATED_BODY()

public:
	ASMPlayerCharacterBase(const FObjectInitializer& ObjectInitializer);

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

	const USMPlayerCharacterDataAsset* GetDataAsset() { return DataAsset; }

	template<typename T>
	const T* GetDataAsset() { return Cast<T>(DataAsset); }

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override;

	template<typename T>
	T* GetHoldInteractionComponent() { return Cast<T>(HIC); }

	/** 예측적으로 데미지를 먼저 UI에 반영해야할 때 사용합니다.*/
	void PredictHPChange(float Amount);

	/** 현재 마우스커서가 위치한 곳의 좌표를 반환합니다.
	 * 기본값은 플레이어 캐릭터의 중심을 기준으로 하는 평면으로 계산됩니다. 만약 bIsZeroBasis가 true라면 캐릭터가 서있는 바닥을 기준으로 하는 평면으로 계산됩니다. */
	FVector GetCursorTargetingPoint(bool bIsZeroBasis = false);

	/** 액터를 숨깁니다. 서버에서 호출되어야합니다. */
	void SetCharacterHidden(bool bIsEnable);

	/** 액터의 콜리전을 설정합니다. 서버에서 호출되어야합니다.*/
	void SetCollisionEnable(bool bIsEnable);

	/** 움직임을 잠급니다. 서버에서 호출되어야합니다. */
	void SetMovementEnable(bool bIsEnable);

	/** 컨트롤 로테이션을 기준으로 캐릭터를 회전시킬지 여부를 나타냅니다. */
	void SetUseControllerRotation(bool bNewUseControllerRotation);

	/** 캐릭터 상태 위젯의 가시성을 조절합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void SetCharacterStateVisibility(bool bNewVisibility);

	/** 다른 클라이언트들에게 인디케이터를 추가합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAddScreenIndicatorToSelf(AActor* TargetActor);

	/** 다른 클라이언트들에게서 인디케이터를 제거합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCRemoveScreenIndicatorToSelf(AActor* TargetActor);

	/** 해당 클라이언트에서만 인디케이터를 제거합니다. */
	UFUNCTION(Client, Reliable)
	void ClientRPCRemoveScreendIndicatorToSelf(AActor* TargetActor);

	virtual AActor* GetLastAttackInstigator() const override { return LastAttackInstigator.Get(); }

	virtual void SetLastAttackInstigator(AActor* NewStunInstigator) override { LastAttackInstigator = NewStunInstigator; }

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetYawRotation(float InYaw);

	UFUNCTION(NetMulticast, Reliable)
	void MulitcastRPCSetLocation(const FVector_NetQuantize10& NewLocation);

	virtual void Landed(const FHitResult& Hit) override;

	virtual void FocusToCursor();

	UFUNCTION(Client, Reliable)
	void ClientRPCCharacterPushBack(FVector_NetQuantize10 Velocity);

	UFUNCTION(Server, Reliable)
	void ServerRPCCharacterPushBack(FVector_NetQuantize10 Velocity);

	void AddMoveSpeed(float MoveSpeedMultiplier, float Duration);

	UFUNCTION(Client, Reliable)
	void ClientRPCAddMoveSpeed(float MoveSpeedMultiplier, float Duration);

	FOnCharacterLandedSignature OnCharacterLanded;

protected:
	void Move(const FInputActionValue& InputActionValue);

	void InitASC();

	void GiveDefaultAbilities();

	void GAInputPressed(EActiveAbility InInputID);

	void GAInputReleased(EActiveAbility InInputID);

	void UpdateCameraLocation();

	void UpdateFocusToCursor();

	void BindCharacterStateWidget(USMUserWidget_CharacterState* CharacterStateWidget);

	UFUNCTION()
	void OnRep_bIsHiddenCharacter();

	UFUNCTION()
	void OnRep_bActivateCollision();

	UFUNCTION()
	void OnRep_bEnableMovement();

	UFUNCTION()
	void OnRep_bUseControllerRotation();

	UFUNCTION(Server, Reliable)
	void ServerRPCAddMoveSpeed(float MoveSpeedMultiplier, float Duration);

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

	UPROPERTY()
	TObjectPtr<USMHIC_Character> HIC;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer LockAimTags;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer LockMovementTags;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer PushBackImmuneTags;

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

	UPROPERTY(ReplicatedUsing = "OnRep_bIsHiddenCharacter")
	uint32 bIsHiddenCharacter:1 = false;

	UPROPERTY(ReplicatedUsing = "OnRep_bActivateCollision")
	uint32 bActivateCollision:1 = true;

	UPROPERTY(ReplicatedUsing = "OnRep_bEnableMovement")
	uint32 bEnableMovement:1 = true;

	UPROPERTY(ReplicatedUsing = "OnRep_bUseControllerRotation")
	uint32 bUseControllerRotation:1 = true;

	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> LastAttackInstigator;
};
