#pragma once
#include "GameplayTagContainer.h"

namespace StereoMixTag
{
	namespace Ability
	{
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
		inline FGameplayTag Caught = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Caught"));
		inline FGameplayTag Stun = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Stun"));
		inline FGameplayTag IsSmashed = FGameplayTag::RequestGameplayTag(TEXT("Character.State.IsSmashed"));
		inline FGameplayTag Uncatchable = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Uncatchable"));
	}

	namespace Event::Character
	{
		inline FGameplayTag Catch = FGameplayTag::RequestGameplayTag(TEXT("Event.Character.Catch"));
		inline FGameplayTag Stun = FGameplayTag::RequestGameplayTag(TEXT("Event.Character.Stun"));
	}
}
