﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Object.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "STEnums.h"
#include "../Infos/STLogChannels.h"

class USTGameInstance;
class USTLocalPlayer;
class USTUISystem;
class USTUIContents;
class USTUIContentsLobby;
class USTUIContentsInGame;
class USTUIManagerSubsystem;
class ASTCharacter;

/**
 * 
 */

// 공용 상수 정의
namespace Const
{
	const uint32 InvalidTID = 0;		// 유효하지 않은 테이블 일때

	const FName NAME_TitleMap = FName(TEXT("/Game/Environment/Title"));
	const FName NAME_LobbyMap = FName(TEXT("/Game/Environment/Lobby"));
	const FName NAME_InGameMap = FName(TEXT("/Game/Environment/TestMap01"));

};

namespace LocalizationContext
{
	const FString DefaultLanguage(TEXT("en"));
	const FString KeyLanguage(TEXT("ko"));
	const FString AutoGeneratedLocalizedStringDirectoryName = TEXT("AutoGenerated/");
	const FString LocalizedStringFileSuffix = TEXT("LocalizedString");
};

namespace DataTableContext
{
	const FString GameTablePath = TEXT("/Game/Resources/Table/");
	const FString CertTablePath = TEXT("/Game/Common/Table/");
};

namespace STHelper
{
	//접근을 쉽게 사용하기 위한 함수 및 변수를 정의한다. 	

	PROJECTR_API USTGameInstance* GetGameInstance(UWorld* InWorld);
	
	PROJECTR_API USTLocalPlayer* GetLocalPlayer(UWorld* InWorld);
		
	PROJECTR_API USTUIManagerSubsystem* GetUIManagerSubSystem(UWorld* InWorld);

	PROJECTR_API USTUISystem* GetUISystem(UWorld* InWorld);
	
	PROJECTR_API USTUIContentsLobby* GetUIContentsLobby(UWorld* InWorld);

	PROJECTR_API USTUIContentsInGame* GetUIContentsInGame(UWorld* InWorld);		

	/// <summary>
	/// 월드 내의 액터를 찾는다. 액터를 검색하므로 퍼포먼스 민감한 부분은 자제한다.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="Functor"></typeparam>
	/// <param name="InWorld"></param>
	/// <param name="InFunctor"></param>
	/// <returns></returns>
	template <typename T, typename Functor>
	FORCEINLINE_DEBUGGABLE T* FindActor(UWorld* InWorld, Functor InFunctor)
	{
		ensure(InWorld);
		for (TActorIterator<T> it(InWorld); it; ++it)
		{
			if (InFunctor(*it))
			{
				return *it;
			}
		}
		return nullptr;
	}

	PROJECTR_API bool IsHostile(const ASTCharacter* InCharA, const ASTCharacter* InCharB);

	void SetActorTickActivate(AActor* InActor, bool bEnabled);
	

};

// 파라곤 예제샘플 
namespace ParagonAssets
{
	const FString GreyStone_BP_Path = TEXT("Blueprint'/Game/Paragon/ParagonGreystone/Characters/Heroes/Greystone/GreystonePlayerCharacter.GreystonePlayerCharacter'");

};

/// <summary>
/// UI 관련 정의
/// </summary>

namespace CommonUIPath
{
	static constexpr auto UI_PATH		= TEXT("/Game/");
	static constexpr auto WIDGET_PATH	= TEXT("/Game/");
	static constexpr auto WIDGET_COMPONENT_PATH = TEXT("/Game/");
	

	FString GetFullPath(ESTUIPathCategory category, const FString& name);
};

namespace CommonUIName
{
	const FString BACKGROUND_IMAGE_NAME = TEXT("BackgroundImage");
};

namespace DifficultyColor
{
	// 난이도 컬러값 정의
	const FLinearColor Locked = FLinearColor::White;
	const FLinearColor Normal = FLinearColor(FColor::FromHex("#9dff55"));
	const FLinearColor Hard = FLinearColor(FColor::FromHex("#fffa71"));
	const FLinearColor Array[] = { Locked, Normal, Hard};
}

namespace GradeColor
{
	// 등급 컬러값 정의
	const FLinearColor None = FLinearColor::White;
	const FLinearColor Common = FLinearColor(FColor::FromHex("#898989ff"));
	const FLinearColor Rare = FLinearColor(FColor::FromHex("#448cffff"));
	const FLinearColor Epic = FLinearColor(FColor::FromHex("#b362ffff"));	
	const FLinearColor Array[] = { None, Common, Rare, Epic};
}

