// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Common/STEnums.h"
#include "STGameType.generated.h"

/**
 * @brief : 캐릭터 상태 정책 나열한다.
 */

UENUM()
enum class EPolicyType : uint8
{
	PCT_Idle,
	PCT_Wait,
	PCT_Spawn,
	PCT_Dead,
	PCT_Disabled,
	PCT_InActive,
	PCT_Active,
	PCT_DeActive,

	PCT_WakeUp,
	PCT_MoveTo,
	PCT_Roaming,
};



