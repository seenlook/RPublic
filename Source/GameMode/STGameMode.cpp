// Copyright Epic Games, Inc. All Rights Reserved.

#include "STGameMode.h"
#include "PlayerController/STPlayerController.h"
#include "UObject/ConstructorHelpers.h"

#include "AssetRegistry/AssetData.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Misc/MessageDialog.h"

#include "Infos/STLogChannels.h"
#include "Misc/CommandLine.h"
#include "System/STAssetManager.h"
#include "GameState/STGameStateBase.h"

#include "PlayerController/STPlayerState.h"
#include "Character/STCharacter.h"
#include "Infos/STCharacterInfo.h"
#include "Character/STPawnExtensionComponent.h"
#include "Character/STPawnData.h"
#include "GameMode/STWorldSettings.h"
#include "GameExperience/STExperienceDefinition.h"
#include "GameExperience/STExperienceManagerComponent.h"
#include "Player/STPlayerSpawningManagerComponent.h"
#include "AI/STCharacterAIController.h"

#include "CommonUserSubsystem.h"
#include "CommonSessionSubsystem.h"
#include "TimerManager.h"
#include "GameMapsSettings.h"
#include "ProjectRPlayerController.h"

#include "Common/STHelper.h"
#include "Common/STSeverHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STGameMode)

/*@brief : 게임모드는 게임의 규칙을 정의하고 게임의 진행을 제어하는 클래스입니다. 
* Client Connection  Flow 
* AGameModeBase::Login(...) -> SpawnPlayerControllerCommon() -> AGameModeBase::InitNewPlayer(...)
* AGameModeBase::PostLogin(...) -> AGameModeBase::HandleStartingNewPlayer(...) -> AGameModeBase::RestartPlayer(...) -> a Pawn for the PC to possess
*/

ASTGameMode::ASTGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// use our custom PlayerController class
	PlayerControllerClass = ASTPlayerController::StaticClass();

	GameStateClass = ASTGameStateBase::StaticClass();
	
	PlayerStateClass = ASTPlayerState::StaticClass();
	DefaultPawnClass = ASTCharacter::StaticClass();
	//HUDClass = ASTHUD::StaticClass();
		
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/Heroes/BP_STCharacterDefault"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}	
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		//PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}

/// <summary>
/// 레벨이 로드되고 데디서버가 준비가 되서 대기상태
/// </summary>
void ASTGameMode::StartPlay()
{
	Super::StartPlay();	

	GamePlayInitialize();

	ChangeGameModeState(ESTGameModeState::GState_Mode_Init);
}

void ASTGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(MainTimerHandle, this, &ASTGameMode::GamePlayMainTimer, 1.0f, true);
}

const USTPawnData* ASTGameMode::GetPawnDataForController(const AController* InController) const
{
	// 플레이어 상태에 폰 데이터가 이미 설정되어 있는지 확인합니다.
	if (InController != nullptr)
	{
		if (const ASTPlayerState* MyPS = InController->GetPlayerState<ASTPlayerState>())
		{
			if (const USTPawnData* PawnData = MyPS->GetPawnData<USTPawnData>())
			{
				return PawnData;
			}
		}
	}

	// 그렇지 않으면 현재 experience 대한 기본값으로 전달.
	check(GameState);
	USTExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USTExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const USTExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		// Experience 로드되었고 폰 데이터가 아직 없는 경우 일단 기본값으로 전달.
		return USTAssetManager::Get().GetDefaultPawnData();
	}

	// Experience 가 로드되지 않았으므로 가져올 폰 데이터가 없다.
	return nullptr;
}

void ASTGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 데이타 테이블 로드
	OnAsyncLoadFinishDelegate.Unbind();
	OnAsyncLoadFinishDelegate.BindUObject(this, &ASTGameMode::OnAsyncLoadFinished);
	FSTDataTableManager::LoadDataTablesAsync(&OnAsyncLoadFinishDelegate);
		
	// 시작 설정을 초기화하는 데 시간을 주기 위해 다음 틱을 기다린다.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ASTGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASTGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	// 게임 시작 전에 경험을 할당하려고 시도한다.
	// 경험을 할당하지 못하면 게임을 시작할 수 없다.
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;	

	UWorld* World = GetWorld();

	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(USTExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	// 명령줄이 experience 설정하려는지 확인
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(FPrimaryAssetType(USTExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromCommandLine));
			}
			ExperienceIdSource = TEXT("CommandLine");
		}
	}

	// 월드 설정에 기본 experience 있는지 확인
	if (!ExperienceId.IsValid())
	{
		if (ASTWorldSettings* TypedWorldSettings = Cast<ASTWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	USTAssetManager& AssetManager = USTAssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogSTExperience, Error, TEXT("EXPERIENCE: not find"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}
	
	if (!ExperienceId.IsValid())
	{
		// 기본 experience를 사용하도록 설정
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("STExperienceDefinition"), FName("BP_STExperienceDefinition"));
		ExperienceIdSource = TEXT("Default");
	}

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

bool ASTGameMode::TryDedicatedServerLogin()
{
	// 활성 데디케이티드 서버로 등록하는 기본 코드입니다. 이것은 게임에 의해 수정 되어야 합니다.
	FString DefaultMap = UGameMapsSettings::GetGameDefaultMap();
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance && World && World->GetNetMode() == NM_DedicatedServer && World->URL.Map == DefaultMap)
	{		
		// 데디케이티드 서버의 기본 맵인 경우에만 등록합니다.
		UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
		
		// 데디케이티드 서버는 온라인 로그인을 수행해야 할 수도 있습니다.
		UserSubsystem->OnUserInitializeComplete.AddDynamic(this, &ASTGameMode::OnUserInitializedForDedicatedServer);

	
		// 데디케이티드 서버에 로컬 사용자가 없지만 인덱스 0은 온라인 로그인 코드에서 처리하는 기본 플랫폼 사용자를 의미합니다.
		if (!UserSubsystem->TryToLoginForOnlinePlay(0))
		{
			OnUserInitializedForDedicatedServer(nullptr, false, FText(), ECommonUserPrivilege::CanPlayOnline, ECommonUserOnlineContext::Default);
		}

		return true;
	}

	return false;
}

void ASTGameMode::HostDedicatedServerMatch(ECommonSessionOnlineMode OnlineMode)
{
	// 데디 서버에서 매치를 시작하는 곳이며, 게임에 따라 변경된다.
}

void ASTGameMode::OnUserInitializedForDedicatedServer(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		// Unbind
		UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
		UserSubsystem->OnUserInitializeComplete.RemoveDynamic(this, &ASTGameMode::OnUserInitializedForDedicatedServer);

		if (bSuccess)
		{		
			// 온라인 로그인이 성공했습니다. 전체 온라인 게임을 시작합니다.
			UE_LOG(LogSTGame, Log, TEXT("데디케이티드 서버에 접속 성공 , 시작"));			
			HostDedicatedServerMatch(ECommonSessionOnlineMode::Online);
		}
		else
		{				
			UE_LOG(LogSTGame, Log, TEXT("온라인 로그인이 실패했습니다. LAN 전용 서버를 시작합니다.	"));
			HostDedicatedServerMatch(ECommonSessionOnlineMode::LAN);
		}
	}
}

void ASTGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogSTGame, Log, TEXT("Identified experience 설정하고 게임을 시작한다.  %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource); 

		if (USTExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USTExperienceManagerComponent>())
		{
			ExperienceComponent->SetCurrentExperience(ExperienceId);
		}
	}
	else
	{
		UE_LOG(LogSTGame, Error, TEXT(" experience 설정하지 못했으므로 게임을 시작할 수 없다. loading screen will stay up forever")); 
	}
}

