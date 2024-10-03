// Copyright Surround, Inc. All Rights Reserved.


#include "SMHIC_Character.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMBassCharacter.h"
#include "Actors/Character/Player/SMElectricGuitarCharacter.h"
#include "Actors/Character/Player/SMPianoCharacter.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"


void USMHIC_Character::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, IAmHoldingActor);
}

void USMHIC_Character::BeginPlay()
{
	Super::BeginPlay();

	SourceCharacter = GetOwner<ASMPlayerCharacterBase>();
}

void USMHIC_Character::InitASC(USMAbilitySystemComponent* NewASC)
{
	SourceASC = NewASC;
}

bool USMHIC_Character::CanHolded(AActor* Instigator) const
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !SourceASC.Get())
	{
		return false;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(Instigator);
	if (!TargetCharacter)
	{
		return false;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		return false;
	}

	const ESMTeam TargetTeam = TargetCharacter->GetTeam();
	if (SourceTeam == TargetTeam)
	{
		return false;
	}

	if (HoldedMeCharcters.Find(TargetCharacter) != INDEX_NONE)
	{
		return false;
	}

	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Unholdable))
	{
		return false;
	}

	if (!SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Neutralize))
	{
		return false;
	}

	return true;
}

void USMHIC_Character::OnHolded(AActor* Instigator)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority())
	{
		return;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(Instigator);
	if (!ensureAlways(Instigator))
	{
		return;
	}

	SourceCharacter->ServerSetActorHiddenInGame(true);
	SourceCharacter->ServerSetCollisionEnabled(false);
	SourceCharacter->ServerSetMovementEnabled(false);

	SetActorHoldingMe(Instigator);

	HoldedMeCharcters.Add(TargetCharacter);

	APlayerController* SourcePlayerCharacter = SourceCharacter->GetController<APlayerController>();
	if (ensureAlways(SourcePlayerCharacter))
	{
		SourcePlayerCharacter->SetViewTargetWithBlend(TargetCharacter, 1.0f, VTBlend_Cubic);
	}

	// 잡혔으므로 잠시 잡기 인디케이터를 비활성화합니다.
	SourceCharacter->MulticastRPCRemoveScreenIndicatorToSelf(SourceCharacter);
}

void USMHIC_Character::OnHoldedReleased(AActor* Instigator)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !SourceASC.Get())
	{
		return;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(GetActorHoldingMe());
	USMHIC_Character* TargetHIC = Cast<USMHIC_Character>(USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetCharacter));

	// 잡기 상태에서 벗어납니다.
	HoldedReleased(TargetCharacter, true);

	if (TargetHIC)
	{
		TargetHIC->SetActorIAmHolding(nullptr);
	}
}

void USMHIC_Character::OnNoiseBreakActionStarted(ASMElectricGuitarCharacter* Instigator)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !SourceASC.Get())
	{
		return;
	}

	SourceASC->AddTag(SMTags::Character::State::NoiseBreaked);
}

void USMHIC_Character::OnNoiseBreakActionStarted(ASMPianoCharacter* Instigator)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !SourceASC.Get())
	{
		return;
	}

	SourceASC->AddTag(SMTags::Character::State::NoiseBreaked);
}

void USMHIC_Character::OnNoiseBreakActionStarted(ASMBassCharacter* Instigator)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !SourceASC.Get())
	{
		return;
	}

	SourceASC->AddTag(SMTags::Character::State::NoiseBreaked);
}

void USMHIC_Character::OnNoiseBreakActionPerformed(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !NoiseBreakData || !SourceCapsule)
	{
		return;
	}

	const FVector Offset(0.0, 0.0, SourceCapsule->GetScaledCapsuleHalfHeight());
	SourceCharacter->MulticastRPCSetLocation(NoiseBreakData->NoiseBreakLocation + Offset);

	HoldedReleased(Instigator, false);

	SourceASC->RemoveTag(SMTags::Character::State::NoiseBreaked);
	NoiseBreaked();
}

void USMHIC_Character::OnNoiseBreakActionPerformed(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !NoiseBreakData || !SourceCapsule)
	{
		return;
	}

	const FVector Offset(0.0, 0.0, SourceCapsule->GetScaledCapsuleHalfHeight());
	SourceCharacter->MulticastRPCSetLocation(NoiseBreakData->NoiseBreakLocation + Offset);

	HoldedReleased(Instigator, false);

	SourceASC->RemoveTag(SMTags::Character::State::NoiseBreaked);
	NoiseBreaked();
}

void USMHIC_Character::OnNoiseBreakActionPerformed(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority())
	{
		return;
	}

	HoldedReleased(Instigator, true);

	SourceASC->RemoveTag(SMTags::Character::State::NoiseBreaked);
	NoiseBreaked();
}

void USMHIC_Character::SetActorIAmHolding(AActor* NewIAmHoldingActor)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority())
	{
		return;
	}

	// 중복 실행되어도 안전하도록 값이 다른 경우에만 처리됩니다.
	if (IAmHoldingActor != NewIAmHoldingActor)
	{
		// nullptr이 매개변수로 넘어왔다면 잡기가 해제된 상황으로 구독했던 이벤트를 해제합니다.
		if (IAmHoldingActor.Get() && !NewIAmHoldingActor)
		{
			NET_LOG(GetOwner(), Log, TEXT("%s가 %s를 놓음"), *GetNameSafe(GetOwner()), *GetNameSafe(IAmHoldingActor.Get()));
			IAmHoldingActor->OnDestroyed.RemoveAll(this);
		}

		if (!IAmHoldingActor.Get() || NewIAmHoldingActor)
		{
			NET_LOG(GetOwner(), Log, TEXT("%s가 %s를 잡음"), *GetNameSafe(GetOwner()), *GetNameSafe(NewIAmHoldingActor));
		}

		IAmHoldingActor = NewIAmHoldingActor;
		OnRep_IAmHoldingActor();
		SourceCharacter->ForceNetUpdate();
	}
}

