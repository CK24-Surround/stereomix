// Copyright Surround, Inc. All Rights Reserved.


#include "SMTeamSelectTriggerBox.h"

#include "Characters/SMPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SMTeamComponent.h"
#include "Player/SMGamePlayerState.h"
#include "Player/SMPlayerController.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

class ISMTeamInterface;

ASMTeamSelectTriggerBox::ASMTeamSelectTriggerBox()
{
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	BaseMesh->SetRelativeLocation(FVector(75.0, 75.0, 1.0));

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetRelativeLocation(FVector(75.0, 75.0, 100.0));
	TriggerBox->InitBoxExtent(FVector(75.0, 75.0, 100.0));

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));
}

void ASMTeamSelectTriggerBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!HasAuthority())
	{
		TriggerBox->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	}
}

void ASMTeamSelectTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	NET_LOG(this, Warning, TEXT(""));
	Super::NotifyActorBeginOverlap(OtherActor);

	RespawnCharacter(OtherActor);
}

void ASMTeamSelectTriggerBox::RespawnCharacter(AActor* TargetActor)
{
	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(TargetActor);
	if (!ensureAlways(TargetCharacter))
	{
		return;
	}

	ASMPlayerController* TargetController = TargetCharacter->GetController<ASMPlayerController>();
	if (!ensureAlways(TargetController))
	{
		return;
	}

	ASMGamePlayerState* OtherPlayerState = TargetController->GetPlayerState<ASMGamePlayerState>();
	if (!ensureAlways(OtherPlayerState))
	{
		return;
	}

	// 팀을 변경 후 캐릭터를 스폰합니다.
	OtherPlayerState->SetTeam(TeamComponent->GetTeam());
	TargetController->SpawnCharacter();

	const FString TeamName = UEnum::GetValueAsString(TEXT("StereoMix.ESMTeam"), TeamComponent->GetTeam());
	NET_LOG(this, Log, TEXT("%s의 팀이 %s로 변경되었습니다."), *TargetCharacter->GetName(), *TeamName);
}
