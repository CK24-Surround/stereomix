// Fill out your copyright notice in the Description page of Project Settings.


#include "SMTeamSelectTriggerBox.h"

#include "Components/BoxComponent.h"
#include "Components/SMTeamComponent.h"
#include "Interfaces/SMTeamComponentInterface.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

class ISMTeamComponentInterface;

ASMTeamSelectTriggerBox::ASMTeamSelectTriggerBox()
{
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

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

	ISMTeamComponentInterface* OtherTeamComponentInterface = Cast<ISMTeamComponentInterface>(OtherActor);
	if (OtherTeamComponentInterface)
	{
		USMTeamComponent* OtherTeamComponent = OtherTeamComponentInterface->GetTeamComponent();
		if (OtherTeamComponent)
		{
			OtherTeamComponent->SetTeam(TeamComponent->GetTeam());

			const FString TeamName = UEnum::GetValueAsString(TEXT("StereoMix.ESMTeam"), TeamComponent->GetTeam());
			NET_LOG(this, Log, TEXT("%s의 팀이 %s로 변경되었습니다."), *OtherActor->GetName(), *TeamName);
		}
	}
}
