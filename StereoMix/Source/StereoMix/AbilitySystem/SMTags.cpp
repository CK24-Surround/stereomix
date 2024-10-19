#include "SMTags.h"

namespace SMTags
{
	namespace Ability
	{
		namespace Activation
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "Ability.Activation.Hold", "Catch의 어빌리티 활성화 여부를 나타냅니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Slash, "Character.State.Slash", "베기 콤보 활성화 상태를 나타냅니다.");
		}

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
			namespace Common
			{
				namespace Blocking
				{
					UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "Character.State.Common.Blocking.Hold", "잡기 시전 불가 상태임을 나타냅니다.");
					UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack, "Character.State.Common.Blocking.Attack", "공격 시전 불가 상태임을 나타냅니다.");
					UE_DEFINE_GAMEPLAY_TAG_COMMENT(Skill, "Character.State.Common.Blocking.Skill", "스킬 시전 불가 상태임을 나타냅니다.");
					UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreak, "Character.State.Common.Blocking.NoiseBreak", "노이즈브레이크 시전 불가 상태임을 나타냅니다.");
					UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sampling, "Character.State.Common.Blocking.Sampling", "샘플링 시전 불가 상태임을 나타냅니다.");
				}

				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Uncontrollable, "Character.State.Common.Uncontrollable", "조작 불가 상태임을 나타냅니다. ");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Invincible, "Character.State.Common.Invincible", "무적 상태를 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(UnlimitStamina, "Character.State.Common.UnlimitStamina", "스태미나 소모가 없는 상태를 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "Character.State.Common.Hold", "캐릭터를 잡고 있는 상태를 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Held, "Character.State.Common.Held", "상대 캐릭터에게 잡혀 있는 상태를 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "Character.State.Common.Stun", "기절 상태를 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Neutralized, "Character.State.Common.Neutralized", "무력화 상태를 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Exhausted, "Character.State.Common.Exhausted", "탈진 상태를 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreak, "Character.State.Common.NoiseBreak", "매치기 중인 상태를 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreaked, "Character.State.Common.NoiseBreaked", "노이즈브레이크에 당하는 상태를 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(NonHoldable, "Character.State.Common.NonHoldable", "잡을 수 없는 상태임을 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Immune, "Character.State.Common.Immune", "모든 피격 판정이 발생하지 않는 면역 상태임을 나타냅니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Jump, "Character.State.Common.Jump", "점프 상태임을 나타냅니다.");
			}

			namespace ElectricGuitar
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBullet, "Character.State.ElectricGuitar.SlowBullet", "마비탄 시전 상태임을 나타냅니다.");
			}

			namespace Piano
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImpactArrow, "Character.State.Piano.ImpactArrow", "충격 화살 시전 상태임을 나타냅니다.");
			}

			namespace Bass
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Charge, "Character.State.Bass.Charge", "돌격 상태임을 나타냅니다.");
			}
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
			namespace Common
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "Event.AnimNotify.Common.Hold", "잡기 애님 노티파이의 이벤트로 사용됩니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakEnd, "Event.AnimNotify.Common.NoiseBreakEnd", "노이즈브레이크 종료 애님 노티파이의 이벤트로 사용됩니다.");
			}

			namespace ElectricGuitar
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBulletShoot, "Event.AnimNotify.ElectricGuitar.SlowBulletShoot", "마비탄 발사 애님 노티파이의 이벤트로 사용됩니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreakFlash, "Event.AnimNotify.ElectricGuitar.NoiseBreakFlash", "노이즈 브레이크 점멸 애님 노티파이의 이벤트로 사용됩니다.");
			}

			namespace Piano
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(PianoNoiseBreakShoot, "Event.AnimNotify.Piano.PianoNoiseBreakShoot", "피아노의 노이즈 브레이크 발사 시점을 알리는 애님 노티파이의 이벤트로 사용됩니다.");
			}

			namespace Bass
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChargeFail, "Event.AnimNotify.Bass.ChargeFail", "돌진 실패 노티파이의 이벤트로 사용됩니다. ");
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
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(BuzzerBeaterEnd, "Event.Character.BuzzerBeaterEnd", "노이즈 브레이크가 종료되어 버저 비터가 끝났음을 알리는데 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(CaughtExitEnd, "Event.Character.CaughtExitEnd", "잡힌 상태 탈출이 종료되었음을 알리는데 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "Event.Character.Stun", "기절 상태에 들어가야함을 알리는데 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Neutralize, "Event.Character.Neutralize", "체력이 모두 소모되어 무력화상태에 들어가야함을 알리는데 사용됩니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Exhausted, "Event.Character.Exhausted", "스태미너가 모두 소모되어 탈진 상태에 들어가야함을 알리는데 사용됩니다.");
		}
	}

	namespace GameplayCue
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ApplyHeal, "GameplayCue.ApplyHeal", "힐 적용시 재생되는 게임플레이 큐의 태그입니다.");

		namespace Common
		{
			namespace Item
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Invincible, "GameplayCue.Common.Item.Invincible", "무적 아이템 효과를 받는 상태 게임플레이 큐의 태그입니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(PowerUp, "GameplayCue.Common.Item.PowerUp", "공격력 증가 아이템 효과를 받는 상태 게임플레이 큐의 태그입니다.");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(MoveSpeedUp, "GameplayCue.Common.Item.MoveSpeedUp", "이동속도 증가 아이템 효과를 받는 상태 게임플레이 큐의 태그입니다.");
			}

			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Neutralize, "GameplayCue.Common.Neutralize", "무력화 상태 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Immune, "GameplayCue.Common.Immune", "면역 상태 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hold, "GameplayCue.Common.Hold", "잡기 시전 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HoldSuccess, "GameplayCue.Common.HoldSuccess", "잡기 성공 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HoldHit, "GameplayCue.Common.HoldHit", "잡기 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "GameplayCue.Common.Stun", "스턴 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitFlash, "GameplayCue.Common.HitFlash", "히트 섬광 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sampling, "GameplayCue.Common.Sampling", "샘플링 게임플레이 큐의 태그입니다.");
		}

		namespace Piano
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Archery, "GameplayCue.Piano.Archery", "활 쏘기 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryCharge1, "GameplayCue.Piano.ArcheryCharge1", "활 충전 1단계 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryCharge2, "GameplayCue.Piano.ArcheryCharge2", "활 충전 2단계 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryProjectileCharge1, "GameplayCue.Piano.ArcheryProjectileCharge1", "1단계 충전 화살 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryProjectileCharge2, "GameplayCue.Piano.ArcheryProjectileCharge2", "2단계 충전 화살 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryHitCharge1, "GameplayCue.Piano.ArcheryHitCharge1", "1단계 충전 화살 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryHitCharge2, "GameplayCue.Piano.ArcheryHitCharge2", "2단계 충전 화살 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryWallHitCharge1, "GameplayCue.Piano.ArcheryWallHitCharge1", "1단계 충전 화살 벽 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcheryWallHitCharge2, "GameplayCue.Piano.ArcheryWallHitCharge2", "2단계 충전 화살 벽 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImpactArrow, "GameplayCue.Piano.ImpactArrow", "충격 화살 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImpactArrowReady, "GameplayCue.Piano.ImpactArrowReady", "충격 화살 준비 게임플레이 큐의 태그입니다.");
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
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ShootWallHit, "GameplayCue.ElectricGuitar.ShootWallHit", "기본공격 투사체 벽 적중 게임플레이 큐 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBullet, "GameplayCue.ElectricGuitar.SlowBullet", "마비탄 발사 게임플레이 큐 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SlowBulletCharge, "GameplayCue.ElectricGuitar.SlowBulletCharge", "마비탄 발사 게임플레이 큐 태그입니다.");
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
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChargeCrush, "GameplayCue.Bass.ChargeCrush", "돌진 충돌 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChargeHit, "GameplayCue.Bass.ChargeHit", "돌진 적중 게임플레이 큐의 태그입니다.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(NoiseBreak, "GameplayCue.Bass.NoiseBreak", "노이즈 브레이크 시작 게임플레이 큐의 태그입니다.");
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
