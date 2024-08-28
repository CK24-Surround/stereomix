// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SMSlashAnimNotify_Inputable.generated.h"

class ASMBassCharacter;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMSlashAnimNotify_Inputable : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	void InputableZoneEntry(ASMBassCharacter* SourceCharacter);

	void InputableZoneEscape(ASMBassCharacter* SourceCharacter);

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bIsStart:1 = true;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bIsLeftSlashNext:1 = true;
};
