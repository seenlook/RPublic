// Fill out your copyright notice in the Description page of Project Settings.


#include "STGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Async/TaskGraphInterfaces.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "AbilitySystem/STAbilitySystemComponent.h"
#include "GameExperience/STExperienceManagerComponent.h"
#include "PlayerController/STPlayerState.h"
#include "Infos/STLogChannels.h"
#include "Character/STCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STGameStateBase)

class APlayerState;
class FLifetimeProperty;

extern ENGINE_API float GAverageFPS;

ASTGameStateBase::ASTGameStateBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<USTAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ExperienceManagerComponent = CreateDefaultSubobject<USTExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));

	ServerFPS = 0.0f;
}

void ASTGameStateBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASTGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

UAbilitySystemComponent* ASTGameStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASTGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TotalPlayTime);
	DOREPLIFETIME(ThisClass, RemainingTime);
	DOREPLIFETIME(ThisClass, CountDownTime);	
	DOREPLIFETIME(ThisClass, ServerFPS);	
}

void ASTGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	RecvGameModeStateList.Empty();
	for (ESTGameModeState ElementType : TEnumRange<ESTGameModeState>())
	{
		RecvGameModeStateList.Emplace(ESTGameModeState::GState_None);
	}
}

void ASTGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASTGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void ASTGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
}

void ASTGameStateBase::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)
{	
	for (int32 i = PlayerArray.Num() - 1; i >= 0; i--)
	{
		APlayerState* PlayerState = PlayerArray[i];
		if (PlayerState && (PlayerState->IsABot() || PlayerState->IsInactive()))
		{
			RemovePlayerState(PlayerState);
		}
	}
}

void ASTGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
}

float ASTGameStateBase::GetServerFPS() const
{
	return ServerFPS;
}

void ASTGameStateBase::SetGameModeState(const FSTGameModeState& InGameModeState)
{
	if (!IsNetMode(NM_Client))
	{
		GameModeStateList.CurState = InGameModeState.State;
		GameModeStateList.StateList[(int32)InGameModeState.State] = InGameModeState;
	}

	if (IsNetMode(NM_Standalone))
	{
		OnRep_GameModeStateList();
	}
}

void ASTGameStateBase::OnRep_GameModeStateList()
{

}

void ASTGameStateBase::OnRep_RemainingTime()
{
	//
}

void ASTGameStateBase::OnRep_CountDownTime()
{
	//
}

bool ASTGameStateBase::IsHostile(const ASTCharacter* InCharA, const ASTCharacter* InCharB)
{
	
	return false;
}

