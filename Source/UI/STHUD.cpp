// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/STHUD.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Common//STHelper.h"
#include "UI/InGame/STUIContentsInGame.h"
#include "UI/InGame/STUI_InGame.h"

#include "DisplayDebugHelpers.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(STHUD)

class AActor;
class UWorld;

ASTHUD::ASTHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ASTHUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASTHUD::BeginPlay()
{
	Super::BeginPlay();
	bShowOverlays = true;

	InitUI();
}

void ASTHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASTHUD::DrawHUD()
{
	Super::DrawHUD();
}

void ASTHUD::ShowHUD()
{
	Super::ShowHUD();
}

void ASTHUD::ShowDebug(FName DebugType)
{
	Super::ShowDebug(DebugType);	
}

void ASTHUD::ShowDebugInfo(float& YL, float& YPos)
{
	if (DebugCanvas != nullptr && DebugCanvas->Canvas != nullptr)
	{
		FLinearColor BackgroundColor(0.f, 0.f, 0.f, 0.2f);
		DebugCanvas->Canvas->DrawTile(0, 0, 0.5f * DebugCanvas->ClipX, 0.5f * DebugCanvas->ClipY, 0.f, 0.f, 0.f, 0.f, BackgroundColor);

		FDebugDisplayInfo DisplayInfo(DebugDisplay, ToggledDebugCategories);
		PlayerOwner->PlayerCameraManager->ViewTarget.Target->DisplayDebug(DebugCanvas, DisplayInfo, YL, YPos);

		if (ShouldDisplayDebug(NAME_Game))
		{			
			if (!IsNetMode(ENetMode::NM_Client))
			{
				GetWorld()->GetAuthGameMode()->DisplayDebug(DebugCanvas, DisplayInfo, YL, YPos);
			}
		}
	}
}

