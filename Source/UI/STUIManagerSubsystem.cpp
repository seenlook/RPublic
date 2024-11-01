// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Subsystem/STUIManagerSubsystem.h"

#include "CommonLocalPlayer.h"
#include "Engine/GameInstance.h"
#include "GameFramework/HUD.h"
#include "GameUIPolicy.h"
#include "PrimaryGameLayout.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STUIManagerSubsystem)

class FSubsystemCollectionBase;

USTUIManagerSubsystem::USTUIManagerSubsystem()
{
}

void USTUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &USTUIManagerSubsystem::Tick), 0.0f);

	bIsInit = true;
}

void USTUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);

	RemoveAllContents();
	RemoveAllSystem();

	Systems.Reset();
	Contents.Reset();
	ContentsReserve.Reset();
	SystemsReserve.Reset();
}

bool USTUIManagerSubsystem::Tick(float DeltaTime)
{
	SyncRootLayoutVisibilityToShowHUD();

	ProcessReserve();

	float deltaSeconds = DeltaTime;
	for (TPair<UClass*,USTUISystem*> iter : Systems)
	{
		auto* value = iter.Value;
		if (true == value->CanTick(OUT deltaSeconds))
		{
			value->Tick(deltaSeconds);
		}
	}

	for (TPair<UClass*, USTUIContents*> iter : Contents)
	{
		auto* value = iter.Value;
		if (true == value->CanTick(OUT deltaSeconds))
		{
			value->Tick(deltaSeconds);
		}
	}

	return true;
}

void USTUIManagerSubsystem::AddSystem(UClass* InClass, USTUISystem* InUI)
{
	check(InClass);
	check(InUI);

	if (!SystemsReserve.Contains(InClass))
	{
		SystemsReserve.Emplace(InClass, InUI);
		InUI->Startup();
	}

	ProcessReserve();
}

void USTUIManagerSubsystem::AddContent(UClass* InClass, USTUIContents* InContents)
{
	check(InClass);
	check(InContents);

	if (!ContentsReserve.Contains(InClass))
	{
		ContentsReserve.Emplace(InClass, InContents);
		InContents->Startup();
	}

	ProcessReserve();
}

void USTUIManagerSubsystem::ProcessReserve()
{
	if (SystemsReserve.Num() >= 1)
	{
		for (const auto& pair : SystemsReserve)
		{
			Systems.Add(pair.Key, pair.Value);
		}
		SystemsReserve.Reset();
	}

	if (ContentsReserve.Num() >= 1)
	{
		for (const auto& pair : ContentsReserve)
		{
			Contents.Add(pair.Key, pair.Value);
		}
		ContentsReserve.Reset();
	}
}

void USTUIManagerSubsystem::RemoveAllSystem()
{
	for (TPair<UClass*, USTUISystem*> iter : Systems)
	{
		if (IsValid(iter.Value))
		{
			iter.Value->Shutdown();
		}
	}

	TArray<USTUISystem*> systemsForRemoval;
	Systems.GenerateValueArray(systemsForRemoval);

	for (int32 reverseIndex = systemsForRemoval.Num() - 1; 0 <= reverseIndex; reverseIndex--)
	{
		if (systemsForRemoval[reverseIndex])
		{
			systemsForRemoval[reverseIndex]->Shutdown();

			Systems.Remove(systemsForRemoval[reverseIndex]->GetClass());
		}
	}
	systemsForRemoval.Empty();
}

void USTUIManagerSubsystem::RemoveAllContents()
{
	for (TPair<UClass*, USTUIContents*> iter : Contents)
	{
		if (IsValid(iter.Value))
		{
			iter.Value->Shutdown();
		}
	}

	TArray<USTUIContents*> contentsForRemoval;
	Contents.GenerateValueArray(contentsForRemoval);

	for (int32 reverseIndex = contentsForRemoval.Num() - 1; 0 <= reverseIndex; reverseIndex--)
	{
		if (contentsForRemoval[reverseIndex])
		{
			contentsForRemoval[reverseIndex]->Shutdown();

			Contents.Remove(contentsForRemoval[reverseIndex]->GetClass());
		}
	}
	contentsForRemoval.Empty();
}

void USTUIManagerSubsystem::SyncRootLayoutVisibilityToShowHUD()
{
	if (const UGameUIPolicy* Policy = GetCurrentUIPolicy())
	{
		for (const ULocalPlayer* LocalPlayer : GetGameInstance()->GetLocalPlayers())
		{
			bool bShouldShowUI = true;

			if (const APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld()))
			{
				const AHUD* HUD = PC->GetHUD();

				if (HUD && !HUD->bShowHUD)
				{
					bShouldShowUI = false;
				}
			}

			if (UPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CastChecked<UCommonLocalPlayer>(LocalPlayer)))
			{
				const ESlateVisibility DesiredVisibility = bShouldShowUI ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
				if (DesiredVisibility != RootLayout->GetVisibility())
				{
					RootLayout->SetVisibility(DesiredVisibility);
				}
			}
		}
	}
}

