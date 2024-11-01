// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameMode.h"
#include "../DataTable/STDataTableManager.h"
#include "../GameState/STGameStateBase.h"
#include "Containers/Queue.h"
#include "STGameMode.generated.h"

class ASTCharacter;
class AActor;
class AController;
class AGameModeBase;
class APawn;
class APlayerController;
class UClass;
class USTExperienceDefinition;
class USTPawnData;
class UObject;
struct FFrame;
struct FPrimaryAssetId;
enum class ECommonSessionOnlineMode : uint8;
enum class ESTGameModeState : uint8;
class USTCharacterInfo;
struct FSTGameModeState;
class ASTGameStateBase;

USTRUCT()
struct PROJECTR_API FMonsterSpawnInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()		int32 SpawnID = 0;
	UPROPERTY() 	uint8 RoundID = 0;
	UPROPERTY() 	uint8 WaveID = 0;
	UPROPERTY() 	float SpawnDelayTime = 0.f;
	UPROPERTY()		USTCharacterInfo* CharacterInfo = nullptr;
	UPROPERTY() 	ASTCharacter* SpawnCharacter = nullptr;

	FMonsterSpawnInfo() : SpawnID(0) , RoundID(0), WaveID(0), SpawnDelayTime(0), CharacterInfo(nullptr), SpawnCharacter(nullptr){	}
};

/**
 * 플레이어나 봇이 게임에 참가할 때뿐 아니라 원활하고 원활하지 않은 이동 이후에 트리거되는 로그인 후 이벤트입니다. 
 * 이 이벤트는 플레이어가 초기화를 마친 후에 호출됩니다. *
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSTGameModePlayerInitialized, AGameModeBase* /*GameMode*/, AController* /*NewPlayer*/);

/**
* @class : ASTGameMode
* @brief : 인게임에서 사용하는 모드
*/

UCLASS(Config = Game , Meta = (ShortTooltip = "The base game mode class used by STGame project."))
class ASTGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:
	ASTGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//AGameModeBase interface
	virtual void StartPlay() override;
	virtual void PreInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;		

	//~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;	  
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;	
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual void InitGameState() override;
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;	
	//~End of AGameModeBase interface
	// 	
	// 지정된 플레이어 또는 봇을 다음 프레임에 다시 시작한다
	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

	// 봇과 플레이어 모두 사용할 수 있는 PlayerCanRestart의 중립적 버전.
	virtual bool ControllerCanRestart(AController* Controller);

	// 위에서 설명한 플레이어 초기화에 대한 delegate 호출
	FOnSTGameModePlayerInitialized OnGameModePlayerInitialized;

	UFUNCTION(BlueprintCallable, Category = "STGame|Pawn")
	const USTPawnData* GetPawnDataForController(const AController* InController) const;

protected:
	virtual void GamePlayInitialize();
	virtual void GamePlayMainTimer();	
	virtual void PreCachedNPCs();
	virtual void SpawnPlayer();
	virtual ASTCharacter* SpawnCharacter(int32 InSpawnIndex, USTCharacterInfo* InCharInfo);	

	void OnExperienceLoaded(const USTExperienceDefinition* CurrentExperience);
	bool IsExperienceLoaded() const;

	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);

	void HandleMatchAssignmentIfNotExpectingOne();

	bool TryDedicatedServerLogin();
	void HostDedicatedServerMatch(ECommonSessionOnlineMode OnlineMode);

	UFUNCTION()
	void OnUserInitializedForDedicatedServer(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext);

	FSTDataTableManager::FOnAsyncLoadFinish OnAsyncLoadFinishDelegate;
	void OnAsyncLoadFinished() {} ;
	
	void ChangeGameModeState(const ESTGameModeState& InState, const int32& InArg = 0) ;
	void UpdateGameModeState();
	void CheckGameModeEnd();

	void OnGameStateInit();
	void OnGameStateWait();
	void OnGameStateReady();	
	void OnGameStateCountDown();
	void OnGameStateStart();
	void OnGameStateFinish();
	void OnGameStateResult();

	void OnCountDownTimer();
	void SetPlayerInfos();	
	void SetPlayerInfosByEquipment();
	void SetNPCInfos();

private:

protected:	

	TQueue<FSTGameModeState> GameModeStateQueue;	

	//게임모드 상태 타이머
	UPROPERTY(BlueprintReadOnly, Category = "STGame")
	FTimerHandle GameModeStartTimerHandle;

	//시간 경과에따른 타이머 
	UPROPERTY(BlueprintReadOnly, Category = "STGame")
	FTimerHandle	MainTimerHandle;

	//게임 종료 타이머
	UPROPERTY(BlueprintReadOnly, Category = "STGame")
	FTimerHandle	EndTimerHandle;

	//게임 종료 타이머
	UPROPERTY(BlueprintReadOnly, Category = "STGame")
	FTimerHandle	CountDownTimerHandle;

	//맵에 스폰 시킬 몬스터 리스트 저장
	UPROPERTY()
	TMap<int32, FMonsterSpawnInfo> MonsterList;

	// 스폰된 몬스터 저장
	TArray<FMonsterSpawnInfo*> SpawnedMonsters;
	
};



