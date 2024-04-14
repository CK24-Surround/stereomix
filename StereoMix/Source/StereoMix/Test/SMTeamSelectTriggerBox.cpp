// Fill out your copyright notice in the Description page of Project Settings.


#include "SMTeamSelectTriggerBox.h"

#include "Characters/SMPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Utilities/SMCollision.h"

ASMTeamSelectTriggerBox::ASMTeamSelectTriggerBox()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(SMCollisionObjectChannel::Player, ECR_Overlap);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(TriggerBox);
	BaseMesh->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMTeamSelectTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// 테스트 용 액터기 때문에 인터페이스는 구현하지 않았습니다.
	ASMPlayerCharacter* OtherCharacter = Cast<ASMPlayerCharacter>(OtherActor);
	if (OtherCharacter)
	{
	}
}
