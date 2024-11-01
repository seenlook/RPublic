// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "GameUIManagerSubsystem.h"
#include "STUISystem.h"
#include "STUIContents.h"
#include "STUIManagerSubsystem.generated.h"

class FSubsystemCollectionBase;
class UObject;

/**
 * @class : USTUIManagerSubsystem
 * @brief : UI 관리하는 서브시스템 , UGameInstanceSubsytem 상속
 */
UCLASS()
class PROJECTR_API USTUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()
public:

	USTUIManagerSubsystem();	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();

	FTSTicker::FDelegateHandle TickHandle;

protected:
	
	UPROPERTY()
	bool bIsInit;

private:
	// 시스템 관리 맵컨테이너
	UPROPERTY()
	TMap<UClass*, USTUISystem*> Systems;

	// 시스템 예약 맵컨테이너 
	UPROPERTY()
	TMap<UClass*, USTUISystem*> SystemsReserve;

	// 컨텐츠 관리 맵컨테이너
	UPROPERTY()
	TMap<UClass*, USTUIContents*> Contents;

	// 컨텐츠 예약 맵켄테이너 
	UPROPERTY()
	TMap<UClass*, USTUIContents*> ContentsReserve;

public:
	
	template<class T>
	T* GetUISystem();

	template<class T>
	void AddUISystem();

	template<class T>
	T* GetUIContents();

	template<class T>
	void AddUIContents();

protected:
	
	void AddSystem(UClass* InClass, USTUISystem* InUI);

	void AddContent(UClass* InClass, USTUIContents* InContents);

	void ProcessReserve();

	void RemoveAllSystem();

	void RemoveAllContents();

	template<class T>
	T* GetSystems();

	template<class T>
	T* GetContents();
	
};

template<class T>
T* USTUIManagerSubsystem::GetUISystem()
{
	if (GetSystems<T>() == nullptr)
	{
		auto* staticClass = T::StaticClass();
		AddSystem(staticClass, NewObject<T>(this));
	}

	return Cast<T>(GetSystems<T>());
}

template<class T>
void USTUIManagerSubsystem::AddUISystem()
{
	AddSystem(T::StaticClass(), NewObject<T>(this));
}

template<class T>
T* USTUIManagerSubsystem::GetUIContents()
{
	if (GetContents<T>() == nullptr)
	{
		auto* staticClass = T::StaticClass();
		AddContent(staticClass, NewObject<T>(this));
	}

	return Cast<T>(GetContents<T>());
}

template<class T>
void USTUIManagerSubsystem::AddUIContents()
{
	AddContents(T::StaticClass(), NewObject<T>(this));
}

template<class T>
T* USTUIManagerSubsystem::GetSystems()
{
	auto* staticClass = T::StaticClass();

	if (SystemsReserve.Contains(staticClass))
	{
		return Cast<T>(*SystemsReserve.Find(staticClass));
	}

	if (Systems.Contains(staticClass))
	{
		return Cast<T>(*Systems.Find(staticClass));
	}

	return nullptr;
}

template<class T>
T* USTUIManagerSubsystem::GetContents()
{
	auto* staticClass = T::StaticClass();

	if (ContentsReserve.Contains(staticClass))
	{
		return Cast<T>(*ContentsReserve.Find(staticClass));
	}
	if (Contents.Contains(staticClass))
	{
		return Cast<T>(*Contents.Find(staticClass));
	}

	return nullptr;
}

