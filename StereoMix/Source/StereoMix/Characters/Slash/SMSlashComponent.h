// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SMSlashComponent.generated.h"


class UAnimInstance;
class UGameplayEffect;
class UCapsuleComponent;
class ASMBassCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMSlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMSlashComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

	/** 베기를 시도하고 현재 상황에 맞는 베기 로직을 수행합니다. */
	void TrySlash();

	/** 베기 콜라이더를 현재 상황에 맞는 방향으로 회전시킵니다. */
	void ColliderOrientaionForSlash();

	/** 다음 공격이 좌베기인지 여부를 나타냅니다. */
	uint32 bIsLeftSlashNext:1 = true;

	/** 입력을 받을 수 있는지 여부를 나타냅니다. */
	uint32 bCanInput:1 = false;

	/** 다음 공격으로 진행될 수 있는지 여부를 나타냅니다. */
	uint32 bCanNextAction:1 = false;

	/** 유효한 입력이 입력되었는지 여부를 나타냅니다. */
	uint32 bIsInput:1 = false;

protected:
	/** 베기 애니메이션을 시작합니다. 베기를 시작할때 호출됩니다. */
	void SlashStart();

	/** 애니메이션 섹션을 점프시켜 현재 상황에 맞는 베기 애니메이션을 재생시킵니다. 첫 베기를 제외하고 매 베기 시전시마다 호출됩니다.*/
	void ReSlash();

	/** 베기 콜라이더를 회전시키는 동안 호출됩니다. 실제 회전 로직을 수행합니다. */
	void UpdateSlash();

	/** 베기 애니메이션이 종료될때 호출됩니다. 베기 시스템의 변수 정리에 사용됩니다. */
	void SlashEnded(UAnimMontage* AnimMontage, bool bArg);

	/** 베기 콜라이더가 회전하는 도중 오버랩된 대상을 체크하고 대미지를 주는데 사용합니다. */
	UFUNCTION()
	void OnSlashOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 시전자의 클라이언트 환경에서 미리 상대에게 준 데미지 만큼 UI상에 미리 반영하여 Ping문제를 해결하는데 사용합니다. */
	void PredictApplyDamage(AActor* TargetActor);

	/** 서버에게 타겟에게 대미지 적용을 요청합니다. */
	UFUNCTION(Server, Reliable)
	void ServerRPCRequestDamage(AActor* TargetActor, float Amount);

	/** 시작 애니메이션 리플리케이트를 위한 서버 RPC입니다. */
	UFUNCTION(Server, Reliable)
	void ServerRPCPlaySlashStartAnimation();

	/** 시작 애니메이션 리플리케이트를 위한 멀티캐스트 RPC입니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlaySlashStartAnimation();

	/** ReSlash 애니메이션 리플리케이트를 위한 서버 RPC입니다. */
	UFUNCTION(Server, Reliable)
	void ServerRPCPlayReSlashAnimation(bool bIsLeftSlash);

	/** ReSlash 애니메이션 리플리케이트를 위한 멀티캐스트 RPC입니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlaySlashAnimation(bool bIsLeftSlash);

	/** 소스 캐릭터의 애님 인스턴스를 가져오기 위한 함수입니다. */
	UAnimInstance* GetSourceAnimInstance();

	/** 대미지를 적용할 수 있는 대상인지 체크합니다. */
	bool IsValidTarget(AActor* TargetActor);

	/** 베기 대미지 적용을 위한 GE입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<UGameplayEffect> SlashDamageGE;

	/** 베기 애니메이션 몽타주입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UAnimMontage> SlashMontage;

	/** 베기의 데미지량입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float Damage = 25.0f;

	/** 베기의 사정거리입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float Distance = 500.0f;

	/** 베기의 전방기준 각도입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float Angle = 240.0f;

	/** 베기를 수행하는 동안 소요될 시간입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float SlashTime = 0.1875f;

	/** 콜라이더의 회전을 보여줄지 여부입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bShowDebug:1 = false;

	/** 베기를 시전할 수 없는 상태 태그리스트입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTagContainer DeactivateGameplayTags;

	/** 캐싱해둔 소스캐릭터입니다. 틱 함수에서 사용하기 위해 캐싱했습니다. */
	TSoftObjectPtr<ASMBassCharacter> SourceCharacter;

	/** 캐싱해둔 소스캐릭터의 베기 콜라이더 루트입니다. 틱 함수에서 사용하기 위해 캐싱했습니다. 회전할때 사용됩니다. */
	TSoftObjectPtr<USceneComponent> SourceSlashColliderRootComponent;

	/** 캐싱해둔 소스캐릭터의 베기 콜라이더입니다. 틱 함수에서 사용하기 위해 캐싱했습니다. */
	TSoftObjectPtr<UCapsuleComponent> SourceSlashColliderComponent;

	/** 타겟이 콜라이더 회전에 여러번 오버랩 될 수 있는데 이를 방지하기 위해 사용되는 배열입니다. */
	TArray<TSoftObjectPtr<AActor>> DetectedActors;

	/** 콜라이더 회전에서 사용할 데이터입니다. 초당 몇의 각도로 Yaw를 회전시켜야할지를 저장합니다. */
	float SlashSpeed = 0.0f;

	/** 콜라이더 회전에서 사용할 데이터입니다. 시작 각도를 저장합니다. */
	float StartAngle = 0.0f;

	/** 현재 베기를 수행중인지 여부를 나타냅니다. */
	uint32 bIsSlashing:1 = false;

	/** 베기 콜라이더 회전을 해야하는지 여부를 나타냅니다. */
	uint32 bNeedUpdateSlashing:1 = false;
};
