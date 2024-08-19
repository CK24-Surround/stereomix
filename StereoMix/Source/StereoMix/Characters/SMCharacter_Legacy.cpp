// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacter_Legacy.h"

#include "Data/SMCharacterAssetData.h"
#include "Utilities/SMAssetPath.h"


ASMCharacter_Legacy::ASMCharacter_Legacy()
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

void ASMCharacter_Legacy::BeginPlay()
{
	Super::BeginPlay();
}

void ASMCharacter_Legacy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASMCharacter_Legacy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
