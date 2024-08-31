// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "Utilities/SMLog.h"

#include "SMGameplayAbility.generated.h"

class USMAbilitySystemComponent;
class ASMPlayerCharacter;

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

	template<typename T>
	T* GetASC()
	{
		if (!ensureAlways(CurrentActorInfo))
		{
			return nullptr;
		}

		T* SourceASC = Cast<T>(CurrentActorInfo->AbilitySystemComponent.Get());
		if (!SourceASC)
		{
			NET_LOG(nullptr, Error, TEXT("소스 ASC가 유효하지 않습니다."));
			return nullptr;
		}

		return SourceASC;
	}

	template<typename T>
	T* GetAvatarActor()
	{
		if (!ensureAlways(CurrentActorInfo))
		{
			return nullptr;
		}

		T* SourceCharacter = Cast<T>(CurrentActorInfo->AvatarActor.Get());
		if (!SourceCharacter)
		{
			NET_LOG(nullptr, Error, TEXT("소스 캐릭터가 유효하지 않습니다."));
			return nullptr;
		}

		return SourceCharacter;
	}

	template<typename T>
	T* GetHIC()
	{
		if (!ensureAlways(CurrentActorInfo))
		{
			return nullptr;
		}

		T* SourceHIC = Cast<T>(USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(CurrentActorInfo->AvatarActor.Get()));
		if (!SourceHIC)
		{
			return nullptr;
		}

		return SourceHIC;
	}

	/** 리플리케이션을 활성화 한 경우에만 호출됩니다.*/
	UFUNCTION(Client, Reliable)
	void ClientRPCPlayMontage(UAnimMontage* InMontage, float InPlayRate = 1.0f, FName StartSection = NAME_None);

protected:
	UFUNCTION()
	void EndAbilityByCancel();
};
