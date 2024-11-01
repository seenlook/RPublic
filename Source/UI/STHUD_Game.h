// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUD/STHUD.h"
#include "STHUD_Game.generated.h"

/**
 * @class : ASTHUD_Game - In-Game HUD
 * @brief : 인게임 정보 표시 
 */
UCLASS()
class PROJECTR_API ASTHUD_Game : public ASTHUD
{
	GENERATED_BODY()
	ASTHUD_Game(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual void PreInitializeComponents() override;
	//~End of UObject interface

	// AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface

	virtual void InitUI() override;
	virtual void SetVisibleUI(bool bInIsVisible) override;
	
};
