// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"

#include "STEnums.generated.h"

/**
 * @brief : Enum 
 */

 //�ɸ��� ���� 
UENUM(BlueprintType, meta = (DisplayName = "Character Job"))
enum class ESTCharacterType : uint8
{
	None = 0,
	Warrior= 1,
	Wizard=2,
	Max,
};

//�ɸ��Ͱ� ����� ����� ��з�  
UENUM(BlueprintType, meta = (DisplayName = "Character Weapon"))
enum class ESTCharacterWeaponType : uint8
{
	None = 0,
	Axe = 1,
	Sword= 2,	
	Bow=3,
	
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ESTCharacterWeaponType, ESTCharacterWeaponType::Count);


UENUM(BlueprintType)
enum class ECharType : uint8
{
	None = 0,
	Player = 1,
	Normal_Monster = 2,
	Elite_Monster = 3,
};

UENUM()
enum class ECharMask : uint32
{
	None = 0x00000000,
	Player = 0x00000001,
	Normal_Monster = 0x00000002,
	Elite_Monster = 0x00000004,
};

UENUM(BlueprintType)
enum class ECharGrade : uint8
{
	None = 0,
	Normal = 1,
	Premium = 2,
};

// ĳ���� Ŭ���� 
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	None = 0,		//����
	Gladiator = 1,	//������
	Slayer = 2,		//����
	Lancer = 3,		//â���
};


UENUM(BlueprintType)
enum class ESTUIPathCategory : uint8
{
	UI = 0,
	WIDGET,
	WIDGETCOMPONENT,
	SOUND,
	SPRITE,
	TEXTURE,
	MATERIAL,
	Count UMETA(Hidden),
};
ENUM_RANGE_BY_COUNT(ESTUIPathCategory, ESTUIPathCategory::Count);


UENUM()
enum class EWidgetZOrder : int32
{
	// UI ������ 
	UILayer		 = 1000,			
	// �˾�  
	PopupLayer	 = 2000,
	// �ý���
	SystemLayer  = 3000,
	// �ε�
	Loading		 = 20000,		
	
	ErrorPopup   = 80000,	
	Max = 90000
};

UENUM(BlueprintType)
enum class ESTCountryCode : uint8
{
	None = 0,
	KOR,
	ENG,
	JPN,
	CHN,
	Count UMETA(Hidden),
};
ENUM_RANGE_BY_COUNT(ESTCountryCode, ESTCountryCode::Count);

