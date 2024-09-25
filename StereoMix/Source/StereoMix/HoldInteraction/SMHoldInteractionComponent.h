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
	virtual bool CanHolded(AActor* Instigator) const { return false; }

	/** 시전자에게 잡힐때 필요한 로직을 구현해야합니다. 성공 여부를 반환합니다. 서버에서 호출됩니다. */
	virtual void OnHolded(AActor* Instigator) {}

	/** 시전자로부터 잡히기가 풀릴때 필요한 로직을 구현해야합니다. 성공 여부를 반환합니다. 서버에서 호출됩니다. */
	virtual void OnHoldedReleased(AActor* Instigator) {}

	/** 데미지를 발생시키는지 여부를 반환합니다. */
	virtual bool CanGenerateDamage(AActor* Instigator) const { return false; }

	/** 타일 점령을 발생시키는지 여부를 반환합니다. */
	virtual bool CanGenerateTileCapture(AActor* Instigator) const { return false; }

	/** 일렉기타의 노이즈브레이크 시작 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakActionStarted(ASMElectricGuitarCharacter* Instigator) {}

	/** 피아노의 노이즈브레이크 시작 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakActionStarted(ASMPianoCharacter* Instigator) {}

	/** 베이스의 노이즈브레이크 시작 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakActionStarted(ASMBassCharacter* Instigator) {}

	/** 일렉기타의 노이즈브레이크의 타일 상호작용 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakActionPerformed(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) {}

	/** 피아노의 노이즈브레이크의 타일 상호작용 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakActionPerformed(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) {}

	/** 베이스의 노이즈브레이크의 타일 상호작용 시 필요한 로직을 구현해야합니다. 서버에서 호출됩니다. */
	virtual void OnNoiseBreakActionPerformed(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) {}

	/** 잡힐때 이벤트입니다. */
	FOnHoldStateChangedDelegate OnHoldedStateEntry;

	/** 풀릴때 이벤트입니다. */
	FOnHoldStateChangedDelegate OnHoldedStateExit;

protected:
	UFUNCTION()
	void OnRep_HoldingMeActor();

	/** 자신을 잡고 있는 액터입니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_HoldingMeActor")
	TWeakObjectPtr<AActor> HoldingMeActor;

	UPROPERTY()
	TObjectPtr<AActor> SourceActor;
};
