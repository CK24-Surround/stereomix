#pragma once
#include "NativeGameplayTags.h"

namespace SMTags
{
	namespace Ability
	{
		namespace Activation
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Catch);
		}

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Caught);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Smashed);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(CaughtExit);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(CaughtExitOnStunEnd);
	}

	namespace AttributeSet::Character::Init
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MoveSpeed);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ProjectileCooldown);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ProjectileAttack);
	}

	namespace Character::State
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Catch);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Caught);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stun);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Smashing);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Smashed);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Uncatchable);
	}

	namespace Event
	{
		namespace AnimNotify
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Catch);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Smash);
		}

		namespace Character
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(BuzzerBeaterSmashEnd);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(CaughtExitEnd);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stun);
		}
	}

	namespace GameplayCue
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayNiagara)
	}
}
