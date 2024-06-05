// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "SMCharacter.h"
#include "Interfaces/SMCatchInteractionInterface.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "SMPlayerCharacter.generated.h"

class ASMGameState;
class UNiagaraComponent;
class UNiagaraSystem;
class USMCatchInteractionComponent_Character;
class UWidgetComponent;
class USMTeamComponent;
class USMAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;
class ASMPlayerCharacter;
class ASMGamePlayerController;
struct FInputActionValue;
class USphereComponent;
class UCameraComponent;
class USpringArmComponent;
class USMDesignData;
class USMWidgetComponent;

UENUM(BlueprintType)
enum class EActiveAbility : uint8
{
	None,
	Launch,
	Catch,
	Smash
};

UENUM(BlueprintType)
enum class ECharacterMoveTrailState : uint8
{
	None,
	Default,
	Catch,
	Immune
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLandedSignature, ASMPlayerCharacter* /*LandedCharacter*/);

UCLASS()
class STEREOMIX_API ASMPlayerCharacter : public ASMCharacter, public IAbilitySystemInterface, public ISMTeamInterface, public ISMCatchInteractionInterface, public ISMDamageInterface
{
	GENERATED_BODY()

public:
	ASMPlayerCharacter();

public:
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Controller() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void OnRep_PlayerState() override;

// Camera Section
protected:
	// 카메라 관련 데이터를 초기화합니다.
	void InitCamera();

	void UpdateCameraLocation();

protected:
	/** 카메라 이동에 사용될 마우스 인식 범위입니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Camera")
	float CameraMoveMouseThreshold = 1150.0f;

	/** 카메라의 최대 이동 거리입니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Camera")
	float CameraMoveMaxDistance = 250.0f;
// Camera Section

// ~Component Section
public:
	FORCEINLINE virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

	virtual USMCatchInteractionComponent* GetCatchInteractionComponent() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Component|HitBox")
	TObjectPtr<USphereComponent> HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Component|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Component|Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "Component|Listener")
	TObjectPtr<USceneComponent> ListenerComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component|Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component|CatchInteraction")
	TObjectPtr<USMCatchInteractionComponent_Character> CatchInteractionComponent;

	UPROPERTY(VisibleAnywhere, Category = "UI|PlayerState")
	TObjectPtr<USMWidgetComponent> CharacterStateWidgetComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX|DefaultMove")
	TObjectPtr<UNiagaraComponent> DefaultMoveTrailFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX|CatchMove")
	TObjectPtr<UNiagaraComponent> CatchMoveTrailFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX|ImmuneMove")
	TObjectPtr<UNiagaraComponent> ImmuneMoveTrailFXComponent;
// ~Component Section

// ~Data Section
public:
	const TArray<TObjectPtr<UMaterialInterface>>& GetOriginalMaterials() { return OriginalMaterials; }
	UMaterialInterface* GetOriginalOverlayMaterial() { return OriginalOverlayMaterial; }

protected:
	UPROPERTY()
	TObjectPtr<const USMDesignData> DesignData;

	/** 캐릭터의 원본 머티리얼입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Design|Material")
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

	/** 캐릭터의 원본 오버레이 머티리얼입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Design|Material")
	TObjectPtr<UMaterialInterface> OriginalOverlayMaterial;
// ~Data Section

// ~Caching Section
protected:
	UPROPERTY()
	TObjectPtr<ASMGamePlayerController> CachedSMPlayerController;
// ~Caching Section

// ~GAS Section
protected:
	// GAS에서 사용할 액티브 GA들을 바인드합니다.
	void SetupGASInputComponent();

	// ASC를 초기화하고 기본 GA들을 부여합니다.
	void InitASC();

	void GiveDefaultAbilities();

	// 액티브 GA와 바인드 된 함수입니다. 입력할때 트리거됩니다.
	void GAInputPressed(EActiveAbility InInputID);

	// 액티브 GA와 바인드 된 함수입니다. 놓을때 트리거됩니다.
	void GAInputReleased(EActiveAbility InInputID);

protected:
	UPROPERTY()
	TWeakObjectPtr<USMAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|GA")
	TMap<EActiveAbility, TSubclassOf<UGameplayAbility>> DefaultActiveAbilities;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|GA")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|GE")
	TSubclassOf<UGameplayEffect> ForInitGE;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer LockAimTags;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer LockMovementTags;
// ~GAS Section

// ~Movement Section
public:
	/** 현재 마우스커서가 위치한 곳의 좌표를 반환합니다. 기본값은 플레이어 캐릭터의 중심을 기준으로 하는 평면으로 계산됩니다. */
	FVector GetCursorTargetingPoint(bool bIsZeroBasis = false);

	void SetMaxWalkSpeed(float InSpeed);

