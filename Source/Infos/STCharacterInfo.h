// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../DataTable/STDataTableManager.h"
#include "STCharacterInfo.generated.h"

/**
 * @class : USTCharacterInfo
 * @brief : 캐릭터 정보를 담는다.
 */
UCLASS()
class PROJECTR_API USTCharacterInfo : public UObject
{
	GENERATED_BODY()

public:
	USTCharacterInfo();
	USTCharacterInfo(const FObjectInitializer& ObjectInitializer);

	void Restore();

	void SetLevel(int32 InLevel) { Level = InLevel; }
	void SetUID(int32 InID)		 { UID = InID; }
	void SetName(FString InName) { Name = InName; }
	void SetHp(int32 InHp) { Hp = InHp; }
	void SetMaxHp(int32 InMaxHp) { MaxHp = InMaxHp; }
	void SetSp(int32 InSp) { Sp = InSp; }
	void SetMaxSp(int32 InMaxSp) { MaxSp = InMaxSp; }
	void SetMoveSpeed(int32 InMoveSpeed) { MoveSpeed = InMoveSpeed; }


	FORCEINLINE const FResCharacter* GetResCharacter() { return FSTDataTableManager::GetResCharacter(UID); }
	FORCEINLINE const int32 GetLevel() { return Level; }
	FORCEINLINE const int32 GetUID() { return UID; }
	FORCEINLINE const FString GetName() { return Name; }
	FORCEINLINE const int32 GetHp() { return Hp; }
	FORCEINLINE const int32 GetMaxHp() { return MaxHp; }
	FORCEINLINE const int32 GetSp() { return Sp; }
	FORCEINLINE const int32 GetMaxSp() { return MaxSp; }
	FORCEINLINE const int32 GetMoveSpeed() { return MoveSpeed; }

private:
	FString Name = TEXT("");
	int32 Level = 0;
	int32 UID	= -1;
	int32 Hp = 0;
	int32 MaxHp = 0;	
	int32 Sp = 0;
	int32 MaxSp = 0;
	int32 MoveSpeed = 0;
};
