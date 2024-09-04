// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMHoldInteractionComponent.h"
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

	void InitASC(USMAbilitySystemComponent* NewASC);

	virtual bool CanHolded(AActor* TargetActor) const override;

	virtual void OnHolded(AActor* TargetActor) override;

	virtual void OnHoldedReleased(AActor* TargetActor, bool bIsStunTimeOut) override;

	virtual void OnNoiseBreakActionStarted(ASMElectricGuitarCharacter* Instigator) override;

	virtual void OnNoiseBreakActionStarted(ASMPianoCharacter* Instigator) override;

	virtual void OnNoiseBreakActionStarted(ASMBassCharacter* Instigator) override;

	virtual void OnNoiseBreakActionPerformed(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakActionPerformed(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakActionPerformed(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	FORCEINLINE AActor* GetActorIAmHolding() { return IAmHoldingActor.Get(); }

	void SetActorIAmHolding(AActor* NewIAmHoldingActor);

	void EmptyHoldedMeCharacterList();

	/**
	* 타겟으로부터 잡기상태를 탈출합니다. 타겟이 null이면 타겟과 관련된 처리만 제외하고 모두 처리됩니다.
	* 외부에서도 사용할 수 있습니다. 따로 추가적인 애니메이션 같은 비주얼적인 효과 없이 그저 디태치만 수행합니다.
	*/
	void HoldedReleased(AActor* TargetActor);

	/** 잡고 있는 대상이 파괴되는 경우 예외처리를 담는 함수입니다. */
	UFUNCTION()
	void OnDestroyedIAmHoldingActor(AActor* DestroyedActor);

	/** 잡을때 브로드캐스트합니다. */
	FOnCatchSignature OnCatch;

	/** 잡기를 놓을때 브로드캐스트합니다.*/
	FOnCatchSignature OnCatchRelease;

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

	/** 한 캐릭터에게 여러번 잡히지 않도록 자신을 잡았던 캐릭터들을 담아둡니다. 서버에서만 유효합니다. */
	TArray<TWeakObjectPtr<ASMPlayerCharacterBase>> HoldedMeCharcters;

	UPROPERTY()
	TObjectPtr<ASMPlayerCharacterBase> SourceCharacter;

	UPROPERTY()
	TObjectPtr<USMAbilitySystemComponent> SourceASC;
};
