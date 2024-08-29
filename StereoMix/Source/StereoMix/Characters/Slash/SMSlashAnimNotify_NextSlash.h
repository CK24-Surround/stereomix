// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SMSlashAnimNotify_NextSlash.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMSlashAnimNotify_NextSlash : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bIsLeftSlashNext:1 = true;
};
