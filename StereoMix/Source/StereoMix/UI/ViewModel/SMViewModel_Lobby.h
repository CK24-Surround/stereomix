// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "StereoMix.h"
#include "Connection/SMClientConnectionSubsystem.h"
#include "SMViewModel_Lobby.generated.h"

UENUM(BlueprintType)
enum class ELobbyState : uint8
{
	Idle,
	RequestJoining,
	RequestCreating,
	RequestQuickMatch,
};

/**
 * Lobby ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_Lobby : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	FText StatusText;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	ELobbyState State;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter=SetAllButtonEnabled, Getter=IsAllButtonEnabled, meta=(AllowPrivateAccess))
	bool bAllButtonEnabled = true;
	
	UPROPERTY()
	TObjectPtr<USMClientConnectionSubsystem> ConnectionSubsystem;
	
public:
	FSimpleDelegate OnRequestJoinRoom;
	
	void Init();
	void Destroy();
	
	const FText& GetStatusText() const { return StatusText; }
	void SetStatusText(const FText& NewStatusText) { UE_MVVM_SET_PROPERTY_VALUE(StatusText, NewStatusText); }

	bool IsAllButtonEnabled() const { return bAllButtonEnabled; }
	void SetAllButtonEnabled(const bool bEnabled) { UE_MVVM_SET_PROPERTY_VALUE(bAllButtonEnabled, bEnabled); }
	
	ELobbyState GetState() const { return State; }
	void SetState(const ELobbyState NewState) { UE_MVVM_SET_PROPERTY_VALUE(State, NewState); }
	
	// ==============================================================
	// View

	UFUNCTION(BlueprintCallable, Category="UI")
	void CreateRoom();

	UFUNCTION(BlueprintCallable, Category="UI")
	void QuickMatch();

	UFUNCTION(BlueprintCallable, Category="UI")
	void JoinRoom();

	UFUNCTION(BlueprintCallable, Category="UI")
	void Back();

protected:
	UFUNCTION()
	void OnLobbyServiceStateChanged(EGrpcServiceState ChangedState);
	
	UFUNCTION()
	void OnCreateRoom(EGrpcResultCode Result, EGrpcLobbyRoomDeploymentStatus DeployStatus, const FString& ConnectionUrl);
	
	UFUNCTION()
	void OnJoinRoom(EGrpcResultCode Result, const FString& ConnectionUrl);
};
