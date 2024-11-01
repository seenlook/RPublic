// Fill out your copyright notice in the Description page of Project Settings.


#include "STPolicy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STPolicy)

USTPolicy::USTPolicy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USTPolicy::Enter(const EnterParam& InEnterParam, USTState* InPreviousState)
{
	Super::Enter(InEnterParam, InPreviousState);
}

void USTPolicy::Leave(USTState* InNextState)
{
	Super::Leave(InNextState);
}