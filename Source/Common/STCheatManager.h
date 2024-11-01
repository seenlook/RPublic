// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "STCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTR_API USTCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	
	UFUNCTION(Exec, BlueprintCallable, meta = (OverrideNativeName = "ST.CheatTest"))
	void CheatTest();

	UFUNCTION(Exec, BlueprintCallable, meta = (OverrideNativeName = "ST.SetHP"))
	void SetHP(float InHP = 1.0f);

	UFUNCTION(Exec, BlueprintCallable, meta = (OverrideNativeName = "ST.SetSP"))
	void SetSP(float InSP = 1.0f);
	
};
