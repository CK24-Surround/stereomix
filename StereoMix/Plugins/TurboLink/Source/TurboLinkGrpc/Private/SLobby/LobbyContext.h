//Generated by TurboLink CodeGenerator, do not edit!
#pragma once
#include "TurboLinkGrpcContext.h"
#include "SLobby/LobbyMessage.h"
#include "SLobby/LobbyMarshaling.h"
class GrpcContext_LobbyService_CreateRoom : public GrpcContext_Ping_Pong<LobbyService_CreateRoom_ReaderWriter, ::lobby::CreateRoomResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_CreateRoom_ReaderWriter, ::lobby::CreateRoomResponse> Super;

private:
	void Call(const FGrpcLobbyCreateRoomRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_CreateRoom(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_QuickMatch : public GrpcContext_Ping_Pong<LobbyService_QuickMatch_ReaderWriter, ::lobby::QuickMatchResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_QuickMatch_ReaderWriter, ::lobby::QuickMatchResponse> Super;

private:
	void Call(const FGrpcLobbyQuickMatchRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_QuickMatch(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_JoinRoom : public GrpcContext_Ping_Pong<LobbyService_JoinRoom_ReaderWriter, ::lobby::JoinRoomResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_JoinRoom_ReaderWriter, ::lobby::JoinRoomResponse> Super;

private:
	void Call(const FGrpcLobbyJoinRoomRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_JoinRoom(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_JoinRoomWithCode : public GrpcContext_Ping_Pong<LobbyService_JoinRoomWithCode_ReaderWriter, ::lobby::JoinRoomWithCodeResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_JoinRoomWithCode_ReaderWriter, ::lobby::JoinRoomWithCodeResponse> Super;

private:
	void Call(const FGrpcLobbyJoinRoomWithCodeRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_JoinRoomWithCode(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_GetRoomList : public GrpcContext_Ping_Pong<LobbyService_GetRoomList_ReaderWriter, ::lobby::GetRoomListResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_GetRoomList_ReaderWriter, ::lobby::GetRoomListResponse> Super;

private:
	void Call(const FGrpcLobbyGetRoomListRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_GetRoomList(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_UpdateRoomState : public GrpcContext_Ping_Pong<LobbyService_UpdateRoomState_ReaderWriter, ::lobby::UpdateRoomStateResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_UpdateRoomState_ReaderWriter, ::lobby::UpdateRoomStateResponse> Super;

private:
	void Call(const FGrpcLobbyUpdateRoomStateRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_UpdateRoomState(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_UpdateRoomConfig : public GrpcContext_Ping_Pong<LobbyService_UpdateRoomConfig_ReaderWriter, ::lobby::UpdateRoomConfigResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_UpdateRoomConfig_ReaderWriter, ::lobby::UpdateRoomConfigResponse> Super;

private:
	void Call(const FGrpcLobbyUpdateRoomConfigRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_UpdateRoomConfig(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_ChangeRoomPassword : public GrpcContext_Ping_Pong<LobbyService_ChangeRoomPassword_ReaderWriter, ::lobby::ChangeRoomPasswordResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_ChangeRoomPassword_ReaderWriter, ::lobby::ChangeRoomPasswordResponse> Super;

private:
	void Call(const FGrpcLobbyChangeRoomPasswordRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_ChangeRoomPassword(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_ChangeRoomOwner : public GrpcContext_Ping_Pong<LobbyService_ChangeRoomOwner_ReaderWriter, ::lobby::ChangeRoomOwnerResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_ChangeRoomOwner_ReaderWriter, ::lobby::ChangeRoomOwnerResponse> Super;

private:
	void Call(const FGrpcLobbyChangeRoomOwnerRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_ChangeRoomOwner(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_DeleteRoom : public GrpcContext_Ping_Pong<LobbyService_DeleteRoom_ReaderWriter, ::lobby::DeleteRoomResponse>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Pong<LobbyService_DeleteRoom_ReaderWriter, ::lobby::DeleteRoomResponse> Super;

private:
	void Call(const FGrpcLobbyDeleteRoomRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_DeleteRoom(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

class GrpcContext_LobbyService_ListenRoomUpdates : public GrpcContext_Ping_Stream<LobbyService_ListenRoomUpdates_ReaderWriter, ::lobby::Room>
{
	friend class ULobbyServiceClient;
	typedef GrpcContext_Ping_Stream<LobbyService_ListenRoomUpdates_ReaderWriter, ::lobby::Room> Super;

private:
	void Call(const FGrpcLobbyListenRoomUpdatesRequest& Request);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) override;

public:
	GrpcContext_LobbyService_ListenRoomUpdates(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
};

