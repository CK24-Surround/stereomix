// Copyright Surround, Inc. All Rights Reserved.


#include "SMProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/SMTeamInterface.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "AbilitySystem/SMTags.h"
#include "Components/SMTeamComponent.h"

ASMProjectile::ASMProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetCollisionProfileName(SMCollisionProfileName::Projectile);
	SphereComponent->InitSphereRadius(1.0f);

	ProjectileBodyFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	ProjectileBodyFXComponent->SetupAttachment(SphereComponent);
	ProjectileBodyFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	ProjectileBodyFXComponent->SetAsset(ProjectileBodyFX);
	ProjectileBodyFXComponent->SetAutoActivate(false);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->SetAutoActivate(false);

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));

	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Stun);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Immune);
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

void ASMProjectile::Launch(AActor* NewOwner, const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance, float InMagnitude)
{
	if (!HasAuthority())
	{
		return;
	}

	// 오너와 팀을 지정해줍니다.
	SetOwner(NewOwner);

	ISMTeamInterface* OwnerTeamInterface = Cast<ISMTeamInterface>(NewOwner);
	if (!ensureAlways(OwnerTeamInterface))
	{
		return;
	}

	USMTeamComponent* OwnerTeamComponent = OwnerTeamInterface->GetTeamComponent();
	if (!ensureAlways(OwnerTeamComponent))
	{
		return;
	}

	const ESMTeam OwnerTeam = OwnerTeamInterface->GetTeam();
	TeamComponent->SetTeam(OwnerTeam);

	// 투사체의 데미지는 서버에만 저장해줍니다. 클라이언트에선 쓰이지 않기 때문입니다.
	Magnitude = InMagnitude;

	// 투사체를 활성화합니다.
	StartLifeTime();

	// 클라이언트의 투사체도 발사합니다.
	MulticastRPCLaunchInternal(InStartLocation, InNormal, InSpeed, InMaxDistance);
}

void ASMProjectile::StartLifeTime()
{
	if (!HasAuthority())
	{
		return;
	}

	MulticastRPCStartLifeTimeInternal();
}

void ASMProjectile::EndLifeTime()
{
	if (!HasAuthority())
	{
		return;
	}

	MulticastRPCEndLifeTimeInternal();
}

void ASMProjectile::MulticastRPCLaunchInternal_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance)
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

void ASMProjectile::MulticastRPCStartLifeTimeInternal_Implementation()
{
	if (!HasAuthority())
	{
		ProjectileBodyFXComponent->ActivateSystem();
		SetActorHiddenInGame(false);
	}

	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
}

void ASMProjectile::MulticastRPCEndLifeTimeInternal_Implementation()
{
	if (!HasAuthority())
	{
		ProjectileBodyFXComponent->DeactivateImmediate();
		SetActorHiddenInGame(true);
	}

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
	// 자신이 발사한 투사체에 적중되지 않도록 합니다. (자살 방지)
	if (InTarget == GetOwner())
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget);
	if (!ensureAlways(TargetASC))
	{
		return false;
	}

	// 타겟이 되지 않아야하는 상태라면 무시합니다.
	if (TargetASC->HasAnyMatchingGameplayTags(IgnoreTargetStateTags))
	{
		return false;
	}

	return true;
}

ESMTeam ASMProjectile::GetTeam() const
{
	return TeamComponent->GetTeam();
}
