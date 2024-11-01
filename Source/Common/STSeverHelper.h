// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Character/STCharacter.h"
#include "../Infos/STCharacterInfo.h"

class ASTPlayerController;
class ASTCharacterAIController;


/**
 * @brief 서버에서 사용하는 함수들
 */

namespace STServerHelper
{
	// 캐릭터 생성 한다. 
	ASTCharacter* SpawnCharacter(UWorld* InWorld, USTCharacterInfo* InCharInfo, const FVector& InLocation, const FRotator& InRotation);
	// 
	void SetAdjustedLocationAndRotation(ASTCharacter* InActor, FVector InLocation, FRotator InRotation, ETeleportType InTeleportType = ETeleportType::TeleportPhysics);

	AActor* FindSpawnPoint(UWorld* world, int32 InIndex);

	bool IsTargetable(ASTCharacter* InChar, ASTCharacter* InTarget);
	ASTCharacterAIController* GetAiController(ASTCharacter* InChar);

		
}