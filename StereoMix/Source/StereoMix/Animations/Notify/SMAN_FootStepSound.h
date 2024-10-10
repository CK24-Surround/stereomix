// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SMAN_FootStepSound.generated.h"

class UFMODEvent;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAN_FootStepSound : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UFMODEvent> Event;
};
