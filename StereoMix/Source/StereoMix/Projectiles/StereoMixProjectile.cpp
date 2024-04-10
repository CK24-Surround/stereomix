// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Data/StereoMixDesignData.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Utilities/StereoMixAssetPath.h"
#include "Utilities/StereoMixCollision.h"
#include "Utilities/StereoMixLog.h"

AStereoMixProjectile::AStereoMixProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStereoMixDesignData> DesignDataRef(StereoMixAssetPath::DesignData);
	if (DesignDataRef.Object)
	{
		DesignData = DesignDataRef.Object;
	}
	else
	{
		NET_LOG(nullptr, Error, TEXT("DesignData 로드에 실패했습니다."));
	}

	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetCollisionProfileName(StereoMixCollisionProfileName::Projectile);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionProfileName(StereoMixCollisionProfileName::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = DesignData->ProjectileSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->SetAutoActivate(true);
}

void AStereoMixProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!HasAuthority())
	{
		// SphereComponent->SetCollisionProfileName(StereoMixCollisionProfileName::NoCollision);
	}
}

void AStereoMixProjectile::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

void AStereoMixProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float DistanceSquaredFromStartLocation = FVector::DistSquared(StartLocation, GetActorLocation());
	if (DistanceSquaredFromStartLocation > FMath::Square(DesignData->ProjectileMaxDistance))
	{
		if (HasAuthority())
		{
			Destroy();
		}
		else
		{
			SetActorHiddenInGame(true);
		}
	}
}

void AStereoMixProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor == GetOwner())
	{
		return;
	}

	if (HasAuthority())
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (TargetASC)
		{
			const FGameplayEffectContextHandle GEContext = TargetASC->MakeEffectContext();
			if (GEContext.IsValid())
			{
				TargetASC->BP_ApplyGameplayEffectToSelf(HitGE, 0, GEContext);
				Destroy();
			}
		}
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void AStereoMixProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (HasAuthority())
	{
		Destroy();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}