void ASTGameMode::OnExperienceLoaded(const USTExperienceDefinition* CurrentExperience)
{	
	//이미 어태치된 플레이어를 생성합니다.
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool ASTGameMode::IsExperienceLoaded() const
{
	check(GameState);
	USTExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USTExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

UClass* ASTGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const USTPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ASTGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	return nullptr;
}

bool ASTGameMode::ShouldSpawnAtStartSpot(AController* Player)
{	
	return false;
}

void ASTGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Experience 로드될 때까지 새 플레이어를 시작하는 것을 지연합니다.
	// 그 전에 로그인한 플레이어는 OnExperienceLoaded에 의해 시작됩니다.
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* ASTGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (USTPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<USTPlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ChoosePlayerStart(Player);
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void ASTGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	if (USTPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<USTPlayerSpawningManagerComponent>())
	{
		PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	}

	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

bool ASTGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

bool ASTGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{		
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	if (USTPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<USTPlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ControllerCanRestart(Controller);
	}

	return true;
}

void ASTGameMode::InitGameState()
{
	Super::InitGameState();
	
	USTExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USTExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnSTExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	
}

void ASTGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);
}

void ASTGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
}

bool ASTGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	//todo - implement this
	return true;
}

void ASTGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);
	
	// 폰을 생성 다시 시도한다. pawn class 를 꼭 확인해라.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{		
			if (PlayerCanRestart(NewPC))
			{
				//RequestPlayerRestartNextFrame(NewPlayer, false);
			}
			else
			{
				UE_LOG(LogSTGame, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			//RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(LogSTGame, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}

void ASTGameMode::GamePlayInitialize()
{
	// 게임모드에서 게임 시작에 필요한 초기화를 진행한다. 
	// npc 를 미리 로드하여 가지고 있는다. 
	PreCachedNPCs();
}

void ASTGameMode::PreCachedNPCs()
{
	TArray<const FResSpawnIndex*> outSpawns;

	int32 spawnGroupID = 1;

	// 스폰 정보 설정
	FSTDataTableManager::GetSpawnTableRows(outSpawns, spawnGroupID);
	for (auto iter : outSpawns)
	{
		USTCharacterInfo* charInfo = NewObject<USTCharacterInfo>(this, USTCharacterInfo::StaticClass());
		charInfo->SetLevel(iter->UnitLevel);
		charInfo->SetUID(iter->UnitID);

		if (ASTCharacter* newChar = SpawnCharacter(iter->SpawnPoint, charInfo))
		{
			FMonsterSpawnInfo spawnInfo;
			spawnInfo.CharacterInfo = charInfo;
			spawnInfo.SpawnCharacter = newChar;
			spawnInfo.SpawnID = iter->SpawnPoint;
			spawnInfo.RoundID = iter->RoundID;
			spawnInfo.WaveID = iter->WaveID;
			
			// 미리 로드 시켜놓기 위함으로 캐릭터는 비활성화 상태로 설정한다. 
			STServerHelper::GetAiController(newChar)->StartDeActive();
			MonsterList.Emplace(iter->SpawnID, spawnInfo);
		}
	}	
}

#pragma region MainGamePlayTimer 

void ASTGameMode::GamePlayMainTimer()
{
	if (GameModeStateQueue.IsEmpty())
	{
		return;
	}
	
	if (ASTGameStateBase* myGameState = GetGameState<ASTGameStateBase>())
	{	
		if (myGameState->GetCurGameModeState() == ESTGameModeState::GState_Game_Result)
		{
			return;
		}
		myGameState->TotalPlayTime++;
	}
	
	CheckGameModeEnd();
	
	UpdateGameModeState();	
}

void ASTGameMode::CheckGameModeEnd()
{
	if (ASTGameStateBase* stGameState = GetGameState<ASTGameStateBase>())
	{
		if (stGameState->RemainingTime <= 0)
		{
			ChangeGameModeState(ESTGameModeState::GState_Game_Finish);
		}
	}	
}

void ASTGameMode::ChangeGameModeState(const ESTGameModeState& InState, const int32& InArg)
{
	ASTGameStateBase* myGameState = GetGameState<ASTGameStateBase>();
	ensure(myGameState);
	if (myGameState->GetCurGameModeState() == InState)
	{
		return;
	}
	
	FSTGameModeState modeState;
	modeState.State = InState;
	modeState.IntArg = InArg;
	GameModeStateQueue.Enqueue(modeState);

	return;
}

void ASTGameMode::UpdateGameModeState()
{
	if (!GameModeStateQueue.IsEmpty())
	{
		FSTGameModeState modeState;
		GameModeStateQueue.Dequeue(modeState);

		ASTGameStateBase* myGameState = GetGameState<ASTGameStateBase>();
		ensure(myGameState);
		myGameState->SetGameModeState(modeState);

		switch (modeState.State)
		{
		case ESTGameModeState::GState_Mode_Init:
		{
			OnGameStateInit();
			break;
		};	
		case ESTGameModeState::GState_Mode_Wait:
		{
			OnGameStateWait();
			break;
		}	
		case ESTGameModeState::GState_Mode_Ready:
		{
			OnGameStateReady();
			break;
		}
		case ESTGameModeState::GState_Game_Countdown:
		{
			OnGameStateCountDown();
			break;
		};
		case ESTGameModeState::GState_Game_Start:
		{
			OnGameStateStart();
			break;
		};		
		case ESTGameModeState::GState_Game_Finish:
		{
			OnGameStateFinish();
			break;
		};
		case ESTGameModeState::GState_Game_Result:
		{
			OnGameStateResult();
			break;
		};	
		default: break;
		}	
	}
}

void ASTGameMode::OnGameStateInit()
{
	if (IsNetMode(NM_DedicatedServer))
	{
		//데디서버일경우 처리 , 웹서버를 사용할 경우 웹서버에 로그인하고 접속한다.- Request Web Login()
		ChangeGameModeState(ESTGameModeState::GState_Mode_Wait);
	}	
	else
	{
		//로컬게임 처리 -> 대기상태 바로 진입
		ChangeGameModeState(ESTGameModeState::GState_Mode_Wait);
	}	
}

void ASTGameMode::OnGameStateWait()
{
	//플레이어 정보를 셋팅
	SetPlayerInfos();
	//NPC 정보를 셋팅
	SetNPCInfos();	
	
	// 테스트 위한 인덱스 1번으로 스폰
	if (FMonsterSpawnInfo* spawnInfo = MonsterList.Find(1))
	{
		SpawnedMonsters.Emplace(spawnInfo);
	}

	// ToDo : 라운드,웨이브,스페이지별 npc , monster 들을 스폰시키고 in-active 시킨다. 
	for (auto iter : SpawnedMonsters)
	{
		if (nullptr!=iter->SpawnCharacter)
		{
			STServerHelper::GetAiController(iter->SpawnCharacter)->StartInActive();
		}
	}
}

void ASTGameMode::OnGameStateReady()
{
	// 클라이언트가 준비가 되었다.
}

void ASTGameMode::OnGameStateCountDown()
{
	ASTGameStateBase* stGameState = GetGameState<ASTGameStateBase>();
	if (nullptr != stGameState)
	{
		stGameState->CountDownTime = 5;
	}
	GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &ASTGameMode::OnCountDownTimer, 1.0f, false);
}

void ASTGameMode::OnGameStateStart()
{
	if (ASTGameStateBase* myGameState = GetGameState<ASTGameStateBase>())
	{
		myGameState->RemainingTime = 6000;
	}
	
	// 스폰 몬스터 활성화 시킨다.
	for (auto iter : SpawnedMonsters)
	{
		if (nullptr!=iter->SpawnCharacter)
		{
			STServerHelper::GetAiController(iter->SpawnCharacter)->StartActive();
		}
	}
}

void ASTGameMode::OnGameStateFinish()
{

}

void ASTGameMode::OnGameStateResult()
{

}

void ASTGameMode::OnCountDownTimer()
{
	if (ASTGameStateBase* myGameState = GetGameState<ASTGameStateBase>())
	{
		if (myGameState->CountDownTime <= 0)
		{
			ChangeGameModeState(ESTGameModeState::GState_Game_Start);
			return;
		}
		// 카운트 다운
		myGameState->CountDownTime--;
		GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &ASTGameMode::OnCountDownTimer, 1.0f, false);

		GEngine->AddOnScreenDebugMessage(1, 20.0f, FColor::Cyan, FString::Printf(TEXT("Game CountDown -  : %d"), myGameState->CountDownTime));

	}
}

