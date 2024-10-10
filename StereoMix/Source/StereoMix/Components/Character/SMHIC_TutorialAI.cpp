// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_TutorialAI.h"

#include "SMHIC_Character.h"
#include "Actors/Character/AI/SMAICharacterBase.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"


USMHIC_TutorialAI::USMHIC_TutorialAI()
{
}

bool USMHIC_TutorialAI::CanBeHeld(AActor* Instigator) const
{
	if (!SourceCharacter)
	{
		return false;
	}

	if (HeldMeActorsHistory.Contains(Instigator))
	{
		return false;
	}

	return true;
}

void USMHIC_TutorialAI::BeginPlay()
{
	Super::BeginPlay();

	SourceCharacter = GetOwner<ASMAICharacterBase>();
}

void USMHIC_TutorialAI::OnHeld(AActor* Instigator)
{
	if (!SourceCharacter)
	{
		return;
	}

	SourceCharacter->SetActorHiddenInGame(true);
	SourceCharacter->SetActorEnableCollision(false);

	SourceCharacter->AttachToActor(Instigator, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	HeldMeActorsHistory.Add(Instigator);
	SetActorHoldingMe(Instigator);
}

void USMHIC_TutorialAI::OnReleasedFromHold(AActor* Instigator)
{
	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(GetActorHoldingMe());
	USMHIC_Character* TargetHIC = Cast<USMHIC_Character>(USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetCharacter));

	// 잡기 상태에서 벗어납니다.
	ReleasedFromBeingHeld(TargetCharacter);

	if (TargetHIC)
	{
		TargetHIC->SetActorIAmHolding(nullptr);
	}
}

void USMHIC_TutorialAI::ReleasedFromBeingHeld(AActor* TargetActor, const TOptional<FVector>& TargetOptionalLocation)
{
	if (!SourceCharacter)
	{
		return;
	}

	// 타겟 캐릭터가 있으면 타겟의 회전값을 사용하고, 없으면 소스 캐릭터의 현재 회전값을 사용하여 소스 캐릭터의 최종 회전을 설정합니다.
	const float FinalSourceYaw = TargetActor ? TargetActor->GetActorRotation().Yaw : SourceCharacter->GetActorRotation().Yaw;
	SourceCharacter->SetActorRotation(FRotator(0.0, FinalSourceYaw, 0.0));

	// 타겟의 앞으로 위치를 조정합니다. 만약 옵션 위치가 제공되었다면 이를 사용합니다.
	const FVector TargetLocation = TargetActor ? TargetActor->GetActorLocation() : SourceCharacter->GetActorLocation();
	const FVector FinalSourceLocation = TargetOptionalLocation.Get(TargetLocation);
	SourceCharacter->SetActorLocation(FinalSourceLocation);

	SourceCharacter->SetActorHiddenInGame(false);
	SourceCharacter->SetActorEnableCollision(true);

	SetActorHoldingMe(nullptr);
}

void USMHIC_TutorialAI::ClearHeldMeActorsHistory()
{
	HeldMeActorsHistory.Empty();
}
