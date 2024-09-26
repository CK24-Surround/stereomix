// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Utilities/SMLog.h"


USMCharacterMovementComponent::USMCharacterMovementComponent() {}

float USMCharacterMovementComponent::GetMaxSpeed() const
{
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!SourceASC)
	{
		return Super::GetMaxSpeed();
	}

	const USMCharacterAttributeSet* SourceAttributeSet = SourceASC->GetSet<USMCharacterAttributeSet>();
	if (!SourceAttributeSet)
	{
		return 0.0f;
	}

	return GetBaseSpeed() + ModifierMoveSpeed;
}

float USMCharacterMovementComponent::GetBaseSpeed() const
{
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!SourceASC)
	{
		return Super::GetMaxSpeed();
	}

	const USMCharacterAttributeSet* SourceAttributeSet = SourceASC->GetSet<USMCharacterAttributeSet>();
	if (!SourceAttributeSet)
	{
		return 0.0f;
	}

	return SourceAttributeSet->GetMoveSpeed();
}

FSMMoveSpeedModifierHandle USMCharacterMovementComponent::AddMoveSpeedBuff(float SpeedMultiplier, float Duration)
{
	return AddMoveSpeedModifier(SpeedMultiplier, Duration);
}

void USMCharacterMovementComponent::RemoveMoveSpeedBuff(const FSMMoveSpeedModifierHandle& Handle)
{
	RemoveMoveSpeedModifier(Handle);
}

void USMCharacterMovementComponent::ClientAddMoveSpeedBuff_Implementation(float SpeedMultiplier, float Duration)
{
	AddMoveSpeedBuff(SpeedMultiplier, Duration);
	ServerAddMoveSpeedBuff(SpeedMultiplier, Duration);
}

FSMMoveSpeedModifierHandle USMCharacterMovementComponent::AddMoveSpeedModifier(float SpeedMultiplier, float Duration)
{
	int32 NewHandleID;
	if (AvailableIDs.IsEmpty())
	{
		NewHandleID = NextModifierID++;
	}
	else
	{
		NewHandleID = AvailableIDs.Pop();
	}

	FSMMoveSpeedModifierHandle NewHandle(NewHandleID);

	FMoveSpeedModifierData NewModifierData;
	NewModifierData.Handle = NewHandle;

	const float BaseSpeed = GetBaseSpeed();
	NewModifierData.ModifiedMoveSpeed = (BaseSpeed * SpeedMultiplier) - BaseSpeed;

	TWeakObjectPtr<USMCharacterMovementComponent> ThisWeakPtr(this);
	auto Lambda = [ThisWeakPtr, NewHandle]() {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->RemoveMoveSpeedModifier(NewHandle);
		}
	};
	GetWorld()->GetTimerManager().SetTimer(NewModifierData.TimerHandle, Lambda, 1.0f, false, Duration);

	ModifierMoveSpeed += NewModifierData.ModifiedMoveSpeed;
	ActiveModifiers.Add(NewModifierData);

	return NewHandle;
}

void USMCharacterMovementComponent::RemoveMoveSpeedModifier(const FSMMoveSpeedModifierHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}

	FMoveSpeedModifierData* RemoveModifier = ActiveModifiers.FindByPredicate([&Handle](const FMoveSpeedModifierData& SpeedModifierData) { return Handle.ID == SpeedModifierData.Handle.ID; });
	if (RemoveModifier)
	{
		ModifierMoveSpeed -= RemoveModifier->ModifiedMoveSpeed;
		if (RemoveModifier->TimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(RemoveModifier->TimerHandle);
		}

		AvailableIDs.Push(RemoveModifier->Handle.ID);
		ActiveModifiers.RemoveAll([&Handle](const FMoveSpeedModifierData& SpeedModifierData) { return Handle.ID == SpeedModifierData.Handle.ID; });
	}
}

void USMCharacterMovementComponent::ServerAddMoveSpeedBuff_Implementation(float SpeedMultiplier, float Duration)
{
	AddMoveSpeedBuff(SpeedMultiplier, Duration);
}