#pragma endregion MainGamePlayTimer

#pragma region SpawnCharacter

void ASTGameMode::SetPlayerInfos()
{
	if (IsNetMode(NM_DedicatedServer))
	{
		//TO DO : 데디서버일경우 요청하고 정보를 받아서 처리 한다. 
		//Request Player Infos();
		SetPlayerInfosByEquipment();
	}
	else
	{
		//로컬용 게임 처리 
		SetPlayerInfosByEquipment();
	}	

	// 카운트다운 시작
	ChangeGameModeState(ESTGameModeState::GState_Game_Countdown);
}

void ASTGameMode::SetPlayerInfosByEquipment()
{
	// 캐릭터 정보 가지고 스폰
	this->SpawnPlayer();
	
	if (ASTPlayerController* myPC = Cast<ASTPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		//플레이컨트롤러에 정보 전달
	}	
}

void ASTGameMode::SpawnPlayer()
{
	//해당 캐릭터를 스폰한다.
	FActorSpawnParameters spawnInfo;
	spawnInfo.Instigator = GetInstigator();
	spawnInfo.ObjectFlags |= RF_Transient;
	spawnInfo.bDeferConstruction = true;

	ASTPlayerController* newPlayer = Cast<ASTPlayerController>(GetWorld()->GetFirstPlayerController());
	if (nullptr == newPlayer)
	{
		return;
	}

	FTransform spawnTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(1.f, 1.f, 1.f));
	AActor* startSpot = ChoosePlayerStart(newPlayer);
	if (startSpot)
	{
		spawnTransform.SetRotation(startSpot->GetActorQuat());
		spawnTransform.SetLocation(startSpot->GetActorLocation());
	}

	if (UClass* pawnClass = GetDefaultPawnClassForController(newPlayer))
	{
		if (APawn* spawnedPawn = GetWorld()->SpawnActor<APawn>(pawnClass, spawnTransform, spawnInfo))
		{
			if (USTPawnExtensionComponent* pawnExtComp = USTPawnExtensionComponent::FindPawnExtensionComponent(spawnedPawn))
			{
				if (const USTPawnData* pawnData = GetPawnDataForController(newPlayer))
				{
					pawnExtComp->SetPawnData(pawnData);
				}
				else
				{
					UE_LOG(LogSTGame, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(spawnedPawn));
				}
			}

			spawnedPawn->FinishSpawning(spawnTransform);

			if (IsValid(spawnedPawn))
			{				
				newPlayer->Possess(spawnedPawn);
			}
		}
	}
}

