#pragma once
#include "GameplayTagContainer.h"

namespace SMTagName
{
	namespace Ability
	{
		namespace Activation
		{
			static FName Catch = TEXT("Ability.Activation.Catch");
		}

		static FName Caught = TEXT("Ability.Caught");
		static FName Smashed = TEXT("Ability.Smashed");
	}

	namespace AttributeSet::Character::Init
	{
		static FName MoveSpeed = TEXT("AttributeSet.Character.Init.MoveSpeed");
		static FName ProjectileCooldown = TEXT("AttributeSet.Character.Init.ProjectileCooldown");
		static FName ProjectileAttack = TEXT("AttributeSet.Character.Init.ProjectileAttack");
	}

	namespace Character::State
	{
		static FName Catch = TEXT("Character.State.Catch");
		static FName Caught = TEXT("Character.State.Caught");
		static FName Stun = TEXT("Character.State.Stun");
		static FName Smashing = TEXT("Character.State.Smashing");
		static FName Smashed = TEXT("Character.State.Smashed");
		static FName Uncatchable = TEXT("Character.State.Uncatchable");
	}

	namespace Event
	{
		namespace AnimNotify
		{
			static FName Catch = TEXT("Event.AnimNotify.Catch");
			static FName Smash = TEXT("Event.AnimNotify.Smash");
		}

		namespace Character
		{
			static FName OnSmash = TEXT("Event.Character.OnSmash");
			static FName Stun = TEXT("Event.Character.Stun");
		}
	}
}
