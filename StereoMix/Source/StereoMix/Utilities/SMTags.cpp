#include "SMTags.h"

namespace SMTags
{
	namespace Ability
	{
		namespace Activation
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Catch, "Ability.Activation.Catch", "Catch의 어빌리티 활성화 여부를 나타냅니다.");
		}

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Caught, "Ability.Caught", "Caugh의 어빌리티 태그를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Smashed, "Ability.Smashed", "Smashed의 어빌리티 태그를 나타냅니다.");
	}

	namespace AttributeSet::Character::Init
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MoveSpeed, "AttributeSet.Character.Init.MoveSpeed", "캐릭터의 스탯을 초기화할때 SetByCaller를 사용하기 위한 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ProjectileCooldown, "AttributeSet.Character.Init.ProjectileCooldown", "캐릭터의 스탯을 초기화할때 SetByCaller를 사용하기 위한 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ProjectileAttack, "AttributeSet.Character.Init.ProjectileAttack", "캐릭터의 스탯을 초기화할때 SetByCaller를 사용하기 위한 태그입니다.");
	}

	namespace Character::State
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Catch, "Character.State.Catch", "캐릭터를 잡고 있는 상태를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Caught, "Character.State.Caught", "상대 캐릭터에게 잡혀 있는 상태를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "Character.State.Stun", "기절 상태를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Smashing, "Character.State.Smashing", "매치기 중인 상태를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Smashed, "Character.State.Smashed", "상대 캐릭터에게 매치기 당하는 상태를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Uncatchable, "Character.State.Uncatchable", "잡힐 수 없는 상태임을 나타냅니다.");
	}

	namespace Event
	{
		namespace AnimNotify
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Catch, "Event.AnimNotify.Catch", "잡기 애님 노티파이의 이벤트로 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Smash, "Event.AnimNotify.Smash", "매치기 애님 노티파이의 이벤트로 사용됩니다.");
		}

		namespace Character
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(BuzzerBeaterSmashEnd, "Event.Character.BuzzerBeaterSmashEnd", "매치기가 종료되어 버저 비터가 끝났음을 알리는데 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "Event.Character.Stun", "체력이 모두 소모되어 기절상태에 들어가야함을 알리는데 사용됩니다.");
		}
	}
}