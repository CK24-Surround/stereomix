// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Launch_ElectricGuitar.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacter.h"
#include "Games/SMGameMode.h"
#include "Projectiles/SMProjectile.h"
#include "Utilities/SMLog.h"

USMGameplayAbility_Launch_ElectricGuitar::USMGameplayAbility_Launch_ElectricGuitar()
{
	ProjectileSpeed = 3750.0f;
	LaunchRate = 1.75f;
	MaxDistance = 1100.0f;
	Damage = 6.0f;
}

void USMGameplayAbility_Launch_ElectricGuitar::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		NET_LOG(nullptr, Warning, TEXT("소스 캐릭터가 무효합니다."));
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	if (IsLocallyControlled()) // 커서의 정보를 갖고 있는 로컬에서부터 투사체 로직이 수행되어야합니다.
	{
		LaunchProjectile();
	}
}

void USMGameplayAbility_Launch_ElectricGuitar::LaunchProjectile()
{
	LaunchData.SpawnLocationOffsets.Empty();
	LaunchData.SpawnLocationOffsets.AddZeroed(ProjectileCount); // 발사해야할 투사체의 개수만큼 사이즈를 초기화하고 0으로 채웁니다.
	LaunchData.SpawnRotationOffsets.Empty();
	LaunchData.SpawnRotationOffsets.AddZeroed(ProjectileCount); // 발사해야할 투사체의 개수만큼 사이즈를 초기화하고 0으로 채웁니다.
	LaunchData.Count = 0;
	LaunchData.Rate = TotalLaunchTime / static_cast<float>(ProjectileCount - 1);

	const FVector StartLocation = FVector::LeftVector * (ProjectileWidth / 2.0f); // 왼쪽 시작 위치입니다.
	const float SegmentLocation = ProjectileWidth / (ProjectileCount - 1);

	const FRotator StartRotation = FRotator(0.0, -(ProjectileTotalAngle / 2.0), 0.0); // 왼쪽 시작 각도 입니다.
	const float SegmentRotation = ProjectileTotalAngle / (ProjectileCount - 1);

	// 각 배열에 위치와 회전의 오프셋을 통해 순차적으로 저장합니다.
	TArray<FVector> Locations;
	TArray<FRotator> Rotations;
	for (int32 i = 0; i < ProjectileCount; ++i)
	{
		const FVector OffsetLocation = FVector::RightVector * (i * SegmentLocation);
		Locations.Add(StartLocation + OffsetLocation);

		const float OffsetRotationYaw = i * SegmentRotation;
		FRotator NewRotation(0.0, StartRotation.Yaw + OffsetRotationYaw, 0.0);
		Rotations.Add(NewRotation);
	}

	// 중심 값을 먼저 배열에 저장합니다. 가운데서 먼저 발사되게 하기 위함입니다.
	const int32 MidIndex = Locations.Num() / 2;
	LaunchData.SpawnLocationOffsets[0] = Locations[MidIndex];
	LaunchData.SpawnRotationOffsets[0] = Rotations[MidIndex];

	// 좌우에 번갈아가며 순서대로 배열에 채워넣습니다. 결과적으로 중심, 좌, 우, 좌, 우... 이런식으로 배열에 저장되게됩니다.
	int32 Left = MidIndex - 1;
	int32 Right = MidIndex + 1;
	int32 Index = 1;
	while (Left >= 0 || Right < ProjectileCount)
	{
		if (Left >= 0)
		{
			LaunchData.SpawnLocationOffsets[Index] = Locations[Left];
			LaunchData.SpawnRotationOffsets[Index] = Rotations[Left];
			++Index;
			--Left;
		}

		if (Right < ProjectileCount)
		{
			LaunchData.SpawnLocationOffsets[Index] = Locations[Right];
			LaunchData.SpawnRotationOffsets[Index] = Rotations[Right];
			++Index;
			++Right;
		}
	}

	// 첫발은 즉시 발사합니다.
	LaunchProjectileTimerCallback();
}

void USMGameplayAbility_Launch_ElectricGuitar::LaunchProjectileTimerCallback()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		NET_LOG(nullptr, Error, TEXT("소스 캐릭터가 무효합니다."));
		return;
	}

	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	const FVector TargetLocatiion = SourceCharacter->GetCursorTargetingPoint();
	const FRotator SourceToTargetRotation = (TargetLocatiion - SourceLocation).GetSafeNormal2D().Rotation(); // 타겟을 향하는 회전값입니다.

	// 순차적으로 저장된 배열에서 순서대로 오프셋을 불러옵니다.
	const FVector OffsetLocation = LaunchData.SpawnLocationOffsets[LaunchData.Count];
	const FRotator OffsetRotation = LaunchData.SpawnRotationOffsets[LaunchData.Count];
	const FRotator FinalRotation = SourceToTargetRotation + OffsetRotation; // 최종 회전값입니다.

	// 회전 행렬을 통해 오프셋 위치를 보정합니다. 영벡터 기준으로 좌우로 나열된 벡터들을 올바른 방향을 바라보도록 합니다.
	const FMatrix RotationMatrix = FRotationMatrix(FinalRotation);
	const FVector RotatingLocation = RotationMatrix.TransformVector(OffsetLocation);

	// 주어진 위치와 회전을 기준으로 투사체를 발사합니다.
	ServerRPCLaunchProjectile(SourceLocation + RotatingLocation, FinalRotation.Vector(), ++LaunchData.Count);

	if (LaunchData.Count < ProjectileCount) // 아직 발사 횟수를 채우지 못했다면 다시 발사하기 위해 타이머를 작동시킵니다.
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USMGameplayAbility_Launch_ElectricGuitar::LaunchProjectileTimerCallback, LaunchData.Rate);
	}
	else // 발사 횟수가 충족되면 GA를 종료합니다.
	{
		K2_EndAbility();
	}
}

void USMGameplayAbility_Launch_ElectricGuitar::ServerRPCLaunchProjectile_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantizeNormal& Normal, int LaunchCount)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	ASMGameMode* CachedSMGameMode = GetWorld()->GetAuthGameMode<ASMGameMode>();
	if (!ensureAlways(CachedSMGameMode))
	{
		EndAbilityByCancel();
		return;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		EndAbilityByCancel();
		return;
	}

	ASMProjectile* NewProjectile = CachedSMGameMode->GetProjectileFromProjectilePool(SourceTeam, ESMCharacterType::ElectricGuitar); // 게임모드의 투사체풀을 통해 투사체를 가져옵니다.
	if (!NewProjectile)
	{
		EndAbilityByCancel();
		return;
	}

	NewProjectile->Launch(SourceCharacter, SourceLocation, Normal, ProjectileSpeed, MaxDistance, Damage); // 투사체를 발사합니다.

	// FX를 실행합니다.
	FGameplayCueParameters GCParams;
	GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
	GCParams.RawMagnitude = LaunchCount;
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileLaunch, GCParams);
}