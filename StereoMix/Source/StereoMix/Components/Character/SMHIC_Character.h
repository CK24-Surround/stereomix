// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HoldInteraction/SMHoldInteractionComponent.h"
#include "Data/SMNoiseBreakData.h"
#include "SMHIC_Character.generated.h"

class ASMPlayerCharacterBase;
class USMAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMHIC_Character : public USMHoldInteractionComponent
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	void SetASC(USMAbilitySystemComponent* NewASC);

	virtual bool CanBeHeld(AActor* Instigator) const override;

	virtual void OnHeld(AActor* Instigator) override;

	virtual void OnReleasedFromHold(AActor* Instigator) override;

	virtual bool ShouldApplyDamageFromNoiseBreak() const override { return true; }

	virtual bool ShouldCaptureTilesFromNoiseBreak() const override { return true; }

	virtual void OnNoiseBreakStarted(AActor* Instigator) override;

	virtual void OnNoiseBreakApplied(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakApplied(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakApplied(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	/** 오너가 잡고 있는 대상을 반환합니다. */
	FORCEINLINE AActor* GetActorIAmHolding() const { return IAmHoldingActor.Get(); }

	void SetActorIAmHolding(AActor* NewIAmHoldingActor);

	void EmptyHeldMeCharacterList();

	/**
	 * 잡힌 상태에서 풀려나야할때 호출합니다.
	 * 외부에서도 사용할 수 있습니다. 따로 추가적인 애니메이션 같은 비주얼적인 효과 없이 그저 디태치만 수행합니다.
	 * @param TargetOptionalLocation 타겟의 위치를 설정합니다. nullptr을 넘길경우 자동으로 지정합니다.
	 */
	void ReleasedFromBeingHeld(AActor* TargetActor, const TOptional<FVector>& TargetOptionalLocation = TOptional<FVector>());

	/** 잡고 있는 대상이 파괴되는 경우 예외처리를 담는 함수입니다. */
	UFUNCTION()
	void OnDestroyedIAmHoldingActor(AActor* DestroyedActor);

	/** 잡을때 이벤트입니다. */
	FOnHoldStateChangedDelegate OnHoldStateEnrty;

	/** 잡기를 놓을때 이벤트입니다.*/
	FOnHoldStateChangedDelegate OnHoldStateExit;

protected:
	UFUNCTION()
	void OnRep_IAmHoldingActor();

	void InternalHolded(AActor* TargetActor);

	void NoiseBreaked();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayNoiseBreakedSFX();

	/** 자신이 잡고 있는 액터입니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_IAmHoldingActor")
	TWeakObjectPtr<AActor> IAmHoldingActor;

	/** 한 캐릭터에게 여러번 잡히지 않도록 자신을 잡았던 액터들을 담아둡니다. 서버에서만 유효합니다. */
	TArray<TWeakObjectPtr<AActor>> HeldMeActors;

	UPROPERTY()
	TObjectPtr<ASMPlayerCharacterBase> SourceCharacter;

	UPROPERTY()
	TObjectPtr<USMAbilitySystemComponent> SourceASC;
};
