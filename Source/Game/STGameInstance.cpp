// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STGameInstance.h"

#include "CommonSessionSubsystem.h"
#include "CommonUserSubsystem.h"
#include "Components/GameFrameworkComponentManager.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/PlayerState.h"
#include "Engine/World.h"

#include "../Infos/STGameplayTags.h"
#include "PlayerController/STPlayerController.h"
#include "PlayerController/STLocalPlayer.h"
#include "System/STSaveGame.h"
#include "GameMode/STGameMode.h"
#include "Character/STCharacter.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(STGameInstance)


USTGameInstance::USTGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void USTGameInstance::Init()
{
	Super::Init();

	// Register our custom init states  
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(STGameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(STGameplayTags::InitState_DataAvailable, false, STGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(STGameplayTags::InitState_DataInitialized, false, STGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(STGameplayTags::InitState_GameplayReady, false, STGameplayTags::InitState_DataInitialized);
	}
}

void USTGameInstance::Shutdown()
{
	if (UCommonSessionSubsystem* SessionSubsystem = GetSubsystem<UCommonSessionSubsystem>())
	{
		SessionSubsystem->OnPreClientTravelEvent.RemoveAll(this);
	}

	Super::Shutdown();
}


ASTPlayerController* USTGameInstance::GetPrimaryPlayerController() const
{
	return Cast<ASTPlayerController>(Super::GetPrimaryPlayerController(false));
}

int32 USTGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	int32 ReturnVal = UGameInstance::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
	
	}
	return ReturnVal;
}

bool USTGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	
	return UGameInstance::RemoveLocalPlayer(ExistingPlayer);
}

void USTGameInstance::LoadLevel(const FString& LevelName)
{
	//@remark : LevelName 는 fullpath 를 사용하는것이 빠르다.
	if (nullptr == GetWorld())
	{
		return;
	}

	if (auto instance = GetSaveGameInstance())
	{
		const auto levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		if (!levelName.Equals(LevelName))
		{
			UGameplayStatics::OpenLevel(MyCharacter, FName(*LevelName));
		}
	}
}

USTSaveGame* USTGameInstance::GetSaveGameInstance(const FString& SlotName)
{
	if (SaveGameInstance == nullptr)
	{
		//이미 세이브 파일이 있는 경우
		if (UGameplayStatics::DoesSaveGameExist(SlotName, SaveIndex))
		{
			SaveGameInstance = Cast<USTSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SaveIndex));
		}

		//세이브 파일이 없는 상태에서 새로 생성하는 경우
		if (SaveGameInstance == nullptr)
		{
			SaveGameInstance = Cast<USTSaveGame>(UGameplayStatics::CreateSaveGameObject(USTSaveGame::StaticClass()));
		}		
	}

	return SaveGameInstance;
}

void USTGameInstance::SaveGameInstanceToSlot(USTSaveGame* SaveInstance, const FString& SlotName)
{
	if (bBlockSave)
	{
		return;
	}
	
	if (SaveInstance)
	{
		bBlockSave = true;
		FAsyncSaveGameToSlotDelegate saveDelegate;
		saveDelegate.BindUObject(this, &USTGameInstance::OnSaved);
		UGameplayStatics::AsyncSaveGameToSlot(SaveInstance, SlotName, SaveIndex, saveDelegate);
	}
}

void USTGameInstance::OnSaved(const FString& SlotName, const int32 UserIndex, bool bSucc)
{
	if (bSucc)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("저장에 성공했습니다."));
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("저장에 실패했습니다."));
	}

	OnLoadFinish();
}

void USTGameInstance::OnLoadFinish()
{
	//UE_LOG(LogSave, Warning, "세이브 가능 상태로 전환합니다.");
	bBlockSave = false;
	OnFinishLoadGame.Broadcast();
}

void USTGameInstance::DeleteSaveFile(const FOnDeletedSaveFile& OnDeletedSaveFile, FString SlotName)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SaveIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SaveIndex);
		OnDeletedSaveFile.ExecuteIfBound();
		UE_LOG(LogTemp, Warning, TEXT("세이브 파일이 삭제되었습니다."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("해당 세이브 파일이 존재하지 않습니다."));
	}
}
