// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/STCheatManager.h"
#include <Engine/Engine.h>
#include <Containers/UnrealString.h>

#include "PlayerController/STPlayerController.h"
#include "Character/STCharacter.h"
#include "PlayerController/STPlayerState.h"

void USTCheatManager::CheatTest()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("msg : s | param1 : f | param2 d")/*, *Msg, param1, param2*/));
}

void USTCheatManager::SetHP(float InHP)
{
#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	if (ASTPlayerController* myPC = Cast<ASTPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
	{
		if (ASTCharacter* myChar = Cast<ASTCharacter>(myPC->GetPawn()))
		{
			//myChar->SetHP(InHP);
		}
	}
#endif
}

void USTCheatManager::SetSP(float InSP)
{
#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	if (ASTPlayerController* myPC = Cast<ASTPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
	{
		if (ASTCharacter* myChar = Cast<ASTCharacter>(myPC->GetPawn()))
		{
			//myPS->SetSP(InSP);	
		}
	}
#endif
}