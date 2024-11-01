// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CommonGameInstance.h"
#include "STGameInstance.generated.h"

class ASTPlayerController;
class ASTCharacter;
class UObject;
class USTSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishLoadGame);
DECLARE_DYNAMIC_DELEGATE(FOnDeletedSaveFile);

/**
 * 
 */
UCLASS(Config = Game)
class PROJECTR_API USTGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()
	
public:
	
	USTGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	ASTPlayerController* GetPrimaryPlayerController() const;

protected:

	virtual void Init() override;
	virtual void Shutdown() override;

	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;
	virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;

	void LoadLevel(const FString& LevelName);

	USTSaveGame* GetSaveGameInstance(const FString& SlotName = "SaveGameFile");
	void OnSaved(const FString& SlotName, const int32 UserIndex, bool bSucc);
	void OnLoadFinish();	
	void SaveGameInstanceToSlot(USTSaveGame* SaveInstance, const FString& SlotName = "SaveGameFile");
	void DeleteSaveFile(const FOnDeletedSaveFile& OnDeletedSaveFile, FString SlotName = "SaveGameFile");


public:

protected:
	UPROPERTY()
	TObjectPtr<ASTCharacter> MyCharacter;

	UPROPERTY()
	int32 SaveIndex = 0;

	UPROPERTY(Transient)
	bool bBlockSave = false;

	UPROPERTY()
	USTSaveGame* SaveGameInstance;

	UPROPERTY()
	FOnFinishLoadGame OnFinishLoadGame;

};