void USMHIC_Character::EmptyHoldedMeCharacterList()
{
	HoldedMeCharcters.Empty();
}

void USMHIC_Character::HoldedReleased(AActor* TargetActor, bool bNeedLocationAdjust)
{
	APlayerController* SourcePlayerController = SourceCharacter ? Cast<APlayerController>(SourceCharacter->Controller) : nullptr;
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !SourceASC.Get() || !ensureAlways(SourcePlayerController))
	{
		return;
	}

	SetActorHoldingMe(nullptr);

	SourceCharacter->ServerSetActorHiddenInGame(false);
	SourceCharacter->ServerSetCollisionEnabled(true);
	SourceCharacter->ServerSetMovementEnabled(true);

	// 회전 및 위치를 재지정합니다.
	FVector NewLocation;
	float NewYaw;
	if (TargetActor)
	{
		NewYaw = TargetActor->GetActorRotation().Yaw;
		NewLocation = TargetActor->GetActorLocation();
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("타겟이 유효하지 않아 자신의 Yaw를 사용합니다."));
		NewYaw = SourceCharacter->GetActorRotation().Yaw;
		NewLocation = SourceCharacter->GetActorLocation();
	}
	const FVector Offset = FRotator(0.0, NewYaw, 0.0).Vector() * 100.0f;
	SourceCharacter->MulticastRPCSetYawRotation(NewYaw);

	if (bNeedLocationAdjust)
	{
		SourceCharacter->MulticastRPCSetLocation(NewLocation + Offset);
	}

	// SourceCharacter->ServerRPCPreventGroundEmbedding();

	// 카메라 뷰를 원래대로 복구합니다.
	SourcePlayerController->SetViewTargetWithBlend(SourceCharacter, 1.0f, VTBlend_Cubic);

	// 자신을 잡았던 대상을 제외하고 다시 잡기 인디케이터를 활성화합니다.
	SourceCharacter->MulticastRPCAddScreenIndicatorToSelf(SourceCharacter);
	for (const auto& HoldedMeCharacter : HoldedMeCharcters)
	{
		HoldedMeCharacter->ClientRPCRemoveScreendIndicatorToSelf(SourceCharacter);
	}
}

void USMHIC_Character::OnDestroyedIAmHoldingActor(AActor* DestroyedActor)
{
	SetActorIAmHolding(nullptr);
}

void USMHIC_Character::OnRep_IAmHoldingActor()
{
	// 잡은 대상이 유효하다면 잡기 태그를 붙이고, 유효하지 않거나 nullptr인 경우 잡기 태그를 제거합니다.
	if (IAmHoldingActor.Get())
	{
		if (SourceASC.Get() && GetOwnerRole() == ROLE_Authority)
		{
			SourceASC->AddTag(SMTags::Character::State::Hold);
		}

		// 로컬 컨트롤인 경우 노이즈 브레이크 인디케이터를 활성화합니다.
		if ((SourceCharacter.Get() && SourceASC.Get()) ? SourceCharacter->IsLocallyControlled() : false)
		{
			SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::NoiseBreakIndicator));
		}

		OnHoldStateEnrty.Broadcast();
	}
	else
	{
		if (SourceASC.Get() && GetOwnerRole() == ROLE_Authority)
		{
			SourceASC->RemoveTag(SMTags::Character::State::Hold);
		}

		/** 잡은 대상을 제거하면 잡기 풀기 델리게이트가 호출됩니다.*/
		OnHoldStateExit.Broadcast();
	}
}

void USMHIC_Character::InternalHolded(AActor* TargetActor)
{
	// SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Holded));
}

void USMHIC_Character::NoiseBreaked()
{
	if (!SourceCharacter.Get() || !SourceASC.Get())
	{
		return;
	}

	// 버저 비터 종료 이벤트를 타겟에게 보냅니다. 이 이벤트는 만약 스턴 종료시간에 임박했을때 스매시를 시전하는 경우 스매시가 끊기는 것을 막고자 스턴 종료를 보류하게됩니다. 이 상황에서 스턴 종료 로직을 재개 시킬때 호출되어야합니다.
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceCharacter, SMTags::Event::Character::BuzzerBeaterEnd, FGameplayEventData());

	// TODO: 나중에 이펙트를 재생해야한다면 활용하면 됩니다.
	// MulticastRPCPlayNoiseBreakedSFX();
}

void USMHIC_Character::MulticastRPCPlayNoiseBreakedSFX_Implementation()
{
	if (!SourceCharacter.Get())
	{
		return;
	}

	USkeletalMeshComponent* SourceMesh = SourceCharacter->GetMesh();
	if (!ensureAlways(SourceMesh))
	{
		return;
	}

	UAnimInstance* AnimInstance = SourceMesh->GetAnimInstance();
	if (!ensureAlways(AnimInstance))
	{
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset();
	if (!SourceDataAsset)
	{
		return;
	}

	// TODO: 이펙트가 필요하다면 재생해줘야합니다.
}
