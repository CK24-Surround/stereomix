#pragma once
#include "GameplayTagContainer.h"

namespace StereoMixTag
{
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
	}

	namespace Event::Character
	{
		inline FGameplayTag Catch = FGameplayTag::RequestGameplayTag(TEXT("Event.Character.Catch"));
		inline FGameplayTag Stun = FGameplayTag::RequestGameplayTag(TEXT("Event.Character.Stun"));
	}
}
