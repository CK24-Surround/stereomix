#pragma once
#include "GameplayTagContainer.h"

namespace StereoMixTag
{
	namespace Ability
	{
		namespace Activation
		{
			inline FGameplayTag Catch = FGameplayTag::RequestGameplayTag(TEXT("Ability.Activation.Catch"));
		}

		inline FGameplayTag Caught = FGameplayTag::RequestGameplayTag(TEXT("Ability.Caught"));
		inline FGameplayTag CaughtRecover = FGameplayTag::RequestGameplayTag(TEXT("Ability.CaughtRecover"));
	}

	namespace AttributeSet::Character::Init
	{
		inline FGameplayTag MoveSpeed = FGameplayTag::RequestGameplayTag(TEXT("AttributeSet.Character.Init.MoveSpeed"));
		inline FGameplayTag ProjectileCooldown = FGameplayTag::RequestGameplayTag(TEXT("AttributeSet.Character.Init.ProjectileCooldown"));
		inline FGameplayTag ProjectileAttack = FGameplayTag::RequestGameplayTag(TEXT("AttributeSet.Character.Init.ProjectileAttack"));
	}

	namespace Character::State
	{
		inline FGameplayTag Catch = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Catch"));
		inline FGameplayTag Caught = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Caught"));
		inline FGameplayTag Stun = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Stun"));
		inline FGameplayTag Smashing = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Smashing"));
		inline FGameplayTag Smashed = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Smashed"));
		inline FGameplayTag Uncatchable = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Uncatchable"));
	}

	namespace Event
	{
		namespace AnimNotify
		{
			inline FGameplayTag Catch = FGameplayTag::RequestGameplayTag(TEXT("Event.AnimNotify.Catch"));
		}

		namespace Character
		{
			inline FGameplayTag Stun = FGameplayTag::RequestGameplayTag(TEXT("Event.Character.Stun"));
			inline FGameplayTag Smash = FGameplayTag::RequestGameplayTag(TEXT("Event.Character.Smash"));
			inline FGameplayTag Smashed = FGameplayTag::RequestGameplayTag(TEXT("Event.Character.Smashed"));
		}
	}
}
