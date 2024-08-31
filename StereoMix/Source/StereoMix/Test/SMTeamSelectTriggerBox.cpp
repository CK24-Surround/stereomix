// Copyright Surround, Inc. All Rights Reserved.


#include "SMTeamSelectTriggerBox.h"

#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Components/BoxComponent.h"
#include "Components/SMTeamComponent.h"
#include "Controllers/SMGamePlayerController.h"
#include "Games/SMGamePlayerState.h"
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
	Super::NotifyActorBeginOverlap(OtherActor);

	RespawnCharacter(OtherActor);
}

void ASMTeamSelectTriggerBox::RespawnCharacter(AActor* TargetActor)
{
	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!ensureAlways(TargetCharacter))
	{
		return;
	}

	ASMGamePlayerController* TargetController = TargetCharacter->GetController<ASMGamePlayerController>();
	if (!ensureAlways(TargetController))
	{
		return;
	}

	ASMGamePlayerState* OtherPlayerState = TargetController->GetPlayerState<ASMGamePlayerState>();
	if (!ensureAlways(OtherPlayerState))
	{
		return;
	}

	// 팀 및 캐릭터 변경 후 캐릭터를 스폰합니다.
	OtherPlayerState->SetTeam(TeamComponent->GetTeam());
	OtherPlayerState->SetCharacterType(CharacterType);
	TargetController->SpawnCharacter();

	const FString TeamName = UEnum::GetValueAsString(TEXT("StereoMix.ESMTeam"), TeamComponent->GetTeam());
	NET_LOG(this, Log, TEXT("%s의 팀이 %s, 캐릭터는 %s로 변경되었습니다."), *TargetCharacter->GetName(), *TeamName, *UEnum::GetValueAsString(CharacterType));
}
