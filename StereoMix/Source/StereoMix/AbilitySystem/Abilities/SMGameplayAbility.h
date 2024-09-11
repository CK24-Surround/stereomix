// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "Utilities/SMLog.h"

#include "SMGameplayAbility.generated.h"

class ASMPlayerCharacter;
class ASMPlayerCharacterBase;
class USMHoldInteractionComponent;
class USMAbilitySystemComponent;
class USMPlayerCharacterDataAsset;

/**
 *
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility();

	USMAbilitySystemComponent* GetSMAbilitySystemComponentFromActorInfo() const;

	ASMPlayerCharacter* GetSMPlayerCharacterFromActorInfo() const;

	UAbilitySystemComponent* GetASC() const;

	template<typename T>
	T* GetASC() { return Cast<T>(GetASC()); }

	AActor* GetAvatarActor() const;

	template<typename T>
	T* GetAvatarActor() { return Cast<T>(GetAvatarActor()); }

	USMHoldInteractionComponent* GetHIC() const;

	template<typename T>
	T* GetHIC() { return Cast<T>(GetHIC()); }

	const USMPlayerCharacterDataAsset* GetDataAsset() const;

	template<typename T>
	const T* GetDataAsset()
	{
		return Cast<T>(GetDataAsset());
	}

	/** 리플리케이션을 활성화 한 경우에만 호출됩니다.*/
	UFUNCTION(Client, Reliable)
	void ClientRPCPlayMontage(UAnimMontage* InMontage, float InPlayRate = 1.0f, FName StartSection = NAME_None);

protected:
	UFUNCTION()
	void EndAbilityByCancel();
};
