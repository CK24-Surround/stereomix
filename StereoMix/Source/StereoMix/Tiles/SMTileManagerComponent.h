// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMTeam.h"
#include "SMTileManagerComponent.generated.h"


class ASMTile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTileManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTileManagerComponent();

	/**
	 * 타일을 점령합니다.
	 * @param StartTile 시작 타일입니다.
	 * @param InstigatorTeam 점령을 시도하는 오브젝트의 팀입니다.
	 * @param HalfHorizenSize 가로측 점령 사이즈입니다.
	 * @param HalfVerticalSize 세로측 점령 사이즈입니다.
	 * @return 점령에 성공한 타일의 개수를 반환합니다.
	 */
	int32 TileCapture(ASMTile* StartTile, ESMTeam InstigatorTeam, float HalfHorizenSize, float HalfVerticalSize, bool bShowDebug = false);
};
