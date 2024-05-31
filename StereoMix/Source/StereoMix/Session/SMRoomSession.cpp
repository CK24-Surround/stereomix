// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomSession.h"

// Sets default values
ASMRoomSession::ASMRoomSession()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bCanEnterRoom = true;
}

void ASMRoomSession::RegisterServer()
{
	Super::RegisterServer();
}

void ASMRoomSession::InitOptions(const FString& Options)
{
	MaxPlayers = 6;
	Super::InitOptions(Options);
}

// Called when the game starts or when spawned
void ASMRoomSession::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASMRoomSession::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
