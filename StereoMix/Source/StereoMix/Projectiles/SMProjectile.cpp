// Copyright Surround, Inc. All Rights Reserved.


#include "SMProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SMTeamComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/SMTeamComponentInterface.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "AbilitySystem/SMTags.h"
#include "Interfaces/SMDamageInterface.h"

ASMProjectile::ASMProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetCollisionProfileName(SMCollisionProfileName::Projectile);
	SphereComponent->InitSphereRadius(1.0f);

	ProjectileFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	ProjectileFXComponent->SetupAttachment(SphereComponent);
	ProjectileFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	ProjectileFXComponent->SetAsset(ProjectileBodyFX);
	ProjectileFXComponent->SetAutoActivate(false);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->SetAutoActivate(false);

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));

	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Stun);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Immune);
}

void ASMProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 팀이 변경될 경우 처리해야할 로직을 담은 함수를 바인드해줍니다.
	TeamComponent->OnChangeTeam.AddDynamic(this, &ASMProjectile::OnChangeTeamCallback);
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

void ASMProjectile::Launch(AActor* NewOwner, const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance, float InDamage)
{
	if (!HasAuthority())
	{
		return;
	}

	// 오너와 팀을 지정해줍니다.
	SetOwner(NewOwner);

	ISMTeamComponentInterface* OwnerTeamComponentInterface = Cast<ISMTeamComponentInterface>(NewOwner);
	if (!ensureAlways(OwnerTeamComponentInterface))
	{
		return;
	}

	USMTeamComponent* OwnerTeamComponent = OwnerTeamComponentInterface->GetTeamComponent();
	if (!ensureAlways(OwnerTeamComponent))
	{
		return;
	}

	const ESMTeam OwnerTeam = OwnerTeamComponent->GetTeam();
	TeamComponent->SetTeam(OwnerTeamComponent->GetTeam());

	// 투사체의 데미지는 서버에만 저장해줍니다. 클라이언트에선 쓰이지 않기 때문입니다.
	Damage = InDamage;

	// 클라이언트의 투사체도 발사합니다.
	MulticastRPCLaunch(InStartLocation, InNormal, InSpeed, InMaxDistance);
}

void ASMProjectile::MulticastRPCLaunch_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance)
{
	// 투사체에 필요한 데이터들을 저장하고 위치를 초기화해줍니다. 이 시점은 막 투사체 풀에서 투사체를 꺼내온 상황입니다.
	MaxDistance = InMaxDistance;
	StartLocation = InStartLocation;
	SetActorLocationAndRotation(InStartLocation, InNormal.Rotation());

	// 투사체를 활성화 해줍니다.
	MulticastRPCStartLifeTime();

	// 투사체를 실제로 발사합니다. 블로킹으로 투사체가 마무리된 경우 SetUpdatedComponent가 초기화되기 때문에 재지정해줍니다.
	ProjectileMovementComponent->Activate(true);
	ProjectileMovementComponent->SetUpdatedComponent(GetRootComponent());
	ProjectileMovementComponent->SetComponentTickEnabled(true);
	ProjectileMovementComponent->Velocity = InNormal * InSpeed;
}

void ASMProjectile::MulticastRPCStartLifeTime_Implementation()
{
	if (!HasAuthority())
	{
		ProjectileFXComponent->ActivateSystem();
		SetActorHiddenInGame(false);
	}

	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
}

void ASMProjectile::MulticastRPCEndLifeTime_Implementation()
{
	if (!HasAuthority())
	{
		SetActorHiddenInGame(true);
	}

	ProjectileFXComponent->DeactivateImmediate();
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
			MulticastRPCEndLifeTime();
		}
		else
		{
			SetActorHiddenInGame(true);
		}
	}
}

void ASMProjectile::OnChangeTeamCallback()
{
	// 투사체 발사 후 팀 리플리케이트 정보가 콜백되어 이펙트가 올바르게 적용되지 않는 현상이 존재합니다.
	// 따라서 아래 코드는 폐기하고 직접 RPC를 호출할때 Team을 매개변수로 전달해주도록 변경했습니다.
	// const ESMTeam Team = TeamComponent->GetTeam();
	//
	// if (!HasAuthority())
	// {
	// 	ProjectileFXComponent->SetAsset(ProjectileBodyFX[Team]);
	// }
}
