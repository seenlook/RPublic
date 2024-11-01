// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STPolicyActive.h"
#include "AI/STCharacterAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STPolicyActive)

USTPolicyActive::USTPolicyActive(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetType<EPolicyType>(EPolicyType::PCT_Active);
}

void USTPolicyActive::Enter(const EnterParam& InEnterParam, USTState* InPreviousState)
{
	Super::Enter(InEnterParam, InPreviousState);	
	GetOwner<ASTCharacterAIController>()->SetActive(true);
}

void USTPolicyActive::Leave(USTState* InNextState)
{
	Super::Leave(InNextState);
}