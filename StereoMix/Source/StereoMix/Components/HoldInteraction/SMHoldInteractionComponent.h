// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMNoiseBreakData.h"
#include "SMHoldInteractionComponent.generated.h"

class ASMElectricGuitarCharacter;
class ASMPianoCharacter;
class ASMBassCharacter;
class UGameplayEffect;

DECLARE_MULTICAST_DELEGATE(FOnHoldStateChangedDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMHoldInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMHoldInteractionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	/** 자신을 잡고 있는 액터를 반환합니다. 서버에서만 유효합니다. */
	FORCEINLINE virtual AActor* GetActorHoldingMe() const { return HoldingMeActor.Get(); }

	/** 자신을 잡고 있는 액터를 할당합니다. 서버에서만 유효합니다. */
	virtual void SetActorHoldingMe(AActor* Instigator);

	/** 잡을 수 있는지 여부를 반환하도록 구현해야합니다. 서버에서만 유효합니다. */
	virtual bool CanBeHeld(AActor* Instigator) const { return false; }

	/** 시전자에게 잡힐때 필요한 로직을 구현해야합니다. 성공 여부를 반환합니다. 서버에서 호출됩니다. */
	virtual void OnHeld(AActor* Instigator) {}

	/** 시전자로부터 잡히기가 풀릴때 필요한 로직을 구현해야합니다. 성공 여부를 반환합니다. 서버에서 호출됩니다. */
	virtual void OnReleasedFromHold(AActor* Instigator) {}

	/** 노이즈 브레이크 당했을때 데미지를 발생시키는지 여부를 반환합니다. */
	virtual bool ShouldApplyDamageFromNoiseBreak() const { return false; }

	/** 노이즈 브레이크 당했을때 타일 점령을 발생시키는지 여부를 반환합니다. */
	virtual bool ShouldCaptureTilesFromNoiseBreak() const { return false; }

	/** 노이즈 브레이크 당하기 시작될 때 호출됩니다. 이 때 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakStarted(AActor* Instigator) {}

	/** 일렉기타에게 노이즈 브레이크를 적용당할 때 호출됩니다. 이때 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakApplied(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) {}

	/** 피아노에게 노이즈 브레이크를 적용당할 때 호출됩니다. 이때 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakApplied(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) {}

	/** 베이스에게 노이즈 브레이크를 적용당할 때 호출됩니다. 이때 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakApplied(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) {}

	/** 잡힐때 이벤트입니다. */
	FOnHoldStateChangedDelegate OnHeldStateEntry;

	/** 풀릴때 이벤트입니다. */
	FOnHoldStateChangedDelegate OnHeldStateExit;

protected:
	UFUNCTION()
	void OnRep_HoldingMeActor();

	/** 자신을 잡고 있는 액터입니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_HoldingMeActor")
	TWeakObjectPtr<AActor> HoldingMeActor;

	UPROPERTY()
	TObjectPtr<AActor> SourceActor;
};
