// Fill out your copyright notice in the Description page of Project Settings.


#include "STState.h"

USTState::USTState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), IsActivate(false)
{

}

void USTState::Enter(const EnterParam& enterParam, USTState* previousState)
{
	ensure(previousState);

	ThisEnterParam = enterParam;
	IsActivate = true;
}

void USTState::Leave(USTState* nextState)
{
	ensure(nextState);

	IsActivate = false;
}

void USTState::Tick(float DeltaTime)
{

}
