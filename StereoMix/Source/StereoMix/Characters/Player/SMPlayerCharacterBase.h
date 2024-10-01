// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMCharacterBase.h"
#include "Interfaces/SMTeamInterface.h"
#include "Data/SMActiveAbility.h"
#include "Data/SMCharacterType.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "SMPlayerCharacterBase.generated.h"

class ASMNoteBase;
class ASMWeaponBase;
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

	virtual void SetActorHiddenInGame(bool bNewHidden) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template<typename T>
	T* GetAbilitySystemComponent() const { return Cast<T>(GetAbilitySystemComponent()); }

	virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; };

	virtual ESMTeam GetTeam() const override;

	UFUNCTION(BlueprintCallable)
	const USMPlayerCharacterDataAsset* GetDataAsset() { return DataAsset; }

	template<typename T>
	const T* GetDataAsset() { return Cast<T>(DataAsset); }

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override;

	template<typename T>
	T* GetHoldInteractionComponent() { return Cast<T>(HIC); }

	/** 예측적으로 데미지를 먼저 UI에 반영해야할 때 사용합니다.*/
	void PredictHPChange(float Amount);

	/** 현재 커서가 위치한 곳의 좌표를 반환합니다. 양수 값의 사거리를 매개변수로 넘겨주면 사거리 내의 좌표를 반환합니다.
	 * bIsZeroBasis가 true면 캐릭터의 바닥을 기준으로, false면 캐릭터의 중심을 기준으로 계산합니다.
	 */
	FVector GetLocationFromCursor(bool bUseZeroBasis = false, float MaxDistance = -1.0f);

	/** 현재 커서가 위치한 곳의 타일좌표를 가져옵니다. 양수 값의 사거리를 매개변수로 넘겨주면 사거리 내의 타일의 좌표를 가져옵니다. 실패시 false를 반환합니다. */
	bool GetTileLocationFromCursor(FVector& OutTileLocation, float MaxDistance = -1.0f);

	/** 액터를 숨깁니다. 리플리케이트 됩니다. */
	UFUNCTION(Server, Reliable)
	void ServerSetActorHiddenInGame(bool bNewIsActorHidden);

	/** 액터의 콜리전을 설정합니다. 리플리케이트 됩니다.*/
	UFUNCTION(Server, Reliable)
	void ServerSetCollisionEnabled(bool bNewIsCollisionEnabled);

	/** 움직임을 잠급니다. 리플리케이트 됩니다. */
	UFUNCTION(Server, Reliable)
	void ServerSetMovementEnabled(bool bNewIsMovementEnabled);

	/** 컨트롤 로테이션을 기준으로 캐릭터를 회전시킬지 여부를 나타냅니다. 서버에서 호출되어야합니다. */
	UFUNCTION(Server, Reliable)
	void ServerSetUseControllerRotation(bool bNewUseControllerRotation);

	/** 캐릭터 상태 위젯의 가시성을 조절합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCharacterStateVisibility(bool bNewVisibility);

	/** 다른 클라이언트들에게 인디케이터를 추가합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAddScreenIndicatorToSelf(AActor* TargetActor);

	/** 다른 클라이언트들에게서 인디케이터를 제거합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCRemoveScreenIndicatorToSelf(AActor* TargetActor);

	/** 해당 클라이언트에서만 인디케이터를 제거합니다. */
	UFUNCTION(Client, Reliable)
	void ClientRPCRemoveScreendIndicatorToSelf(AActor* TargetActor);

	virtual AActor* GetLastAttacker() const override { return LastAttacker.Get(); }

	virtual void SetLastAttacker(AActor* NewAttacker) override { LastAttacker = NewAttacker; }

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;

	virtual bool CanIgnoreAttack() const override;

	virtual bool IsObstacle() override { return false; }

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetYawRotation(float InYaw);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetLocation(const FVector_NetQuantize10& NewLocation);

	virtual void Landed(const FHitResult& Hit) override;

	virtual void FocusToCursor();

	UFUNCTION(Client, Reliable)
	void ClientRPCCharacterPushBack(FVector_NetQuantize10 Velocity);

	UFUNCTION(Server, Reliable)
	void ServerRPCCharacterPushBack(FVector_NetQuantize10 Velocity);

	void AddMoveSpeed(float MoveSpeedMultiplier, float Duration);

	UFUNCTION(Client, Reliable)
	void ClientRPCAddMoveSpeed(float MoveSpeedMultiplier, float Duration);

	UNiagaraComponent* GetNoiseBreakIndicator() const { return NoiseBreakIndicatorComponent; }

	ASMWeaponBase* GetWeapon() const { return Weapon; }

	template<typename T>
	T* GetWeapon() const { return Cast<T>(GetWeapon()); }

	UFUNCTION()
	void OnTeamChanged();

	ESMCharacterType GetCharacterType() { return CharacterType; }

	ASMNoteBase* GetNote() { return Note; }

	/** 노트 상태인지 여부를 반환합니다. */
	bool IsNoteState() { return bIsNoteState; }

	/** True면 음표 상태로, False면 기본 상태로 설정합니다. 서버에서 호출해야합니다.*/
	UFUNCTION(Server, Reliable)
	void ServerSetNoteState(bool bNewIsNote);

	TArray<UMaterialInterface*> GetOriginalMaterials() { return OriginalMaterials; }

	UMaterialInterface* GetOriginalOverlayMaterial() { return OriginalOverlayMaterial; }

	FOnCharacterLandedSignature OnCharacterLanded;

