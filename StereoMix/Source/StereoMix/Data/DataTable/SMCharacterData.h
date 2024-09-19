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
