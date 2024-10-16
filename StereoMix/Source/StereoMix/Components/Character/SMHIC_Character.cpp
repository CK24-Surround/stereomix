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
#include "Controllers/SMGamePlayerController.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
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

void USMHIC_Character::SetASC(USMAbilitySystemComponent* NewASC)
{
	SourceASC = NewASC;
}

bool USMHIC_Character::CanBeHeld(AActor* Instigator) const
{
	if (!SourceCharacter || !SourceCharacter->HasAuthority() || !SourceASC || !Instigator)
	{
		return false;
	}

	if (USMTeamBlueprintLibrary::IsSameTeam(SourceCharacter, Instigator))
	{
		return false;
	}

	if (HeldMeActors.Find(Instigator) != INDEX_NONE)
	{
		return false;
	}

	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Common::NonHoldable))
	{
		return false;
	}

	if (!SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Common::Neutralized))
	{
		return false;
	}

	return true;
}

void USMHIC_Character::OnHeld(AActor* Instigator)
{
	if (!SourceCharacter || !SourceCharacter->HasAuthority())
	{
		return;
	}

	// 캐릭터를 숨기고 콜리전, 움직임을 비활성화합니다.
	SourceCharacter->ServerSetActorHiddenInGame(true);
	SourceCharacter->ServerSetCollisionEnabled(false);
	SourceCharacter->ServerSetMovementEnabled(false);

	if (Instigator)
	{
		SourceCharacter->AttachToActor(Instigator, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		HeldMeActors.Add(Instigator);
		SetActorHoldingMe(Instigator);
	}

	// 잡혔으므로 잠시 잡기 인디케이터를 비활성화합니다.
	SourceCharacter->MulticastRemoveScreenIndicatorToSelf(SourceCharacter);
}

void USMHIC_Character::OnReleasedFromHold(AActor* Instigator)
{
	if (!SourceCharacter || !SourceCharacter->HasAuthority() || !SourceASC)
	{
		return;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(GetActorHoldingMe());
	USMHIC_Character* TargetHIC = Cast<USMHIC_Character>(USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetCharacter));

	// 잡기 상태에서 벗어납니다.
	ReleasedFromBeingHeld(TargetCharacter);

	if (TargetHIC)
	{
		TargetHIC->SetActorIAmHolding(nullptr);
	}
}

void USMHIC_Character::OnNoiseBreakStarted(AActor* Instigator)
{
	if (!SourceCharacter || !SourceCharacter->HasAuthority() || !SourceASC)
	{
		return;
	}

	SourceASC->AddTag(SMTags::Character::State::Common::NoiseBreaked);
}

void USMHIC_Character::OnNoiseBreakApplied(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter || !SourceCharacter->HasAuthority() || !NoiseBreakData || !SourceCapsule)
	{
		return;
	}

	const FVector Offset(0.0, 0.0, SourceCapsule->GetScaledCapsuleHalfHeight());
	const FVector NewLocation = NoiseBreakData->NoiseBreakLocation + Offset;
	ReleasedFromBeingHeld(Instigator, NewLocation);

	// SourceCharacter->MulticastRPCSetLocation(NoiseBreakData->NoiseBreakLocation + Offset);

	SourceASC->RemoveTag(SMTags::Character::State::Common::NoiseBreaked);
	NoiseBreaked();
}

void USMHIC_Character::OnNoiseBreakApplied(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority() || !NoiseBreakData || !SourceCapsule)
	{
		return;
	}

	const FVector Offset(0.0, 0.0, SourceCapsule->GetScaledCapsuleHalfHeight());
	const FVector NewLocation = NoiseBreakData->NoiseBreakLocation + Offset;
	ReleasedFromBeingHeld(Instigator, NewLocation);

	// SourceCharacter->MulticastRPCSetLocation(NoiseBreakData->NoiseBreakLocation + Offset);

	SourceASC->RemoveTag(SMTags::Character::State::Common::NoiseBreaked);
	NoiseBreaked();
}

void USMHIC_Character::OnNoiseBreakApplied(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	if (!SourceCharacter.Get() || !SourceCharacter->HasAuthority())
	{
		return;
	}

	ReleasedFromBeingHeld(Instigator);

	SourceASC->RemoveTag(SMTags::Character::State::Common::NoiseBreaked);
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

void USMHIC_Character::EmptyHeldMeCharacterList()
{
	HeldMeActors.Empty();
}

void USMHIC_Character::ReleasedFromBeingHeld(AActor* TargetActor, const TOptional<FVector>& TargetOptionalLocation)
{
	if (!SourceCharacter || !SourceCharacter->HasAuthority() || !SourceASC)
	{
		return;
	}

	SourceCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 타겟 캐릭터가 있으면 타겟의 회전값을 사용하고, 없으면 소스 캐릭터의 현재 회전값을 사용하여 소스 캐릭터의 최종 회전을 설정합니다.
	const float FinalSourceYaw = TargetActor ? TargetActor->GetActorRotation().Yaw : SourceCharacter->GetActorRotation().Yaw;
	SourceCharacter->MulticastRPCSetYawRotation(FinalSourceYaw);

	// 타겟의 앞으로 위치를 조정합니다. 만약 옵션 위치가 제공되었다면 이를 사용합니다.
	const FVector TargetLocation = TargetActor ? TargetActor->GetActorLocation() : SourceCharacter->GetActorLocation();
	const FVector FinalSourceLocation = TargetOptionalLocation.Get(TargetLocation);
	SourceCharacter->MulticastRPCSetLocation(FinalSourceLocation);

	// 자신을 잡았던 대상을 제외하고 다시 잡기 인디케이터를 활성화합니다.
	SourceCharacter->MulticastAddScreenIndicatorToSelf(SourceCharacter);
	for (const auto& HoldedMeActor : HeldMeActors)
	{
		if (ASMPlayerCharacterBase* HoldedMeCharacter = Cast<ASMPlayerCharacterBase>(HoldedMeActor))
		{
			HoldedMeCharacter->ClientRemoveScreenIndicatorToSelf(SourceCharacter);
		}
	}

	// 콜라이더와 움직임을 활성화하고 캐릭터가 보이도록 설정합니다.
	SourceCharacter->ServerSetActorHiddenInGame(false);
	SourceCharacter->ServerSetCollisionEnabled(true);
	SourceCharacter->ServerSetMovementEnabled(true);

	SetActorHoldingMe(nullptr); // nullptr를 매개변수로 넘겨 자신을 잡고 있는 액터를 떼어냅니다.
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
			SourceASC->AddTag(SMTags::Character::State::Common::Hold);
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
			SourceASC->RemoveTag(SMTags::Character::State::Common::Hold);
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