	/** 땅에 박히는 것을 방지하기 위한 코드입니다. 서버에서 호출되어야합니다. */
	UFUNCTION(Server, Reliable)
	void ServerRPCPreventGroundEmbedding();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetLocation(const FVector_NetQuantize10 InLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetYawRotation(float InYaw);

	/** 어태치할 경우 루트컴포넌트를 기준으로 어태치 되다보니 메시와는 어긋나게됩니다. 이때 오프셋을 맞춰주는 함수입니다.*/
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCApplyAttachOffset();

public:
	FOnLandedSignature OnLanded;

protected:
	void Move(const FInputActionValue& InputActionValue);

	void FocusToCursor();

	UFUNCTION()
	void OnRep_MaxWalkSpeed();

	void OnChangeMoveSpeed(const FOnAttributeChangeData& OnAttributeChangeData);

	virtual void Landed(const FHitResult& Hit) override;

protected:
	// 이동속도 리플리케이션을 위한 변수로 직접 수정되어서는 안됩니다. SetMaxWalkSpeed를 사용해 수정해주세요.
	UPROPERTY(ReplicatedUsing = "OnRep_MaxWalkSpeed")
	float MaxWalkSpeed;
// ~Movement Section

// ~Widget Section
public:
	/** 캐릭터 상태 위젯의 가시성을 조절합니다. */
	UFUNCTION(NetMulticast, Unreliable)
	void SetCharacterStateVisibility(bool bEnable);

	/** 다른 클라이언트들에게 인디케이터를 추가합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAddScreenIndicatorToSelf(AActor* TargetActor);

	/** 다른 클라이언트들에게서 인디케이터를 제거합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCRemoveScreenIndicatorToSelf(AActor* TargetActor);

	/** 해당 클라이언트에서만 인디케이터를 제거합니다. */
	UFUNCTION(Client, Reliable)
	void ClientRPCRemoveScreendIndicatorToSelf(AActor* TargetActor);
// ~Widget Section

// ~State Section
public:
	void SetEnableCollision(bool bInEnableCollision);

	UFUNCTION()
	void OnRep_EnableCollision();

	void SetUseControllerRotation(bool bInUseControllerRotation);

	UFUNCTION()
	void OnRep_UseControllerRotation();

	void SetEnableMovement(bool bInEnableMovementMode);

	UFUNCTION()
	void OnRep_EnableMovement();

protected:
	/* 무언가를 잡을때 호출됩니다. */
	void OnCatch();

	/* 무언가를 놓거나 놓칠때 호출됩니다. */
	void OnCatchRelease();

protected:
	UPROPERTY(ReplicatedUsing = "OnRep_EnableCollision")
	uint32 bEnableCollision:1 = true;

	UPROPERTY(ReplicatedUsing = "OnRep_UseControllerRotation")
	uint32 bUseControllerRotation:1 = true;

	UPROPERTY(ReplicatedUsing = "OnRep_EnableMovement")
	uint32 bEnableMovement:1 = true;
// ~State Section

// ~Damage Section
public:
	FORCEINLINE virtual AActor* GetLastAttackInstigator() override { return LastAttackInstigator.Get(); };

	FORCEINLINE virtual void SetLastAttackInstigator(AActor* NewStunInstigator) override { LastAttackInstigator = NewStunInstigator; }

protected:
	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> LastAttackInstigator;
// ~Damage Section

protected:
	UFUNCTION()
	void OnTeamChangeCallback();

// ~Animation Section
public:
	bool bAmICatching();
// ~Animation Section

// ~Catch Section
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design|Socket")
	FName CatchSocket = TEXT("CatchSlot");
// ~Catch Section

// ~Trail Section
public:
	ECharacterMoveTrailState GetCharacterMoveTrailState() { return CharacterMoveTrailState; }

	void SetCharacterMoveTrailState(ECharacterMoveTrailState NewCharacterMoveTrailState);

	UFUNCTION()
	void OnRep_CharacterMoveTrailState();

protected:
	void DeactivateAllMoveTrail();

	void ActivateDefaultMoveTrail();

	void ActivateCatchMoveTrail();

	void ActivateImmuneMoveTrail();

protected:
	UPROPERTY(ReplicatedUsing = "OnRep_CharacterMoveTrailState")
	ECharacterMoveTrailState CharacterMoveTrailState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design|Socket")
	FName TrailSocket = TEXT("Trail-Point");

	UPROPERTY(EditAnywhere, Category = "Design|FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> DefaultMoveTrailFX;

	UPROPERTY(EditAnywhere, Category = "Design|FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> CatchMoveTrailFX;

	UPROPERTY(EditAnywhere, Category = "Design|FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> ImmuneMoveTrailFX;
// ~Trail Section

// Temp Section
// TODO: 임시 함수입니다. 스매시 엔드 포인트를 그려줍니다. 이 캐릭터 클래스는 모든 캐릭터의 부모 클래스이기에 스매시 엔드포인트를 그려주는 함수가 여기 존재해서는 안됩니다. 단지 느낌을 보기 위한 프로토타이핑입니다.
protected:
	UPROPERTY(VisibleAnywhere, Category = "Design|Trajectory")
	TObjectPtr<UNiagaraComponent> TrajectoryFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "Design|Tragectory")
	TObjectPtr<UStaticMeshComponent> TrajectoryMeshComponent;

protected:
	void DrawSmashEndPoint();
// Temp Section
};
