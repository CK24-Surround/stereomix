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
	ProjectileFXComponent->SetAutoActivate(false);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->SetAutoActivate(false);

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));

	ProjectileBodyFX.Add(ESMTeam::None, nullptr);
	ProjectileBodyFX.Add(ESMTeam::EDM, nullptr);
	ProjectileBodyFX.Add(ESMTeam::FutureBass, nullptr);

	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Stun);
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

void ASMProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// 유효한 타겟인지 검증합니다.
	if (!IsValidateTarget(OtherActor))
	{
		return;
	}

	if (HasAuthority())
	{
		ApplyDamage(OtherActor);
		ApplyFX(OtherActor);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		MulticastRPCEndLifeTime();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void ASMProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (HasAuthority())
	{
		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		MulticastRPCEndLifeTime();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void ASMProjectile::Launch(AActor* NewOwner, const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InNormal, float InSpeed, float InMaxDistance, float InDamage)
{
	if (!HasAuthority())
	{
		return;
	}

	// 오너와 팀을 지정해줍니다.
	// TODO: 추후 오너대신 ASC를 WeakObjectPtr로 갖게 할 계획입니다.
	SetOwner(NewOwner);

	ISMTeamComponentInterface* OwnerTeamComponentInterface = Cast<ISMTeamComponentInterface>(NewOwner);
	if (OwnerTeamComponentInterface)
	{
		USMTeamComponent* OwnerTeamComponent = OwnerTeamComponentInterface->GetTeamComponent();
		if (ensure(OwnerTeamComponent))
		{
			TeamComponent->SetTeam(OwnerTeamComponent->GetTeam());
		}
	}

	// 투사체의 데미지는 서버에서만 쓰이니 미리 저장해줍니다.
	Damage = InDamage;
	MulticastRPCLaunch(InStartLocation, InNormal, InSpeed, InMaxDistance);
}

void ASMProjectile::MulticastRPCLaunch_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InNormal, float InSpeed, float InMaxDistance)
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
	ProjectileFXComponent->ActivateSystem();
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
}

void ASMProjectile::MulticastRPCEndLifeTime_Implementation()
{
	ProjectileFXComponent->DeactivateImmediate();
	ProjectileMovementComponent->Deactivate();
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

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

bool ASMProjectile::IsValidateTarget(AActor* InTarget)
{
	// 자신이 발사한 투사체에 적중되지 않도록 합니다. (자살 방지)
	if (InTarget == GetOwner())
	{
		return false;
	}

	// 투사체가 무소속인 경우 무시합니다.
	const ESMTeam SourceTeam = TeamComponent->GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		return false;
	}

	ISMTeamComponentInterface* TargetTeamComponentInterface = Cast<ISMTeamComponentInterface>(InTarget);
	if (!TargetTeamComponentInterface)
	{
		return false;
	}

	USMTeamComponent* OtherTeamComponent = TargetTeamComponentInterface->GetTeamComponent();
	if (!ensureAlways(OtherTeamComponent))
	{
		return false;
	}

	// 타겟이 무소속인 경우 무시합니다.
	const ESMTeam OtherTeam = OtherTeamComponent->GetTeam();
	if (OtherTeam == ESMTeam::None)
	{
		return false;
	}

	// 투사체와 타겟이 같은 팀이라면 무시합니다. (팀킬 방지)
	if (SourceTeam == OtherTeam)
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

void ASMProjectile::ApplyDamage(AActor* InTarget)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget);
	if (!ensureAlways(TargetASC))
	{
		return;
	}

	const FGameplayEffectContextHandle GEContext = TargetASC->MakeEffectContext();
	if (!ensureAlways(GEContext.IsValid()))
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(DamageGE, 1.0f, SourceASC->MakeEffectContext());
	if (!ensureAlways(GESpecHandle.IsValid()))
	{
		return;
	}

	FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
	if (!ensureAlways(GESpec))
	{
		return;
	}

	// SetByCaller를 통해 매개변수로 전달받은 Damage로 GE를 적용합니다.
	GESpec->SetSetByCallerMagnitude(SMTags::Data::Damage, Damage);
	SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);
}

void ASMProjectile::ApplyFX(AActor* InTarget)
{
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget);
	if (!ensureAlways(TargetASC))
	{
		return;
	}

	const AActor* TargetActor = TargetASC->GetAvatarActor();
	if (!ensureAlways(TargetActor))
	{
		return;
	}

	// 적중한 타겟에게 FX를 재생시킵니다.
	FGameplayCueParameters GCParams;
	GCParams.Location = TargetActor->GetActorLocation();
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileHit, GCParams);
}

void ASMProjectile::OnChangeTeamCallback()
{
	const ESMTeam Team = TeamComponent->GetTeam();

	if (!HasAuthority())
	{
		ProjectileFXComponent->SetAsset(ProjectileBodyFX[Team]);
	}
}
