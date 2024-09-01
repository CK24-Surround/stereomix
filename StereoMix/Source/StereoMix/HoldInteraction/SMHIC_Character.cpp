// Copyright Surround, Inc. All Rights Reserved.


#include "SMHIC_Character.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMBassCharacter.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "Utilities/SMLog.h"


void USMHIC_Character::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
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

bool USMHIC_Character::CanHolded(AActor* TargetActor) const
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority())
	{
		return false;
	}

	if (!SourceASC.Get())
	{
		return false;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!ensureAlways(TargetCharacter))
	{
		return false;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		return false;
	}

	if (SourceTeam == TargetCharacter->GetTeam())
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

void USMHIC_Character::OnHolded(AActor* TargetActor)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority())
	{
		return;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!ensureAlways(TargetActor))
	{
		return;
	}

	SourceCharacter->SetCharacterHidden(true);
	SourceCharacter->SetCollisionEnable(false);
	SourceCharacter->SetMovementEnable(false);

	SetActorHoldingMe(TargetActor);
	SourceASC->AddTag(SMTags::Character::State::Holded);

	HoldedMeCharcters.Add(TargetCharacter);

	APlayerController* SourcePlayerCharacter = SourceCharacter->GetController<APlayerController>();
	if (ensureAlways(SourcePlayerCharacter))
	{
		SourcePlayerCharacter->SetViewTargetWithBlend(TargetCharacter, 1.0f, VTBlend_Cubic);
	}
}

void USMHIC_Character::OnHoldedReleased(AActor* TargetActor, bool bIsStunTimeOut)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !SourceASC.Get())
	{
		return;
	}

	AActor* TargetCharacter = GetActorHoldingMe();
	USMHIC_Character* TargetHIC = Cast<USMHIC_Character>(USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetCharacter));

	// 잡기 상태에서 벗어납니다.
	HoldedReleased(TargetCharacter);

	if (TargetHIC)
	{
		TargetHIC->SetActorIAmHolding(nullptr);
	}

	// TODO: 더 이상 무력화 탈출을 GA로 처리하지 않을 예정입니다.
	if (!bIsStunTimeOut)
	{
		// SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::HoldedExit));
	}
	else
	{
		// SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::HoldedExitOnStunEnd));
	}
}

void USMHIC_Character::OnNoiseBreakActionStarted(ASMElectricGuitarCharacter* Instigator) {}

void USMHIC_Character::OnNoiseBreakActionStarted(ASMPianoCharacter* Instigator) {}

void USMHIC_Character::OnNoiseBreakActionStarted(ASMBassCharacter* Instigator)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !SourceASC.Get())
	{
		return;
	}

	SourceASC->AddTag(SMTags::Character::State::NoiseBreaked);
}

void USMHIC_Character::OnNoiseBreakActionPerformed(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) {}

void USMHIC_Character::OnNoiseBreakActionPerformed(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) {}

void USMHIC_Character::OnNoiseBreakActionPerformed(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	// TODO: 타일 점령 및 데미지 구현 필요

	HoldedReleased(Instigator);

	SourceASC->RemoveTag(SMTags::Character::State::Smashed);
	SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Smashed));
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
			IAmHoldingActor->OnDestroyed.RemoveAll(this);
		}

		IAmHoldingActor = NewIAmHoldingActor;
		OnRep_IAmHoldingActor();
	}
}

void USMHIC_Character::EmptyHoldedMeCharacterList()
{
	HoldedMeCharcters.Empty();
}

void USMHIC_Character::HoldedReleased(AActor* TargetActor)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !SourceASC.Get())
	{
		return;
	}

	SetActorHoldingMe(nullptr);
	SourceASC->RemoveTag(SMTags::Character::State::Holded);

	SourceCharacter->SetCharacterHidden(false);
	SourceCharacter->SetCollisionEnable(true);
	SourceCharacter->SetMovementEnable(true);

	// 회전및 위치를 재지정합니다.
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
	SourceCharacter->MulticastRPCSetYawRotation(NewYaw);
	SourceCharacter->SetActorLocation(NewLocation);

	// SourceCharacter->ServerRPCPreventGroundEmbedding();

	// 카메라 뷰를 원래대로 복구합니다.
	APlayerController* SourcePlayerController = Cast<APlayerController>(SourceCharacter->Controller);
	if (ensureAlways(SourcePlayerController))
	{
		SourcePlayerController->SetViewTargetWithBlend(SourceCharacter, 1.0f, VTBlend_Cubic);
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
		if (GetOwnerRole() == ROLE_Authority)
		{
			SourceASC->AddTag(SMTags::Character::State::Hold);
		}

		OnCatch.Broadcast();
	}
	else
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			SourceASC->RemoveTag(SMTags::Character::State::Hold);
		}

		/** 잡은 대상을 제거하면 잡기 풀기 델리게이트가 호출됩니다.*/
		OnCatchRelease.Broadcast();
	}
}

void USMHIC_Character::InternalHolded(AActor* TargetActor)
{
	// SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Holded));
}
