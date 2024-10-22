// Copyright Studio Surround. All Rights Reserved.


#include "SMFaceControlComponent.h"

#include "Actors/Character/SMCharacterBase.h"
#include "Components/TimelineComponent.h"
#include "Utilities/SMLog.h"


USMFaceControlComponent::USMFaceControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	BlinkTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("BlinkTimelineComponent"));

	MouthTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("MouthTimelineComponent"));
}

void USMFaceControlComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USMFaceControlComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	InitMaterials();

	auto BlinkLoop = [BlinkTimelineComponentWeakPtr = MakeWeakObjectPtr(BlinkTimelineComponent), CopiedMinBlinkTime = MinBlinkTime, CopiedMaxBlinkTime = MaxBlinkTime] {
		if (BlinkTimelineComponentWeakPtr.IsValid())
		{
			if (const UWorld* World = BlinkTimelineComponentWeakPtr->GetWorld())
			{
				FTimerHandle TimerHandle;
				World->GetTimerManager().SetTimer(TimerHandle, [BlinkTimelineComponentWeakPtr] {
					if (BlinkTimelineComponentWeakPtr.IsValid())
					{
						BlinkTimelineComponentWeakPtr->PlayFromStart();
					}
				}, FMath::RandRange(CopiedMinBlinkTime, CopiedMaxBlinkTime), false);
			}
		}
	};

	FOnTimelineEventStatic OnTimelineEventStatic;
	OnTimelineEventStatic.BindLambda(BlinkLoop);
	BlinkTimelineComponent->SetTimelineFinishedFunc(OnTimelineEventStatic);

	const float BlinkPlayRate = 30.0f / BlinkFrameCount;
	BlinkTimelineComponent->SetPlayRate(BlinkPlayRate);

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [WeakBlinkTimelineComponent = MakeWeakObjectPtr(BlinkTimelineComponent)] {
			if (WeakBlinkTimelineComponent.IsValid())
			{
				WeakBlinkTimelineComponent->PlayFromStart();
			}
		}, FMath::RandRange(MinBlinkTime, MaxBlinkTime), false);
	}

	// MouthTimelineComponent->PlayFromStart();
}

void USMFaceControlComponent::Deactivate()
{
	BlinkTimelineComponent->Deactivate();
	MouthTimelineComponent->Deactivate();
	Super::Deactivate();
}

void USMFaceControlComponent::AddBlinkCurve(UCurveFloat* Curve)
{
	FOnTimelineFloatStatic OnTimelineFloatStatic;
	OnTimelineFloatStatic.BindLambda([ThisWeakPtr = MakeWeakObjectPtr(this)](float Value) {
		if (ThisWeakPtr.IsValid())
		{
			ThisWeakPtr->UpdateValue(ThisWeakPtr->InEyeMID, ThisWeakPtr->BlinkParameterName, Value);
			ThisWeakPtr->UpdateValue(ThisWeakPtr->OutEyeMID, ThisWeakPtr->BlinkParameterName, Value);
		}
	});

	BlinkTimelineComponent->AddInterpFloat(Curve, OnTimelineFloatStatic);
}

void USMFaceControlComponent::AddMouthCurve(UCurveFloat* Curve)
{
	FOnTimelineFloatStatic OnTimelineFloatStatic;
	OnTimelineFloatStatic.BindLambda([ThisWeakPtr = MakeWeakObjectPtr(this)](float Value) {
		if (ThisWeakPtr.IsValid())
		{
			// NET_LOG(ThisWeakPtr->GetOwner(), Warning, TEXT("MouthCurve: %f"), Value);
		}
	});

	MouthTimelineComponent->AddInterpFloat(Curve, OnTimelineFloatStatic);
}

void USMFaceControlComponent::InitMaterials()
{
	const ASMCharacterBase* Owner = GetOwner<ASMCharacterBase>();
	if (USkeletalMeshComponent* OwnerMesh = Owner ? Owner->GetMesh() : nullptr)
	{
		InEyeMID = OwnerMesh->CreateAndSetMaterialInstanceDynamic(InEyeMaterialIndex);
		OutEyeMID = OwnerMesh->CreateAndSetMaterialInstanceDynamic(OutEyeMaterialIndex);
	}
}

void USMFaceControlComponent::UpdateValue(UMaterialInstanceDynamic* Material, FName ParameterName, float ParameterValue)
{
	if (Material)
	{
		Material->SetScalarParameterValue(ParameterName, ParameterValue);
	}
}
