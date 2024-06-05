// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMLobbyProgressSectionWidget.h"
#include "Backend/Client/SMClientLobbySubsystem.h"
#include "UI/Widget/Frontend/SMFrontendElementWidget.h"
#include "SMLobbyProcessWidget.generated.h"

UENUM()
enum class ELobbyProcessUiState
{
	Idle,
	Processing,
	Success,
	Failure
};

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMLobbyProcessWidget : public USMFrontendElementWidget
{
	GENERATED_BODY()

public:
	USMLobbyProcessWidget();

protected:
	ELobbyProcessUiState UiState = ELobbyProcessUiState::Idle;

	UPROPERTY()
	TObjectPtr<USMClientAuthSubsystem> AuthSubsystem;

	UPROPERTY()
	TObjectPtr<USMClientLobbySubsystem> LobbySubsystem;
	
	virtual void NativeConstruct() override;

	virtual void NativeOnActivated() override;
	
	virtual bool NativeOnHandleBackAction() override;

	void SetProgressText(const FText& Text) const;

	void SetProgressVisibility(const ESlateVisibility NewVisibility) const;

	bool CheckIsAuthenticated() const;

	void ConnectToGameServer(const FString& ServerUrl) const;

private:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<USMLobbyProgressSectionWidget> Progress;
};
