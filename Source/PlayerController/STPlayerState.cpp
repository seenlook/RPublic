// Fill out your copyright notice in the Description page of Project Settings.


#include "STPlayerState.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h" 
#include "GameFramework/GameplayMessageSubsystem.h"

#include "../Character/STPawnData.h"
#include "../Character/STPawnExtensionComponent.h"
#include "../GameMode/STGameMode.h"
#include "../Infos/STLogChannels.h"
#include "../PlayerController/STPlayerController.h"
#include "../GameExperience/STExperienceManagerComponent.h"
#include "../AbilitySystem/STAbilitySystemComponent.h"
#include "../AbilitySystem/STAbilitySet.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(STPlayerState)

class AController;
class APlayerState;
class FLifetimeProperty;

const FName ASTPlayerState::NAME_STAbilityReady("STAbilitiesReady");

ASTPlayerState::ASTPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
	STAbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<USTAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	STAbilitySystemComponent->SetIsReplicated(true);
	STAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);	// Mixed 는 플레이어에 (Minimal 은 AI 에) 서버에는 게임플레이 이펙트만 전송 	
	
	// AbilitySystemComponent는 높은 빈도로 업데이트되어야 합니다.
	NetUpdateFrequency = 100.0f;
}

void ASTPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASTPlayerState::Reset()
{
	Super::Reset();
}

void ASTPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (USTPawnExtensionComponent* PawnExtComp = USTPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void ASTPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

}

void ASTPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;
	bDestroyDeactivatedPlayerState = true;
	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void ASTPlayerState::OnReactivated()
{
	
}

void ASTPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASTPlayerState, KillCount);
	DOREPLIFETIME(ASTPlayerState, DeathCount);

	DOREPLIFETIME(ASTPlayerState, MainWeaponID);
	DOREPLIFETIME(ASTPlayerState, SubWeaponID);

	DOREPLIFETIME(ASTPlayerState, FaintCount);
	DOREPLIFETIME(ASTPlayerState, IsFainting);
	DOREPLIFETIME(ASTPlayerState, IsDeath);

	DOREPLIFETIME(ASTPlayerState, IsToTreat);
	DOREPLIFETIME(ASTPlayerState, IsTreatedBy);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);	
}

FRotator ASTPlayerState::GetReplicatedViewRotation() const
{
	return ReplicatedViewRotation;
}

void ASTPlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)
{
	if (NewRotation != ReplicatedViewRotation)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedViewRotation, this);
		ReplicatedViewRotation = NewRotation;
	}
}

ASTPlayerController* ASTPlayerState::GetMyPlayerController() const
{
	return Cast<ASTPlayerController>(GetOwner());
}

UAbilitySystemComponent* ASTPlayerState::GetAbilitySystemComponent() const
{
	USTAbilitySystemComponent* STASC = GetSTAbilitySystemComponent();
	return STASC;
}

void ASTPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	check(STAbilitySystemComponent);
	STAbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		USTExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USTExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnSTExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
	
}

void ASTPlayerState::SetPawnData(const USTPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogSTGame, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;
	
	for (const USTAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(STAbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_STAbilityReady);
	

	ForceNetUpdate();
}

void ASTPlayerState::OnRep_PawnData()
{

}

void ASTPlayerState::OnExperienceLoaded(const USTExperienceDefinition*)
{
	if (ASTGameMode* MyGameMode = GetWorld()->GetAuthGameMode<ASTGameMode>() )
	{
		if (const USTPawnData* NewPawnData = MyGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogSTGame, Error, TEXT("ASTPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
	else
	{
		//UE_LOG(LogSTGame, Error, TEXT("ASTPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
	}
}

void ASTPlayerState::SetIsDeath(bool InDeath)
{
	IsDeath = InDeath;
	DeathCount++;	
}

void ASTPlayerState::SetIsFainting(bool InFainting)
{
	IsFainting = InFainting;
}

void ASTPlayerState::OnRepKillCount()
{
	// 서버에서 처리해야 할것.
#if defined(STGame_SERVER)

#endif 
}

void ASTPlayerState::OnRepDeathCount()
{
	// 서버에서 처리해야 할것.
#if defined(STGame_SERVER)

#endif 
}

void ASTPlayerState::SetCurrentArmor(int32 InArmor)
{
	CurrentArmor = InArmor;
}

void ASTPlayerState::SetMaxArmor(int32 InArmor)
{
	MaxArmor = InArmor;
}

void ASTPlayerState::SetArmorGrade(int32 InArmorGrade)
{
	ArmorGrade = InArmorGrade;
}