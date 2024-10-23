// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMCharacterSelectOptionData.h"
#include "Games/SMPlayerState.h"

#include "SMCharacterSelectPlayerState.generated.h"

class ASMCharacterSelectState;

UENUM(BlueprintType)
enum class ECharacterSelectPlayerStateType : uint8
{
	Loading,
	Unready,
	Ready
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSelectPlayerStateTypeChanged, ECharacterSelectPlayerStateType, NewState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterChangeResponse, bool, bSuccess, ESMCharacterType, NewCharacterType);

/**
 *
 */
UCLASS()
class STEREOMIX_API ASMCharacterSelectPlayerState : public ASMPlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCharacterSelectPlayerStateTypeChanged OnCurrentStateChanged;

	FCharacterChangeResponse OnCharacterChangeResponse;

	ASMCharacterSelectPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ChangeCharacterType(ESMCharacterType NewCharacterType);

	UFUNCTION(Server, Reliable)
	void ChangeFocusCharacterType(ESMCharacterType NewCharacterType);

	UFUNCTION(Client, Reliable)
	void ResponseChangeCharacter(bool bSuccess, ESMCharacterType NewCharacterType);

	virtual bool CanChangeCharacterType(ESMCharacterType NewCharacterType) const override;

	virtual void OnCharacterTypeChanged(ESMCharacterType PreviousCharacterType, ESMCharacterType NewCharacterType) override;

	ASMCharacterSelectState* GetCharacterSelectState() const { return CharacterSelectState.Get(); }

	ECharacterSelectPlayerStateType GetCurrentState() const { return CurrentState; }

	ESMCharacterType GetFocusCharacterType() const { return FocusCharacterType; }

	UFUNCTION(Client, Reliable)
	void SetPredictFocusCharacterType(ESMCharacterType NewFocusCharacterType);

	UFUNCTION(Reliable, Server)
	void SetCurrentState(ECharacterSelectPlayerStateType NewState);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Options, meta = (AllowPrivateAccess))
	TObjectPtr<USMCharacterSelectOptionData> DefaultOptions;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_CurrentState)
	ECharacterSelectPlayerStateType CurrentState;

	UPROPERTY(ReplicatedUsing = OnRep_FocusCharacterType)
	ESMCharacterType FocusCharacterType;

	UFUNCTION()
	void OnRep_CurrentState();

	UFUNCTION()
	void OnRep_FocusCharacterType();

private:
	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectState> CharacterSelectState;
};
