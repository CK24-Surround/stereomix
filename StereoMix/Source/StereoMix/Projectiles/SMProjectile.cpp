// Copyright Surround, Inc. All Rights Reserved.


#include "SMProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SMTeamComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/SMTeamInterface.h"
#include "NiagaraComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMCollision.h"

ASMProjectile::ASMProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;
	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetCollisionProfileName(SMCollisionProfileName::Projectile);
	SphereComponent->InitSphereRadius(1.0f);

	ProjectileBodyFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	ProjectileBodyFXComponent->SetupAttachment(SphereComponent);
	ProjectileBodyFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	ProjectileBodyFXComponent->SetAutoActivate(false);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->SetAutoActivate(false);

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));
}

void ASMProjectile::Launch(AActor* NewOwner, const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance, float InMagnitude)
{
	if (!HasAuthority() || !NewOwner)
	{
		return;
	}

	// 오너와 팀을 지정해줍니다.
	SetOwner(NewOwner);

	const ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(NewOwner);
	TeamComponent->SetTeam(SourceTeam);

	// 투사체의 데미지는 서버에만 저장해줍니다. 클라이언트에선 쓰이지 않기 때문입니다.
	Magnitude = InMagnitude;

	// 클라이언트의 투사체도 발사합니다.
	MulticastLaunchInternal(InStartLocation, InNormal, InSpeed, InMaxDistance);

	// 투사체를 활성화합니다.
	StartLifeTime();
}

void ASMProjectile::StartLifeTime()
{
	if (!HasAuthority())
	{
		return;
	}

	AddProjectileFX();
	MulticastStartLifeTimeInternal();
}

void ASMProjectile::EndLifeTime()
{
	if (!HasAuthority())
	{
		return;
	}

	RemoveProjectileFX();
	MulticastEndLifeTimeInternal();
}

ESMTeam ASMProjectile::GetTeam() const
{
	return TeamComponent->GetTeam();
}

void ASMProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 투사체 풀을 통해 사용해야하기 때문에 비활성화 상태로 초기화합니다.
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}

void ASMProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 매번 투사체가 사거리를 벗어났는지 체크합니다.
	ReturnToPoolIfOutOfMaxDistance();
}

void ASMProjectile::MulticastLaunchInternal_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance)
{
	// 투사체에 필요한 데이터들을 저장하고 위치를 초기화해줍니다. 이 시점은 막 투사체 풀에서 투사체를 꺼내온 상황입니다.
	MaxDistance = InMaxDistance;
	StartLocation = InStartLocation;
	SetActorLocationAndRotation(InStartLocation, InNormal.Rotation());

	// 투사체를 발사합니다. 블로킹으로 투사체가 마무리된 경우 SetUpdatedComponent가 초기화되기 때문에 재지정해줍니다.
	ProjectileMovementComponent->Activate(true);
	ProjectileMovementComponent->SetUpdatedComponent(GetRootComponent());
	ProjectileMovementComponent->SetComponentTickEnabled(true);
	ProjectileMovementComponent->Velocity = InNormal * InSpeed;
}

void ASMProjectile::MulticastStartLifeTimeInternal_Implementation()
{
	SetActorHiddenInGame(false);
	ProjectileBodyFXComponent->Activate(true);

	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
}

void ASMProjectile::MulticastEndLifeTimeInternal_Implementation()
{
	SetActorHiddenInGame(true);
	ProjectileBodyFXComponent->DeactivateImmediate();

	ProjectileMovementComponent->Deactivate();
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);

	(void)OnEndLifeTime.ExecuteIfBound(this);
}

void ASMProjectile::ReturnToPoolIfOutOfMaxDistance()
{
	// 투사체가 최대 사거리 밖으로 나아간 경우 투사체를 제거합니다.
	const float DistanceSquaredFromStartLocation = FVector::DistSquared(StartLocation, GetActorLocation());
	if (DistanceSquaredFromStartLocation > FMath::Square(MaxDistance))
	{
		// 서버는 투사체를 풀로 반환시키지만 클라이언트에서는 가시성만 제거합니다.
		if (HasAuthority())
		{
			EndLifeTime();
		}
		else
		{
			SetActorHiddenInGame(true);
		}
	}
}

bool ASMProjectile::IsValidateTarget(AActor* InTarget)
{
	if (InTarget == GetOwner())
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget);
	if (TargetASC)
	{
		// 타겟이 되지 않아야하는 상태라면 무시합니다.
		if (TargetASC->HasAnyMatchingGameplayTags(IgnoreTargetStateTags))
		{
			return false;
		}
	}

	return true;
}
