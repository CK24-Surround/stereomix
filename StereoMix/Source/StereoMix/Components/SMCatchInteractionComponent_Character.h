// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchInteractionComponent.h"
#include "SMCatchInteractionComponent_Character.generated.h"

class USMAbilitySystemComponent;
class ASMPlayerCharacter;

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
	virtual bool IsCatchable(AActor* TargetActor) const override;

	virtual bool OnCaught(AActor* TargetActor) override;

	virtual bool OnCaughtReleased(AActor* TargetActor, bool bIsStunEnd) override;

	virtual void OnSpecialActionPerformed(AActor* TargetActor) override;

	virtual void OnSpecialActionEnded(AActor* TargetActor) override;

public:
	FORCEINLINE AActor* GetActorIAmCatching() { return IAmCatchingActor.Get(); }

	FORCEINLINE void SetActorIAmCatching(AActor* NewIAmCathingActor) { IAmCatchingActor = NewIAmCathingActor; }

	FORCEINLINE TArray<TWeakObjectPtr<ASMPlayerCharacter>>& GetCapturedMeCharcters() { return CapturedMeCharcters; }

public:
	/** 타겟으로 부터 잡기상태를 탈출합니다. 타겟이 null이면 타겟과 관련된 처리만 제외하고 모두 처리됩니다. */
	bool HandleCaughtReleased(AActor* TargetActor);

protected:
	/** 자신이 잡고 있는 액터입니다. */
	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> IAmCatchingActor;

	/** 한 캐릭터에게 여러번 잡히지 않도록 자신을 잡았던 캐릭터들을 담아둡니다. 서버에서만 유효합니다. */
	TArray<TWeakObjectPtr<ASMPlayerCharacter>> CapturedMeCharcters;

protected:
	TObjectPtr<ASMPlayerCharacter> SourceCharacter;
	TObjectPtr<USMAbilitySystemComponent> SourceASC;
};
