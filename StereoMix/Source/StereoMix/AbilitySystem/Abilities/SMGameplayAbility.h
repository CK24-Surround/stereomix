// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Utilities/SMLog.h"

#include "SMGameplayAbility.generated.h"

class ASMPlayerCharacterBase;
class USMAbilitySystemComponent;
class USMPlayerCharacterDataAsset;
class USMHIC_Character;

/**
 *
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility();

	USMAbilitySystemComponent* GetASC() const;

	template<typename T>
	T* GetASC() const { return Cast<T>(GetASC()); }

	AActor* GetAvatarActor() const;

	template<typename T>
	T* GetAvatarActor() const { return Cast<T>(GetAvatarActor()); }

	ASMPlayerCharacterBase* GetCharacter() const;

	template<typename T>
	T* GetCharacter() const { return Cast<T>(GetCharacter()); }

	USMHIC_Character* GetHIC() const;

	template<typename T>
	T* GetHIC() const { return Cast<T>(GetHIC()); }

	const USMPlayerCharacterDataAsset* GetDataAsset() const;

	template<typename T>
	const T* GetDataAsset() const { return Cast<T>(GetDataAsset()); }

protected:
	UFUNCTION()
	void EndAbilityByCancel();
};
