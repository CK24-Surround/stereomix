// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data//SMTeam.h"
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

	/** 자신이 속한 팀을 나타냅니다.*/
	UPROPERTY(EditAnywhere, Category = "Team", ReplicatedUsing = "OnRep_Team")
	ESMTeam Team;

public:
	FOnChangeTeamSignature OnChangeTeam;
};
