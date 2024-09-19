#pragma once

#include "SMCharacterData.generated.h"

USTRUCT(BlueprintType)
struct FSMCharacterStatsData : public FTableRowBase
{
	GENERATED_BODY()

	FSMCharacterStatsData()
		: HP(-1.0f), MoveSpeed(-1.0f), Stamina(-1.0f), SkillGauge(-1.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float SkillGauge;
};

USTRUCT(BlueprintType)
struct FSMCharacterSkillData : public FTableRowBase
{
	GENERATED_BODY()

	FSMCharacterSkillData()
		: Damage(-1.0f), DistanceByTile(-1.0f), Speed(-1.0f), Size(-1.0f), Magnitude(-1.0f), Duration(-1.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float DistanceByTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Magnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Duration;
};

USTRUCT(BlueprintType)
struct FSMCharacterNoiseBreakData : public FTableRowBase
{
	GENERATED_BODY()

	FSMCharacterNoiseBreakData()
		: Damage(-1.0f), CaptureSize(-1.0f), DistanceByTile(-1.0f), ApexHeigth(-1.0f), GravityScale(-1.0f), TotalTriggerTime(-1.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float CaptureSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float DistanceByTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float ApexHeigth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float GravityScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float TotalTriggerTime;
};
