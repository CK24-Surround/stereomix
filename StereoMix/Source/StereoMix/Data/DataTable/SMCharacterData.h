#pragma once

#include "SMCharacterData.generated.h"

USTRUCT(BlueprintType)
struct FSMCharacterStatsData : public FTableRowBase
{
	GENERATED_BODY()

	FSMCharacterStatsData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float HP = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MoveSpeed = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Stamina = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float SkillGauge = -1.0f;
};

USTRUCT(BlueprintType)
struct FSMCharacterAttackData : public FTableRowBase
{
	GENERATED_BODY()

	FSMCharacterAttackData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Damage = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float DistanceByTile = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float ProjectileSpeed = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float AttackPerSecond = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float SpreadAngle = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|ElectricGuitar")
	int32 AccuracyAttackRate = -1;
};

USTRUCT(BlueprintType)
struct FSMCharacterSkillData : public FTableRowBase
{
	GENERATED_BODY()

	FSMCharacterSkillData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Damage = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float DistanceByTile = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float ProjectileSpeed = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float StartUpTime = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Range = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Magnitude = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Duration = -1.0f;
};

USTRUCT(BlueprintType)
struct FSMCharacterNoiseBreakData : public FTableRowBase
{
	GENERATED_BODY()

	FSMCharacterNoiseBreakData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Damage = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float DistanceByTile = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 CaptureSize = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float GravityScale = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float ApexHeight = -1.0f;
};
