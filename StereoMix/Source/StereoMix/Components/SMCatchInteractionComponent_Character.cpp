// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchInteractionComponent_Character.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


void USMCatchInteractionComponent_Character::InitializeComponent()
{
	Super::InitializeComponent();

	SourceCharacter = CastChecked<ASMPlayerCharacter>(GetOwner());
}

void USMCatchInteractionComponent_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, IAmCatchingActor);
}

void USMCatchInteractionComponent_Character::BeginPlay()
{
	Super::BeginPlay();

	SourceASC = CastChecked<USMAbilitySystemComponent>(SourceCharacter->GetAbilitySystemComponent());
}

bool USMCatchInteractionComponent_Character::IsCatchble(AActor* TargetActor) const
{
	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(TargetActor);
	if (!ensureAlways(TargetCharacter))
	{
		return false;
	}

	// 무소속이라면 잡을 수 없습니다.
	// 사실상 무소속은 대미지를 받지 않기 때문에 기절할 일은 없긴하지만 만약을 위한 예외처리입니다.
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		return false;
	}

	// 팀이 같다면 잡을 수 없습니다.
	if (SourceTeam == TargetCharacter->GetTeam())
	{
		return false;
	}

	// 자신을 잡았던 캐릭터들을 순회하며 한 번이라도 자신을 잡았던 대상인지 확인합니다.
	bool bIsCaptureCharacter = false;
	for (const auto& CapturedMeCharcter : CapturedMeCharcters)
	{
		if (CapturedMeCharcter.Get())
		{
			if (CapturedMeCharcter == TargetCharacter)
			{
				bIsCaptureCharacter = true;
				break;
			}
		}
	}

	// 자신을 잡았던 타겟이라면 다시 잡을 수 없습니다.
	if (bIsCaptureCharacter)
	{
		return false;
	}

	// 잡을 수 없는 상태 태그가 부착되어 있다면 잡을 수 없습니다.
	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Uncatchable))
	{
		return false;
	}

	return true;
}

bool USMCatchInteractionComponent_Character::OnCaught(AActor* TargetActor)
{
	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(TargetActor);
	if (!ensureAlways(TargetActor))
	{
		return false;
	}

	// 대상에게 어태치합니다. 어태치 후 상대 회전을 0으로 정렬해줍니다.
	const bool bAttachSuccess = SourceCharacter->AttachToComponent(TargetCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("CatchSlotSocket"));
	if (!ensureAlways(bAttachSuccess))
	{
		return false;
	}

	// 자신을 잡은 캐릭터를 저장하고 잡힌 상태 태그를 부착합니다.
	SetActorCatchingMe(TargetActor);
	SourceASC->AddTag(SMTags::Character::State::Caught);

	// 자신을 잡았던 캐릭터 목록을 업데이트 해줍니다.
	CapturedMeCharcters.Add(TargetCharacter);

	// 캐릭터 상태 위젯을 숨깁니다.
	SourceCharacter->SetCharacterStateVisibility(false);

	// 충돌판정, 움직임을 잠급니다.
	SourceCharacter->SetEnableCollision(false);
	SourceCharacter->SetEnableMovement(false);

	// 위치 보정 무시를 활성화합니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensureAlways(SourceMovement))
	{
		SourceMovement->bIgnoreClientMovementErrorChecksAndCorrection = true;
	}

	// 루트와 메시 사이에 생기는 오프셋 차이를 수정합니다.
	SourceCharacter->MulticastRPCApplyAttachOffset();

	// 플레이어의 카메라 뷰를 타겟의 카메라 뷰로 전환합니다.
	APlayerController* SourcePlayerController = Cast<APlayerController>(SourceCharacter->Controller);
	if (ensureAlways(SourcePlayerController))
	{
		SourcePlayerController->SetViewTargetWithBlend(TargetCharacter, 1.0f, VTBlend_Cubic);
	}

	// 잡히기 GA를 활성화합니다.
	SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Caught));

	return true;
}

bool USMCatchInteractionComponent_Character::OnCaughtReleased(AActor* TargetActor, bool bIsStunEnd)
{
	if (!bIsStunEnd)
	{
		if (SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::CaughtExit)))
		{
			return true;
		}
	}

	return false;
}

void USMCatchInteractionComponent_Character::OnSpecialActionPerformed(AActor* TargetActor) {}

void USMCatchInteractionComponent_Character::OnSpecialActionEnded(AActor* TargetActor) {}

bool USMCatchInteractionComponent_Character::HandleCaughtReleased(AActor* TargetActor)
{
	// 이 함수는 만약 타겟이 null이어도 수행해야할 작업을 수행해줘야합니다.

	// 디태치합니다.
	SourceCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 자신을 잡고 있는 캐릭터를 제거하고 잡히기 상태 태그도 제거합니다.
	SetActorCatchingMe(nullptr);
	SourceASC->RemoveTag(SMTags::Character::State::Caught);

	// 캐릭터 상태 위젯을 다시 보이게합니다.
	SourceCharacter->SetCharacterStateVisibility(true);

	// 충돌판정, 움직임을 복구합니다.
	SourceCharacter->SetEnableCollision(true);
	SourceCharacter->SetEnableMovement(true);

	// 위치보정을 활성화해줍니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensureAlways(SourceMovement))
	{
		SourceMovement->bIgnoreClientMovementErrorChecksAndCorrection = false;
	}

	// 회전을 재지정합니다. 
	float NewYaw = 0.0f;
	if (TargetActor)
	{
		NewYaw = TargetActor->GetActorRotation().Yaw;
	}
	SourceCharacter->MulticastRPCSetYawRotation(NewYaw);

	SourceCharacter->ServerRPCPreventGroundEmbedding();

	// 카메라 뷰를 원래대로 복구합니다.
	APlayerController* SourcePlayerController = Cast<APlayerController>(SourceCharacter->Controller);
	if (ensureAlways(SourcePlayerController))
	{
		SourcePlayerController->SetViewTargetWithBlend(SourceCharacter, 1.0f, VTBlend_Cubic);
	}

	return true;
}
