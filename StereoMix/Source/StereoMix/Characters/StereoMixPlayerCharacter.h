// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "StereoMixCharacter.h"
#include "StereoMixPlayerCharacter.generated.h"

class UStereoMixAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;
class AStereoMixPlayerController;
struct FInputActionValue;
class USphereComponent;
class UCameraComponent;
class USpringArmComponent;
class UStereoMixDesignData;

UENUM(BlueprintType)
enum class EActiveAbility : uint8
{
	None,
	Launch,
	Catch
};

UCLASS()
class STEREOMIX_API AStereoMixPlayerCharacter : public AStereoMixCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AStereoMixPlayerCharacter();

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

protected:
	// 카메라 관련 데이터를 초기화합니다.
	void InitCamera();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Component|HitBox")
	TObjectPtr<USphereComponent> HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Component|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Component|Camera")
	TObjectPtr<UCameraComponent> Camera;

// ~Data Section
protected:
	UPROPERTY()
	TObjectPtr<const UStereoMixDesignData> DesignData;
// ~Data Section

// ~Caching Section
protected:
	UPROPERTY()
	TObjectPtr<AStereoMixPlayerController> CachedStereoMixPlayerController;
// ~Caching Section

// ~GAS Section
protected:
	// GAS에서 사용할 액티브 GA들을 바인드합니다.
	void SetupGASInputComponent();

	// ASC를 초기화하고 기본 GA들을 부여합니다.
	void InitASC();

	// 액티브 GA와 바인드 된 함수입니다. 입력할때 트리거됩니다.
	void GAInputPressed(EActiveAbility InInputID);

	// 액티브 GA와 바인드 된 함수입니다. 놓을때 트리거됩니다.
	// 액티브 GA와 바인드 된 함수입니다. 놓을때 트리거됩니다.
	void GAInputReleased(EActiveAbility InInputID);

	// 부착되는 태그가 변경될때마다 호출됩니다. 서버, 클라이언트 모두 호출됩니다.
	void OnChangedTag(const FGameplayTag& Tag, bool TagExists);

	void OnAddStunTag();
	void OnRemoveStunTag();

	void OnAddCaughtTag();
	void OnRemoveCaughtTag();

protected:
	UPROPERTY()
	TSoftObjectPtr<UStereoMixAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = "GAS|GA")
	TMap<EActiveAbility, TSubclassOf<UGameplayAbility>> DefaultActiveAbilities;

	UPROPERTY(EditAnywhere, Category = "GAS|GA")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> ForInitGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TArray<TSubclassOf<UGameplayEffect>> StunEndedGEs;

	UPROPERTY(EditAnywhere, Category = "GAS|Tag")
	FGameplayTagContainer LockAimTags;

	UPROPERTY(EditAnywhere, Category = "GAS|Tag")
	FGameplayTagContainer LockMovementTags;
// ~GAS Section

// ~Movement Section
public:
	// 현재 마우스커서가 위치한 곳의 좌표를 반환합니다.
	FVector GetCursorTargetingPoint();

	void SetMaxWalkSpeed(float InSpeed);

protected:
	void Move(const FInputActionValue& InputActionValue);

	void FocusToCursor();

	UFUNCTION()
	void OnRep_MaxWalkSpeed();

protected:
	// 이동속도 리플리케이션을 위한 변수로 직접 수정되어서는 안됩니다. SetMaxWalkSpeed를 사용해 수정해주세요.
	UPROPERTY(ReplicatedUsing = "OnRep_MaxWalkSpeed")
	float MaxWalkSpeed;
// ~Movement Section
};
