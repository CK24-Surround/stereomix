// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "SMPlayerState.generated.h"

class USMAbilitySystemComponent;
class USMCharacterAttributeSet;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASMPlayerState();

	FORCEINLINE const FString& GetGameLiftPlayerId() const { return GameLiftPlayerId; }

	void SetGameLiftPlayerId(const FString& InGameLiftPlayerId) { GameLiftPlayerId = InGameLiftPlayerId; }

	FORCEINLINE const FString& GetGameLiftPlayerSessionId() const { return GameLiftPlayerSessionId; }

	void SetGameLiftPlayerSessionId(const FString& InGameLiftPlayerSessionId)
	{
		GameLiftPlayerSessionId = InGameLiftPlayerSessionId;
	}

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleInstanceOnly, Replicated, Category = "GameLift")
	FString GameLiftPlayerId;

	UPROPERTY(VisibleInstanceOnly, Replicated, Category = "GameLift")
	FString GameLiftPlayerSessionId;

	UPROPERTY(VisibleAnywhere, Category = "GAS|ASC")
	TObjectPtr<USMAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, Category = "GAS|AttributeSet")
	TObjectPtr<USMCharacterAttributeSet> CharacterAttributeSet;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