ASTCharacter* ASTGameMode::SpawnCharacter(int32 InSpawnIndex, USTCharacterInfo* InCharInfo)
{
	if (InSpawnIndex == -1)
	{
#if WITH_EDITOR
		FText dialogTitle = FText::FromString(TEXT("Error"));
		FText dialogText = FText::FromString(FString::Printf(TEXT("Not found SpawnPoint : %d."), InSpawnIndex));
		EAppReturnType::Type m_type = FMessageDialog::Open(EAppMsgType::Ok, dialogText, dialogTitle);
#endif 
		return nullptr;
	}

	if (AActor* spawnSpot = STServerHelper::FindSpawnPoint(GetWorld(), InSpawnIndex))
	{
		ASTCharacter* spawnee = STServerHelper::SpawnCharacter(GetWorld(), InCharInfo, spawnSpot->GetActorLocation(), spawnSpot->GetActorRotation());
		if (spawnee)
		{
			//UE_LOG(LogSTGame, Display, TEXT("ASTGameMode::SpawnCharacter() spawnIndex=%d, charID=%d"), InSpawnIndex, InCharInfo->GetUID());
			
		}
		return spawnee;
	}

	return nullptr;
}

void ASTGameMode::SetNPCInfos()
{
	// NPC 정보를 셋팅  	
	if(IsNetMode(NM_DedicatedServer))
	{
		//TO DO : 데디서버일경우 요청하고 정보를 받아서 처리 한다. 
		//Request NPC Monster Infos();
	}
	else
	{
		//로컬용 게임 처리 
	}	
}

#pragma endregion SpawnCharacter

