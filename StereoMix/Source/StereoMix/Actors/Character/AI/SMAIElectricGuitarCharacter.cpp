// Copyright Studio Surround. All Rights Reserved.


#include "SMAIElectricGuitarCharacter.h"

#include "Actors/Projectiles/SMProjectile.h"
#include "FunctionLibraries/SMProjectileFunctionLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"


void ASMAIElectricGuitarCharacter::Attack(AActor* AttackTarget, float Accuracy, float Damage)
{
	const ESMTeam SourceTeam = GetTeam();
	ASMProjectile* Projectile = USMProjectileFunctionLibrary::GetElectricGuitarProjectile(GetWorld(), ESMTeam::FutureBass);
	if (!Projectile)
	{
		return;
	}

	const int RandomValue = FMath::Rand() % 100;
	float RandomYaw;
	if (RandomValue < Accuracy)
	{
		RandomYaw = (FMath::Rand() % 10) - 5.0f; // 10도 범위
	}
	else
	{
		RandomYaw = (FMath::Rand() % static_cast<int32>(30.0f)) - (30.0f / 2.0f);
	}

	const FVector LaunchDirection = ((AttackTarget->GetActorLocation() - GetActorLocation()).Rotation() + FRotator(0.0, RandomYaw, 0.0)).Vector();
	constexpr float MaxDistance = 7.0f * USMTileFunctionLibrary::DefaultTileSize;

	UE_LOG(LogTemp, Warning, TEXT("ASMAIElectricGuitarCharacter::Attack %s"), *LaunchDirection.ToString());

	FSMProjectileParameters ProjectileParams;
	ProjectileParams.Owner = this;
	ProjectileParams.StartLocation = GetActorLocation();
	ProjectileParams.Normal = LaunchDirection;
	ProjectileParams.Damage = Damage;
	ProjectileParams.Speed = 200.0f;
	ProjectileParams.MaxDistance = MaxDistance;
	Projectile->Launch(ProjectileParams);
}
