// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Characters/SMPlayerCharacter.h"
#include "SMGameplayAbility_Catch.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Catch : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Catch();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> CatchMontage;

	UPROPERTY(EditAnywhere, Category = "GAS|Tag")
	FGameplayTagContainer CatchableTags;

protected:
	UFUNCTION()
	void OnInterrupted();

	UFUNCTION()
	void OnComplete();

protected:
	UFUNCTION()
	void OnHoldAnimNotify(FGameplayEventData Payload);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCRequestCatchProcess(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation);

	/** TArray<FOverlapResult>에서 잡을 수 있는 캐릭터를 추려냅니다. 잡을 수 있는 캐릭터가 없다면 false를 반환합니다. */
	bool GetCatchableCharacters(const TArray<FOverlapResult>& InOverlapResults, TArray<ASMPlayerCharacter*>& OutCatchableCharacters);

	/** 지정된 위치에 가장 가까운 캐릭터를 얻어냅니다. */
	ASMPlayerCharacter* GetClosestCharacterFromLocation(const TArray<ASMPlayerCharacter*>& InCharacters, const FVector& InLocation);

	/** 대상을 자신에게 어태치합니다. 서버에서만 수행되어야합니다. 만약 실패하면 false를 반환합니다.*/
	bool AttachTargetCharacter(ASMPlayerCharacter* InTargetCharacter);

protected:
	FVector StartLocation;
	FVector TargetLocation;
};
