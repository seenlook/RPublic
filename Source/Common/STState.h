// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "STState.generated.h"

/**
 * @class : USTState
 * @brief : 상태 값을 정의한다.
 */
UCLASS()
class PROJECTR_API USTState : public UObject
{
	GENERATED_BODY()
public:
	struct EnterParam
	{
		EnterParam()
			: iValue(0), bValue(false), fValue(0.0f), fValue1(0.0f), vValue(0.f, 0.f, 0.f), actorRef(nullptr), nameValue(NAME_None)
		{
		}

		int iValue = 0;
		bool bValue = false;
		float fValue = 0.f;
		float fValue1 = 0.f;
		FVector vValue = FVector::ZeroVector;
		AActor* actorRef = nullptr;
		FName nameValue = NAME_None;
	};

	using TypeOfState = uint32;

	USTState(const FObjectInitializer& ObjectInitializer);

	virtual void Enter(const EnterParam& enterParam, USTState* previousState);
	virtual void Leave(USTState* nextState);
	virtual void Tick(float DeltaTime);

	FORCEINLINE void Initialize(AActor* owner) { check(owner != nullptr); Owner = owner; }
	template< typename T >
	FORCEINLINE bool IsType(T type) { return (StateType == (TypeOfState)type); }
	template< typename T >
	FORCEINLINE T GetType() { return (T)StateType; }

protected:
	template< typename T >
	FORCEINLINE void SetType(T type) { StateType = (TypeOfState)type; }	
	
	template< typename T >
	FORCEINLINE_DEBUGGABLE T* GetOwner() { return Cast<T>(Owner.Get()); }

	bool IsActivate;

private:
	EnterParam ThisEnterParam;		
	TypeOfState StateType = 0;
	TWeakObjectPtr<AActor> Owner;
	
};
