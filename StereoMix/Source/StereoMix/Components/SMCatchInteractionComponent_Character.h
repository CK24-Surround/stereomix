// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchInteractionComponent.h"
#include "Data/SMTeam.h"
#include "SMCatchInteractionComponent_Character.generated.h"

class USMAbilitySystemComponent;
class ASMPlayerCharacter;

DECLARE_MULTICAST_DELEGATE(FOnCatchSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCatchInteractionComponent_Character : public USMCatchInteractionComponent
{
	GENERATED_BODY()

protected:
	virtual void InitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	void InitASC(USMAbilitySystemComponent* NewASC);

public:
	virtual bool IsCatchable(AActor* TargetActor) const override;

	virtual void OnCaught(AActor* TargetActor) override;

	virtual void OnCaughtReleased(AActor* TargetActor, bool bIsStunEnd) override;

	virtual void OnSpecialActionPerformed(AActor* Instigator, ESpecialAction InSpecialAction) override;

	virtual void OnSpecialActionEnded(AActor* Instigator, ESpecialAction InSpecialAction, float InMagnitude, TSubclassOf<UGameplayEffect> DamageGE, float InDamageAmount) override;

public:
	FORCEINLINE AActor* GetActorIAmCatching() { return IAmCatchingActor.Get(); }

	/** 잡은 대상을 설정할 때 사용합니다. 추가 처리로, 유효한 타겟이면 잡기태그를 추가하고, nullptr이면 잡기 태그를 제거합니다. */
	void SetActorIAmCatching(AActor* NewIAmCatchingActor);

	FORCEINLINE TArray<TWeakObjectPtr<ASMPlayerCharacter>>& GetCapturedMeCharacters() { return CapturedMeCharcters; }

	/**
	 * 타겟으로부터 잡기상태를 탈출합니다. 타겟이 null이면 타겟과 관련된 처리만 제외하고 모두 처리됩니다.
	 * 외부에서도 사용할 수 있습니다. 따로 추가적인 애니메이션 같은 비주얼적인 효과 없이 그저 디태치만 수행합니다.
	 */
	void CaughtReleased(AActor* TargetActor);

protected:
	/** 타겟으로부터 잡히기 상태가 될때 필요한 처리를 합니다. */
	void InternalCaught(AActor* TargetActor);

	/** 타겟으로부터 잡기 상태를 탈출하는데 필요한 처리를 합니다. */
	void InternalCaughtReleased(AActor* TargetActor, bool bIsStunEnd);

	/** 인스티게이터측에서 매치기를 시전할때 필요한 처리를 합니다.*/
	void InternalSmashed();

	/** 인스티게이터측에서 매치기를 종료할때 필요한 처리를 합니다.*/
	void InternalSmashedEnded(AActor* Instigator, float Magnitude, TSubclassOf<UGameplayEffect> DamageGE, float DamageAmount);

	/** 타일을 트리거합니다. 내부적으로 트리거 GA가 활성화됩니다. */
	void InternalTileTriggerdBySmash(AActor* Instigator, float Magnitude, TSubclassOf<UGameplayEffect> DamageGE, float DamageAmount);

	/** 스플래시 대미지를 처리합니다. */
	void InternalApplySmashSplashDamage(const FVector& TileLocation, float TileHorizonSize, float Magnitude, AActor* Instigator, TSubclassOf<UGameplayEffect> DamageGE, float DamageAmount);

	/** 스플래시 대미지를 받을 수 있는 타겟인지 검사합니다. */
	bool IsValidateTargetForSmashSplashDamage(const ASMPlayerCharacter* TargetActor, ESMTeam InstigatorTeam);

public:
	/** 잡을때 브로드캐스트합니다. */
	FOnCatchSignature OnCatch;

	/** 잡기를 놓을때 브로드캐스트합니다.*/
	FOnCatchSignature OnCatchRelease;

protected:
	UFUNCTION()
	void OnRep_IAmCatchingActor();

protected:
	/** 자신이 잡고 있는 액터입니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_IAmCatchingActor")
	TWeakObjectPtr<AActor> IAmCatchingActor;

	/** 한 캐릭터에게 여러번 잡히지 않도록 자신을 잡았던 캐릭터들을 담아둡니다. 서버에서만 유효합니다. */
	TArray<TWeakObjectPtr<ASMPlayerCharacter>> CapturedMeCharcters;

protected:
	TObjectPtr<ASMPlayerCharacter> SourceCharacter;
	TObjectPtr<USMAbilitySystemComponent> SourceASC;
};
