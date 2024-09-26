// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "SMGameStateBase.generated.h"

struct FSMVerbMessage;

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASMGameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * 모든 클라이언트에게 메시지를 전달합니다. Unreliable하므로 메시지 전달을 보장받지 못합니다.
	 * @param Message 전달할 메시지
	 */
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "StereoMix|GameState")
	void MulticastMessageToClients(const FSMVerbMessage Message);

	/**
	 * 모든 클라이언트에게 메시지를 전달합니다. Reliable하기 때문에 반드시 전달해야 하는 메시지가 있을 때 사용합니다.
	 * @param Message 전달할 메시지
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "StereoMix|GameState")
	void MulticastReliableMessageToClients(const FSMVerbMessage Message);

	/** 서버의 FPS를 반환합니다. */
	float GetServerFPS() const;

protected:
	virtual void PreInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(Replicated)
	float ServerFPS = 0.f;
};
