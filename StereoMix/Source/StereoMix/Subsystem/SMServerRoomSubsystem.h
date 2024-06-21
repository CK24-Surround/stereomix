// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SLobby/LobbyService.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMServerRoomSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMServerRoomSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	const FGrpcMetaData& GetAuthentication() const
	{
		return AuthorizationHeader;
	}

	const FString& GetRoomId() const
	{
		return RoomId;
	}

	const FString& GetRoomCode() const
	{
		return RoomCode;
	}

	ULobbyService* GetLobbyService() const
	{
		return LobbyService;
	}

private:
	EGrpcLobbyRoomState RoomState;
	FGrpcMetaData AuthorizationHeader;

	FString AuthToken;
	FString RoomId;
	FString RoomCode;

	UPROPERTY(Transient)
	TObjectPtr<ULobbyService> LobbyService;

	UPROPERTY(Transient)
	TObjectPtr<ULobbyServiceClient> LobbyServiceClient;
};