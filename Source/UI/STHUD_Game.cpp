// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/STHUD_Game.h"
#include "Common/STHelper.h"
#include "UI/Subsystem/STUISystem.h"
#include "UI/Subsystem/STUIContents.h"
#include "UI/InGame/STUIContentsInGame.h"
#include "UI/InGame/STUI_InGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STHUD_Game)

class AActor;
class UWorld;

ASTHUD_Game::ASTHUD_Game(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ASTHUD_Game::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASTHUD_Game::BeginPlay()
{
	Super::BeginPlay();

	if (USTUISystem* UISystem = STHelper::GetUISystem(GetWorld()))
	{
		UISystem->AllHide(USTUI_InGame::StaticClass());
		//UISystem->Show<USTUI_InGame>();
	}
}

void ASTHUD_Game::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (USTUISystem* UISystem = STHelper::GetUISystem(GetWorld()))
	{
		UISystem->Hide<USTUI_InGame>();
	}

	Super::EndPlay(EndPlayReason);
}

void ASTHUD_Game::InitUI()
{
	Super::InitUI();

}

void ASTHUD_Game::SetVisibleUI(bool bInIsVisible)
{
	if (USTUISystem* UISystem = STHelper::GetUISystem(GetWorld()))
	{
		if (bInIsVisible)
		{
			UISystem->Show<USTUI_InGame>();
		}
		else
		{
			UISystem->Hide<USTUI_InGame>();
		}
	}
}