#include "SMTags.h"

namespace SMTags
{
	namespace Ability
	{
		namespace Activation
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Catch, "Ability.Activation.Catch", "Catch의 어빌리티 활성화 여부를 나타냅니다."); // TODO: Deprecated
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "Ability.Activation.Hold", "Catch의 어빌리티 활성화 여부를 나타냅니다.");
		}

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Caught, "Ability.Caught", "Caugh의 어빌리티 태그를 나타냅니다."); // TODO: Deprecated
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Holded, "Ability.Holded", "Holded의 어빌리티 태그를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Smashed, "Ability.Smashed", "Smashed의 어빌리티 태그를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CaughtExit, "Ability.CaughtExit", "CaughtExit의 어빌리티 태그를 나타냅니다."); // TODO: Deprecated
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HoldedExit, "Ability.HoldedExit", "HoldedExit의 어빌리티 태그를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CaughtExitOnStunEnd, "Ability.CaughtExitOnStunEnd", "CaughtExitOnStunEnd의 어빌리티 태그를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HoldedExitOnStunEnd, "Ability.HoldedExitOnStunEnd", "HoldedExitOnStunEnd의 어빌리티 태그를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Immune, "Ability.Immune", "Immune의 어빌리티 태그를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CatchBuff, "Ability.CatchBuff", "CatchBuff의 어빌리티 태그를 나타냅니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakIndicator, "Ability.NoiseBreakIndicator", "노이즈 브레이크 인디케이터의 어빌리티 태그를 나타냅니다.");
	}

	namespace AttributeSet
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxHP, "AttributeSet.MaxHP", "어트리뷰트 셋의 데이터를 가리키기위한 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HP, "AttributeSet.HP", "어트리뷰트 셋의 데이터를 가리키기위한 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MoveSpeed, "AttributeSet.MoveSpeed", "어트리뷰트 셋의 데이터를 가리키기위한 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxStamina, "AttributeSet.MaxStamina", "어트리뷰트 셋의 데이터를 가리키기위한 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stamina, "AttributeSet.Stamina", "어트리뷰트 셋의 데이터를 가리키기위한 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxSkillGauge, "AttributeSet.MaxSkillGauge", "어트리뷰트 셋의 데이터를 가리키기위한 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SkillGauge, "AttributeSet.SkillGauge", "어트리뷰트 셋의 데이터를 가리키기위한 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Heal, "Data.Heal", "힐 메타 데이터 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Data.Damage", "데미지 메타 데이터 태그입니다.");
	}

	namespace Character
	{
		namespace Cooldown
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack, "Character.Cooldown.Attack", "기본 공격 쿨타임을 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "Character.Cooldown.Hold", "잡기 쿨타임을 나타냅니다.");
		}

		namespace State
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Invincible, "Character.State.Invincible", "무적 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(UnlimitStamina, "Character.State.UnlimitStamina", "스태미나 소모가 없는 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlashActivation, "Character.State.SlashActivation", "베기 콤보 활성화 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Slash, "Character.State.SlashActivation", "베기 로직에서 휘두르고 있는 상태를 나타냅니다. ");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Catch, "Character.State.Catch", "캐릭터를 잡고 있는 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "Character.State.Hold", "캐릭터를 잡고 있는 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Caught, "Character.State.Caught", "상대 캐릭터에게 잡혀 있는 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Holded, "Character.State.Holded", "상대 캐릭터에게 잡혀 있는 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "Character.State.Stun", "기절 상태를 나타냅니다."); // TODO: Deprecated
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Neutralize, "Character.State.Neutralize", "무력화 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Smashing, "Character.State.Smashing", "매치기 중인 상태를 나타냅니다."); // TODO: Deprecated
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreak, "Character.State.NoiseBreak", "매치기 중인 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Smashed, "Character.State.Smashed", "상대 캐릭터에게 매치기 당하는 상태를 나타냅니다."); // TODO: Deprecated
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreaked, "Character.State.NoiseBreaked", "노이즈브레이크에 당하는 상태를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Uncatchable, "Character.State.Uncatchable", "잡힐 수 없는 상태임을 나타냅니다."); // TODO: Deprecated
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Unholdable, "Character.State.Unholdable", "잡힐 수 없는 상태임을 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Immune, "Character.State.Immune", "모든 피격 판정이 발생하지 않는 면역 상태임을 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Jump, "Character.State.Jump", "점프 상태임을 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Charge, "Character.State.Charge", "돌격 상태임을 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImpactArrow, "Character.State.ImpactArrow", "충격 화살 시전 상태임을 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBullet, "Character.State.SlowBullet", "마비탄 시전 상태임을 나타냅니다.");
		}
	}

	namespace Data
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Duration, "Data.Duration", "GE의 지속시간 데이터 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown, "Data.Cooldown", "쿨다운 데이터 태그입니다.");
	}

	namespace Item
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(AttributeChanger, "Item.AttributeChanger", "어트리뷰트 체인저 아이템에 사용되는 태그입니다. 규모를 정해줄때 사용합니다. ");
	}

	namespace Event
	{
		namespace AnimNotify
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Catch, "Event.AnimNotify.Catch", "잡기 애님 노티파이의 이벤트로 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "Event.AnimNotify.Hold", "잡기 애님 노티파이의 이벤트로 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Smash, "Event.AnimNotify.Smash", "매치기 애님 노티파이의 이벤트로 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SmashEnd, "Event.AnimNotify.SmashEnd", "매치기 종료 애님 노티파이의 이벤트로 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChargeEndEntry, "Event.AnimNotify.ChargeEndEntry", "돌격 종료 애니메이션 진입 시 알리는 노티파이의 이벤트로 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(PianoNoiseBreakShoot, "Event.AnimNotify.PianoNoiseBreakShoot", "피아노의 노이즈 브레이크 발사 시점을 알리는 애님 노티파이의 이벤트로 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakEnd, "Event.AnimNotify.NoiseBreakEnd", "노이즈브레이크 종료 애님 노티파이의 이벤트로 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBulletShoot, "Event.AnimNotify.SlowBulletShoot", "마비탄 발사 애님 노티파이의 이벤트로 사용됩니다.");

			namespace ElectricGuitar
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakFlash, "Event.AnimNotify.ElectricGuitar.NoiseBreakFlash", "노이즈 브레이크 점멸 애님 노티파이의 이벤트로 사용됩니다.");
			}

			namespace Bass
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakWeaponTrailActivate, "Event.AnimNotify.Bass.NoiseBreakWeaponTrailActivate", "노이즈 브레이크 중 무기 트레일 활성화 애님 노티파이의 이벤트로 사용됩니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakWeaponTrailDeactivate, "Event.AnimNotify.Bass.NoiseBreakWeaponTrailDeactivate", "노이즈 브레이크 중 무기 트레일 비활성화 애님 노티파이의 이벤트로 사용됩니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakSlash, "Event.AnimNotify.Bass.NoiseBreakSlash", "노이즈 브레이크 중 베기 애님 노티파이의 이벤트로 사용됩니다.");
			}

			namespace Attack
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(AttackStart, "Event.AnimNotify.Attack.AttackStart", "공격 판정 시작 노티파이의 이벤트로 사용됩니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(CanInput, "Event.AnimNotify.Attack.CanInput", "입력 가능 애님 노티파이의 이벤트로 사용됩니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(CanProceedNextAction, "Event.AnimNotify.Attack.CanProceedNextAction", "다음 액션으로 넘어갈 수 있는 지점을 나타내는 애님 노티파이의 이벤트로 사용됩니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeftSlashNext, "Event.AnimNotify.Attack.LeftSlashNext", "다음 배기의 방향을 나타내는 애님 노티파이의 이벤트로 사용됩니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(RightSlashNext, "Event.AnimNotify.Attack.RightSlashNext", "다음 배기의 방향을 나타내는 애님 노티파이의 이벤트로 사용됩니다.");
			}
		}

		namespace Character
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(BuzzerBeaterSmashEnd, "Event.Character.BuzzerBeaterSmashEnd", "매치기가 종료되어 버저 비터가 끝났음을 알리는데 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(BuzzerBeaterEnd, "Event.Character.BuzzerBeaterEnd", "노이즈 브레이크가 종료되어 버저 비터가 끝났음을 알리는데 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(CaughtExitEnd, "Event.Character.CaughtExitEnd", "잡힌 상태 탈출이 종료되었음을 알리는데 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "Event.Character.Stun", "체력이 모두 소모되어 기절상태에 들어가야함을 알리는데 사용됩니다."); // TODO: Deprecated
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Neutralize, "Event.Character.Neutralize", "체력이 모두 소모되어 무력화상태에 들어가야함을 알리는데 사용됩니다.");
		}

		namespace Tile
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(TileTrigger, "Ability.TileTrigger", "TileTrigger의 어빌리티 태그를 나타냅니다.");
		}
	}

	namespace GameplayCue
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PlayNiagara, "GameplayCue.PlayNiagara", "파라미터의 위치와 소스 오브젝트를 통해 나이아가라 시스템을 재생시키는 게임플레이 큐 태그입니다. 소스 오브젝트에 나이아가라 시스템을 담아 사용합니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ProjectileLaunch, "GameplayCue.ProjectileLaunch", "투사체 발사시 재생되는 게임플레이 큐의 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ProjectileHit, "GameplayCue.ProjectileHit", "투사체 적중시 재생되는 게임플레이 큐의 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ProjectileWallHit, "GameplayCue.ProjectileWallHit", "투사체가 벽에 적중시 재생되는 게임플레이 큐의 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Catch, "GameplayCue.Catch", "잡기 시전시 재생되는 게임플레이 큐의 태그입니다."); // TODO: Deprecated
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "GameplayCue.Hold", "잡기 시전시 재생되는 게임플레이 큐의 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CatchHit, "GameplayCue.CatchHit", "잡기 적중시 재생되는 게임플레이 큐의 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "GameplayCue.Stun", "스턴시 재생되는 게임플레이 큐의 태그입니다."); // TODO: Deprecated
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Neutralize, "GameplayCue.Neutralize", "무력화 시 재생되는 게임플레이 큐의 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ApplyHeal, "GameplayCue.ApplyHeal", "힐 적용시 재생되는 게임플레이 큐의 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImmuneEnd, "GameplayCue.ImmuneEnd", "면역 종료시 재생되는 게임플레이 큐의 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImmuneMaterialApply_ElectricGuitar, "GameplayCue.ImmuneMaterialApply_ElectricGuitar", "일렉기타의 면역 머티리얼 적용시 재생되는 게임플레이 큐의 태그입니다.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImmuneMaterialReset, "GameplayCue.ImmuneMaterialReset", "면역 머티리얼 리셋시 재생되는 게임플레이 큐의 태그입니다.");

		namespace Common
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Immune, "GameplayCue.Common.Immune", "면역 상태 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "GameplayCue.Common.Hold", "잡기 시전 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HoldSuccess, "GameplayCue.Common.HoldSuccess", "잡기 성공 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HoldHit, "GameplayCue.Common.HoldHit", "잡기 적중 게임플레이 큐의 태그입니다.");
		}

		namespace Piano
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Archery, "GameplayCue.Piano.Archery", "활 쏘기 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryProjectileCharge1, "GameplayCue.Piano.ArcheryProjectileCharge1", "1단계 충전 화살 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryProjectileCharge2, "GameplayCue.Piano.ArcheryProjectileCharge2", "2단계 충전 화살 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryHitCharge1, "GameplayCue.Piano.ArcheryHitCharge1", "1단계 충전 화살 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryHitCharge2, "GameplayCue.Piano.ArcheryHitCharge2", "2단계 충전 화살 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImpactArrow, "GameplayCue.Piano.ImpactArrow", "충격 화살 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImpactArrowExplosion, "GameplayCue.Piano.ImpactArrowExplosion", "충격 화살 폭발 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImpactArrowHit, "GameplayCue.Piano.ImpactArrowHit", "충격 화살 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreak, "GameplayCue.Piano.NoiseBreak", "노이즈 브레이크 발사 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakBurst, "GameplayCue.Piano.NoiseBreakBurst", "노이즈 브레이크 버스트 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakBurstHit, "GameplayCue.Piano.NoiseBreakBurstHit", "노이즈 브레이크 버스트 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HoldWeapon, "GameplayCue.Piano.HoldWeapon", "무기의 잡기 상태 게임플레이 큐의 태그입니다.");
		}

		namespace ElectricGuitar
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shoot, "GameplayCue.ElectricGuitar.Shoot", "기본공격 발사 게임플레이 큐 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ShootProjectile, "GameplayCue.ElectricGuitar.ShootProjectile", "기본공격 투사체 게임플레이 큐 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ShootHit, "GameplayCue.ElectricGuitar.ShootHit", "기본공격 투사체 적중 게임플레이 큐 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBullet, "GameplayCue.ElectricGuitar.SlowBullet", "마비탄 발사 게임플레이 큐 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBulletProjectile, "GameplayCue.ElectricGuitar.SlowBulletProjectile", "마비탄 투사체 게임플레이 큐 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBulletHit, "GameplayCue.ElectricGuitar.SlowBulletHit", "마비탄 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBulletDebuff, "GameplayCue.ElectricGuitar.SlowBulletDebuff", "마비탄 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakFlash, "GameplayCue.ElectricGuitar.NoiseBreakFlash", "노이즈 브레이크 플래시 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakBurst, "GameplayCue.ElectricGuitar.NoiseBreakBurst", "노이즈 브레이크 버스트 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakBurstHit, "GameplayCue.ElectricGuitar.NoiseBreakBurstHit", "노이즈 브레이크 버스트 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HoldWeapon, "GameplayCue.ElectricGuitar.HoldWeapon", "무기의 잡기 상태 게임플레이 큐의 태그입니다.");
		}

		namespace Bass
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Slash, "GameplayCue.Bass.Slash", "베기 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlashHit, "GameplayCue.Bass.SlashHit", "베기 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Charge, "GameplayCue.Bass.Charge", "돌진 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChargeHit, "GameplayCue.Bass.ChargeHit", "돌진 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakBurst, "GameplayCue.Bass.NoiseBreakBurst", "노이즈 브레이크 버스트 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakBurstHit, "GameplayCue.Bass.NoiseBreakBurstHit", "노이즈 브레이크 버스트 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakWeaponTrail, "GameplayCue.Bass.NoiseBreakWeaponTrail", "노이즈 브레이크 무기 트레일 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakSlash, "GameplayCue.Bass.NoiseBreakSlash", "노이즈 브레이크 베기 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HoldWeapon, "GameplayCue.Bass.HoldWeapon", "무기의 잡기 상태 게임플레이 큐의 태그입니다.");
		}

		namespace SpecialAction
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Smash, "GameplayCue.SpecialAction.Smash", "매치기에 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SmashSuccessWithCharacter, "GameplayCue.SpecialAction.SmashSuccessWithCharacter", "캐릭터 매치기 성공시 호출됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SmashSplashHit, "GameplayCue.SpecialAction.SmashSplashHit", "매치기 스플래시 적중시 호출됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SmashSuccessWithHealItem, "GameplayCue.SpecialAction.SmashSuccessWithHealItem", "힐 아이템 매치기 성공시 호출됩니다.");
		}

		namespace JumpPad
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(UseJumpPad, "GameplayCue.JumpPad.UseJumpPad", "점프대 사용시 호출됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Land, "GameplayCue.JumpPad.Land", " 점프대 사용후 착지할때 호출됩니다.");
		}
	}
}
