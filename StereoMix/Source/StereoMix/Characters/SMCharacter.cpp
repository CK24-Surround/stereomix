// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacter.h"

#include "Data/SMCharacterAssetData.h"
#include "Utilities/SMAssetPath.h"


ASMCharacter::ASMCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USMCharacterAssetData> SMCharacterAssetDataRef(SMAssetPath::CharacterAssetData);
	if (SMCharacterAssetDataRef.Object)
	{
		AssetData = SMCharacterAssetDataRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SMCharacterAssetData 로드에 실패했습니다."));
	}
}

void ASMCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