protected:
	void Move(const FInputActionValue& InputActionValue);

	void InitASC();

	void GiveDefaultAbilities();

	void InitStat();

	void GAInputPressed(EActiveAbility InInputID);

	void GAInputReleased(EActiveAbility InInputID);

	/** 현재 마우스커서가 위치한 곳의 좌표를 반환합니다.
	 * bIsZeroBasis가 true면 캐릭터의 바닥을 기준으로, false면 캐릭터의 중심을 기준으로 계산합니다. */
	FVector GetCursorTargetingPoint(bool bUseZeroBasis = false);

	void UpdateCameraLocation();

	void UpdateFocusToCursor();

	void BindCharacterStateWidget(USMUserWidget_CharacterState* CharacterStateWidget);

	UFUNCTION()
	void OnRep_bIsActorHidden();

	UFUNCTION()
	void OnRep_bIsCollisionEnabled();

	UFUNCTION()
	void OnRep_bIsMovementEnabled();

	UFUNCTION()
	void OnRep_bUseControllerRotation();

	UFUNCTION(Server, Reliable)
	void ServerRPCAddMoveSpeed(float MoveSpeedMultiplier, float Duration);

	UFUNCTION()
	void OnRep_bIsNoteState();

	virtual void OnHoldStateEntry() {}

	virtual void OnHoldStateExit() {}

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

	UPROPERTY(Replicated)
	TObjectPtr<ASMWeaponBase> Weapon;

	UPROPERTY(VisibleAnywhere, Category = "Note")
	TObjectPtr<USceneComponent> NoteSlotComponent;

	UPROPERTY(Replicated)
	TObjectPtr<ASMNoteBase> Note;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	TObjectPtr<UWidgetComponent> CharacterStateWidgetComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> NoiseBreakIndicatorComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> DefaultMoveTrailFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> CatchMoveTrailFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> ImmuneMoveTrailFXComponent;

	TWeakObjectPtr<ASMGamePlayerController> SMPlayerController;

	UPROPERTY()
	TObjectPtr<USMHIC_Character> HIC;

	/** 공격을 무시하는 태그들입니다. */
	UPROPERTY(EditAnywhere, Category = "Desing|GAS|Tag")
	FGameplayTagContainer IgnoreAttackTags;

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
	float CameraMoveMouseThreshold = 1400.0f;

	/** 카메라의 최대 이동 거리입니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Camera")
	float CameraMoveMaxDistance = 500.0f;

	UPROPERTY(ReplicatedUsing = "OnRep_bIsActorHidden")
	uint32 bIsActorHidden:1 = false;

	UPROPERTY(ReplicatedUsing = "OnRep_bIsCollisionEnabled")
	uint32 bIsCollisionEnabled:1 = true;

	UPROPERTY(ReplicatedUsing = "OnRep_bIsMovementEnabled")
	uint32 bIsMovementEnabled:1 = true;

	UPROPERTY(ReplicatedUsing = "OnRep_bUseControllerRotation")
	uint32 bUseControllerRotation:1 = true;

	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> LastAttacker;

	ESMCharacterType CharacterType = ESMCharacterType::None;

	UPROPERTY(ReplicatedUsing = "OnRep_bIsNoteState")
	uint32 bIsNoteState:1 = false;
};
