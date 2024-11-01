// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "STHUD.generated.h"

/**
 * @class : ASTHUD
 * @brief : 기본 HUD 를 표시한다.
 */
UCLASS()
class PROJECTR_API ASTHUD : public AHUD
{
	GENERATED_BODY()

public:
public:
	ASTHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual void PreInitializeComponents() override;
	//~End of UObject interface

	// AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface

	//AHUD interface
	virtual void ShowHUD() override;
	virtual void ShowDebug(FName DebugType = NAME_None) override;
	virtual void ShowDebugInfo(float& YL, float& YPos) override;
		
	virtual void DrawHUD() override;
	//~ End AHUD interface

	/*
	* Widget UI 를 초기화 한다.
	*/
	virtual void InitUI() {};

	/*
	* UI visible on/off 
	*/
	virtual void SetVisibleUI(bool bInIsVisible) {};
	
};
