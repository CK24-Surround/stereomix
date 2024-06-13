// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMTeamSelectTriggerBox.generated.h"

class USMTeamComponent;
class UBoxComponent;

UCLASS()
class STEREOMIX_API ASMTeamSelectTriggerBox : public AActor
{
	GENERATED_BODY()

public:
	ASMTeamSelectTriggerBox();

protected:
	virtual void PostInitializeComponents() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void RespawnCharacter(AActor* TargetActor);

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, Category = "TriggerBox")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;
};
