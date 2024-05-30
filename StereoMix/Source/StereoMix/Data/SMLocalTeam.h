// Copyright Surround, Inc. All Rights Reserved.

#pragma once

/** 로컬팀은 로컬 플레이어 기준 같은 팀인지 다른 팀인지를 나타냅니다. */
UENUM(BlueprintType)
enum class ESMLocalTeam : uint8
{
	Equal,
	Different
};
