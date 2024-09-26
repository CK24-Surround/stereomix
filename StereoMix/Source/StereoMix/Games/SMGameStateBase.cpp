// Copyright Studio Surround. All Rights Reserved.


#include "SMGameStateBase.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/SMVerbMessage.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMGameStateBase)

extern ENGINE_API float GAverageFPS;

ASMGameStateBase::ASMGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASMGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerFPS);
}

void ASMGameStateBase::MulticastMessageToClients_Implementation(const FSMVerbMessage Message)
{
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

void ASMGameStateBase::MulticastReliableMessageToClients_Implementation(const FSMVerbMessage Message)
{
	MulticastMessageToClients_Implementation(Message);
}

float ASMGameStateBase::GetServerFPS() const
{
	return ServerFPS;
}

void ASMGameStateBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASMGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
}
