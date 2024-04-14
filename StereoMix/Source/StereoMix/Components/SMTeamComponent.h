// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utilities/SMTeam.h"
#include "SMTeamComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeTeamSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTeamComponent();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FORCEINLINE ESMTeam GetTeam() const { return Team; }
	
	void SetTeam(ESMTeam InTeam);

protected:
	UFUNCTION()
	void OnRep_Team();

protected:
	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_Team")
	ESMTeam Team;

public:
	FOnChangeTeamSignature OnChangeTeam;
};
