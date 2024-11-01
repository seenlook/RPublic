// Fill out your copyright notice in the Description page of Project Settings.


#include "STHelper.h"

#include "Game/STGameInstance.h"
#include "GameState/STGameStateBase.h"
#include "PlayerController/STLocalPlayer.h"
#include "Character/STCharacter.h"

#include "UI/Subsystem/STUISystem.h"
#include "UI/Subsystem/STUIContents.h"
#include "UI/Lobby/STUIContentsLobby.h"
#include "UI/InGame/STUIContentsInGame.h"

#include "UI/Subsystem/STUIManagerSubsystem.h"


namespace STHelper
{
	USTGameInstance* GetGameInstance(UWorld* InWorld)
	{
		USTGameInstance* Instance = nullptr != InWorld ? Cast<USTGameInstance>(InWorld->GetGameInstance()) : nullptr;
		return Instance;
	}

	USTLocalPlayer* GetLocalPlayer(UWorld* InWorld)
	{
		return Cast<USTLocalPlayer>(GetGameInstance(InWorld)->GetFirstGamePlayer());
	}

	USTUIManagerSubsystem* GetUIManagerSubSystem(UWorld* InWorld)
	{
		if (nullptr != InWorld)
		{
			return UGameInstance::GetSubsystem<USTUIManagerSubsystem>(GetGameInstance(InWorld));
		}
		return nullptr;
	}

	USTUISystem* GetUISystem(UWorld* InWorld)
	{
		if (nullptr != InWorld)
		{
			if (UGameInstance* GameInstance = InWorld->GetGameInstance())
			{
				if (USTUIManagerSubsystem* UIManager = UGameInstance::GetSubsystem<USTUIManagerSubsystem>(GameInstance))
				{
					return UIManager->GetUISystem<USTUISystem>();
				}
			}
		}
		return nullptr;
	}	
		
	USTUIContentsLobby* GetUIContentsLobby(UWorld* InWorld)
	{
		if (nullptr != InWorld)
		{
			if (UGameInstance* GameInstance = InWorld->GetGameInstance())
			{
				if (USTUIManagerSubsystem* UIManager = UGameInstance::GetSubsystem<USTUIManagerSubsystem>(GameInstance))
				{
					return UIManager->GetUIContents<USTUIContentsLobby>();
				}
			}
		}
		return nullptr;
	}

	USTUIContentsInGame* GetUIContentsInGame(UWorld* InWorld)
	{
		if (nullptr != InWorld)
		{
			if (UGameInstance* GameInstance = InWorld->GetGameInstance())
			{
				if (USTUIManagerSubsystem* UIManager = UGameInstance::GetSubsystem<USTUIManagerSubsystem>(GameInstance))
				{
					return UIManager->GetUIContents<USTUIContentsInGame>();
				}
			}
		}
		return nullptr;
	}

	// 두 캐릭터가 서로 적대적인지 체크한다.
	bool IsHostile(const ASTCharacter* InCharA, const ASTCharacter* InCharB)
	{
		if (InCharA == nullptr) return false;
		if (InCharB == nullptr) return false;

		ASTGameStateBase* myGameState = InCharA->GetWorld()->GetGameState<ASTGameStateBase>();
		if (myGameState != nullptr)
		{
			return myGameState->IsHostile(InCharA, InCharB);
		}
		return false;
	}

	void SetActorTickActivate(AActor* InActor, bool bEnabled)
	{
		ensure(InActor);
		InActor->SetActorTickEnabled(bEnabled);		

		TArray<UActorComponent*> components;
		InActor->GetComponents(components);

		for (UActorComponent* comp : components)
		{
			comp->SetActive(bEnabled);
		}
	}
	

}; // end of namespace Helper


namespace CommonUIPath
{
	FString GetFullPath(ESTUIPathCategory category, const FString& name)
	{
		FString path = TEXT("");
		switch (category)
		{
		case ESTUIPathCategory::UI:
			path.Append(UI_PATH);
			break;
		case ESTUIPathCategory::WIDGET:
			path.Append(WIDGET_PATH);
			break;
		case ESTUIPathCategory::WIDGETCOMPONENT:
			path.Append(WIDGET_COMPONENT_PATH);
			break;
		case ESTUIPathCategory::TEXTURE:
			break;
		case ESTUIPathCategory::MATERIAL:
			break;
		case ESTUIPathCategory::SPRITE:
			break;
		case ESTUIPathCategory::SOUND:
			break;
		default:			
			break;
		}
		path.Append(name);
		return path;
	}

};

