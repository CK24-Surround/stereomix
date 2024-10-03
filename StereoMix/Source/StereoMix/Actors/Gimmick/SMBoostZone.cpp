// Copyright Surround, Inc. All Rights Reserved.


#include "SMBoostZone.h"

#include "Components/BoxComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMBoostZone::ASMBoostZone()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Gimmick);
}

void ASMBoostZone::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASMBoostZone::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void ASMBoostZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 매 틱마다 부스트존 내의 캐릭터들의 상태를 추적하고 필요한 로직을 처리해줍니다.
	// 이 캐릭터들은 Map에 오버렙 시작 시 추가되고, 종료 시 제거됩니다.
	// for (const auto& InZonePair : InZoneMap)
	// {
	// 	if (InZonePair.Key.Get())
	// 	{
	// 		PerformBoostZone(InZonePair.Key.Get());
	// 	}
	// }
}

void ASMBoostZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(OtherActor);
	// if (!TargetCharacter)
	// {
	// 	return;
	// }
	//
	// // 만약 틱이 비활성화되어 있다면 틱을 활성화합니다. 즉 평상시에는 비활성화해 자원을 아깝니다.
	// if (!IsActorTickEnabled())
	// {
	// 	SetActorTickEnabled(true);
	// }
	//
	// // 캐릭터를 부스트존에 추가합니다.
	// InZoneMap.FindOrAdd(TargetCharacter);
}

void ASMBoostZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	// ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(OtherActor);
	// if (!TargetCharacter)
	// {
	// 	return;
	// }
	//
	// if (InZoneMap[TargetCharacter].bNewIsCurrectDirection)
	// {
	// 	RemoveBoostZone(TargetCharacter);
	// }
	//
	// // 캐릭터를 부스트존에서 제거합니다.
	// InZoneMap.Remove(TargetCharacter);
	//
	// // 부스트 존 내의 캐릭터가 수가 0이라면 틱을 비활성화합니다.
	// if (InZoneMap.Num() <= 0)
	// {
	// 	SetActorTickEnabled(false);
	// }
}

void ASMBoostZone::PerformBoostZone(ASMPlayerCharacter* TargetCharacter)
{
	// if (!TargetCharacter)
	// {
	// 	return;
	// }
	//
	// UCharacterMovementComponent* TargetMovement = TargetCharacter->GetCharacterMovement();
	// if (!ensure(TargetMovement))
	// {
	// 	return;
	// }
	//
	// // 부스트존의 방향과 일치하면 캐릭터를 가속, 불일치하면 원상복구 시킵니다.
	// FBoostZoneDirectionData& TargetBoostZoneDirectionData = InZoneMap[TargetCharacter];
	// const FVector BoostZoneDirection = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X);
	// const FVector TargetMoveDirection = TargetMovement->GetCurrentAcceleration().GetSafeNormal();
	// if (BoostZoneDirection.Dot(TargetMoveDirection) > 0.0f)
	// {
	// 	TargetBoostZoneDirectionData.bNewIsCurrectDirection = true;
	// 	if (TargetBoostZoneDirectionData.bOldIsCurrectDirection != TargetBoostZoneDirectionData.bNewIsCurrectDirection)
	// 	{
	// 		ApplyBoostZone(TargetCharacter);
	// 	}
	//
	// 	TargetBoostZoneDirectionData.bOldIsCurrectDirection = true;
	// }
	// else
	// {
	// 	TargetBoostZoneDirectionData.bNewIsCurrectDirection = false;
	// 	if (TargetBoostZoneDirectionData.bOldIsCurrectDirection != TargetBoostZoneDirectionData.bNewIsCurrectDirection)
	// 	{
	// 		RemoveBoostZone(TargetCharacter);
	// 	}
	//
	// 	TargetBoostZoneDirectionData.bOldIsCurrectDirection = false;
	// }
}

void ASMBoostZone::ApplyBoostZone(ASMPlayerCharacter* TargetCharacter)
{
	// if (!TargetCharacter)
	// {
	// 	return;
	// }
	//
	// UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
	// if (!ensureAlways(TargetASC))
	// {
	// 	return;
	// }
	//
	// const USMCharacterAttributeSet* TargetAttributeSet = TargetASC->GetSet<USMCharacterAttributeSet>();
	// if (!ensureAlways(TargetAttributeSet))
	// {
	// 	return;
	// }
	//
	// // 더해저야할 이동속도를 구합니다.
	// const float MoveSpeedToAdd = (TargetAttributeSet->GetBaseMoveSpeed() * MoveSpeedToApplyMultiply) - TargetAttributeSet->GetBaseMoveSpeed();
	// AddMoveSpeed(TargetCharacter, MoveSpeedToAdd);
}

void ASMBoostZone::RemoveBoostZone(ASMPlayerCharacter* TargetCharacter)
{
	// if (!TargetCharacter)
	// {
	// 	return;
	// }
	//
	// UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
	// if (!ensureAlways(TargetASC))
	// {
	// 	return;
	// }
	//
	// const USMCharacterAttributeSet* TargetAttributeSet = TargetASC->GetSet<USMCharacterAttributeSet>();
	// if (!ensureAlways(TargetAttributeSet))
	// {
	// 	return;
	// }
	//
	// // 감소되야할 이동속도를 구합니다.
	// const float MoveSpeedToAdd = (TargetAttributeSet->GetBaseMoveSpeed() * MoveSpeedToApplyMultiply) - TargetAttributeSet->GetBaseMoveSpeed();
	// AddMoveSpeed(TargetCharacter, -MoveSpeedToAdd);
}

void ASMBoostZone::AddMoveSpeed(ASMPlayerCharacter* TargetCharacter, float MoveSpeedToAdd)
{
	// if (!TargetCharacter)
	// {
	// 	return;
	// }
	//
	// UCharacterMovementComponent* TargetMovement = TargetCharacter->GetCharacterMovement();
	// if (!ensureAlways(TargetMovement))
	// {
	// 	return;
	// }
	//
	// // 클라이언트인 경우 예측 적용합니다.
	// if (TargetCharacter->IsLocallyControlled())
	// {
	// 	TargetMovement->MaxWalkSpeed += MoveSpeedToAdd;
	// }
	//
	// // 서버인 경우 실제로 적용합니다.
	// if (TargetCharacter->HasAuthority())
	// {
	// 	TargetCharacter->SetMaxWalkSpeed(TargetMovement->MaxWalkSpeed + MoveSpeedToAdd);
	// }
}
