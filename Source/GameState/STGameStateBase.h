// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "Misc/EnumRange.h"
#include "STGameStateBase.generated.h"

class APlayerState;
class UAbilitySystemComponent;
class USTAbilitySystemComponent;
class USTExperienceManagerComponent;
class UObject;
struct FFrame;
class ASTCharacter;

struct FSTVerbMessage;

// 게임 상태 정의
UENUM(BlueprintType)
enum class ESTGameModeState : uint8
{
	GState_None = 0, 
	GState_Mode_Init,		// 게임모드 초기화가 진행중이다.
	GState_Mode_Wait,		// 게임모드 초기화가 끝나고 클라이언트 게임시작을 기다리는 중이다.
	GState_Mode_Ready,		// 게임모드 모든 클라이언트 게임준비가 되었다는 걸 알린다.
	GState_Game_Countdown,	// 게임 플레이 시작 카운트 다운 
	GState_Game_Start,		// 게임 플레이 시작 됐다.
	GState_Game_Finish,		// 게임 종료	
	GState_Game_Result,		// 게임 결과
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ESTGameModeState, ESTGameModeState::Count);

// 상태변경 구조체
USTRUCT()
struct FSTGameModeState
{
	GENERATED_BODY()

public:
	FSTGameModeState() : State(ESTGameModeState::GState_None) , IntArg(0)
	{ 
		
	}

	FSTGameModeState(ESTGameModeState InState, int32 InArg) 
	{
		State	= InState;
		IntArg	= InArg;
	}

	void Reset()
	{
		State = ESTGameModeState::GState_None;
		IntArg = 0;
	}

	UPROPERTY()
	ESTGameModeState State = ESTGameModeState::GState_None;
	UPROPERTY()
	int32 IntArg = 0;
};

USTRUCT()
struct FSTGameModeStateList
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ESTGameModeState CurState = ESTGameModeState::GState_None; 

	UPROPERTY()
	TArray<FSTGameModeState> StateList;

	FSTGameModeStateList()
	{
		CurState = ESTGameModeState::GState_None;
		StateList.Empty();
		for (ESTGameModeState ElementType : TEnumRange<ESTGameModeState>())
		{			
			StateList.Emplace(FSTGameModeState());
		}
	}
};

/**
 * 
 */
UCLASS(config=Game)
class PROJECTR_API ASTGameStateBase : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ASTGameStateBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	//~End of AActor interface

	//~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
	//~End of AGameStateBase interface

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	// 게임 전반에 사용되는 Ability System 구성 요소를 가져옵니다.
	UFUNCTION(BlueprintCallable, Category = "STGame|GameState")
	USTAbilitySystemComponent* GetSTAbilitySystemComponent() const { return AbilitySystemComponent; }

	UFUNCTION()
	void OnRep_RemainingTime();
	UFUNCTION()
	void OnRep_CountDownTime();
	UFUNCTION()
	void OnRep_GameModeStateList();	
	
	// 서버의 FPS를 가져와 클라이언트에 리플리케이트 합니다.
	float GetServerFPS() const;

	// 게임모드 상태 저장
	void SetGameModeState(const FSTGameModeState& InGameModeState);
	// 캐릭터가 적대적이냐 ?  
	bool IsHostile(const ASTCharacter* InCharA, const ASTCharacter* InCharB);
	// 현재 게임모드 상태 
	FORCEINLINE ESTGameModeState GetCurGameModeState() { return GameModeStateList.CurState; }


private:
	// 현재 gameplay experience 을 로드하고 관리합니다.
	UPROPERTY()
	TObjectPtr<USTExperienceManagerComponent> ExperienceManagerComponent;

	// 게임 전체에 대한 능력 시스템 구성 요소 하위 객체(주로 gameplay cue)
	UPROPERTY(VisibleAnywhere, Category = "STGame|GameState")
	TObjectPtr<USTAbilitySystemComponent> AbilitySystemComponent;

protected:
	/** 데디측 fps  */
	UPROPERTY(Replicated)
	float ServerFPS;

	/** 게임모드 상태 설정 */
	UPROPERTY(ReplicatedUsing = OnRep_GameModeStateList)
	FSTGameModeStateList GameModeStateList;

	/**  */
	UPROPERTY()
	TArray<ESTGameModeState> RecvGameModeStateList;

public:

	/** 플레이 시간  */
	UPROPERTY(BlueprintReadWrite, Transient, Replicated)
	int32 TotalPlayTime = 0;

	/** 남은 시간 */
	UPROPERTY(BlueprintReadWrite, Transient, ReplicatedUsing = "OnRep_RemainingTime")
	int32 RemainingTime = 0;

	/** 스타트 카운트 다운 */
	UPROPERTY(BlueprintReadWrite, Transient, ReplicatedUsing = "OnRep_CountDownTime")
	int32 CountDownTime = 0;	
	
};
