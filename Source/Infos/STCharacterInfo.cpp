// Fill out your copyright notice in the Description page of Project Settings.


#include "Infos/STCharacterInfo.h"
#include "DataTable/STResourceTypes.h"

USTCharacterInfo::USTCharacterInfo()
{
	Restore(); 
}

USTCharacterInfo::USTCharacterInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Restore();
}

void USTCharacterInfo::Restore()
{
	Name = TEXT("");
	Level = 0;
	UID = -1;
	Hp = 0;
	MaxHp = 0;
	Sp = 0;
	MaxSp = 0;
	MoveSpeed = 0;
	Level = 0;
}
