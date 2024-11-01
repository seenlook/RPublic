// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STPolicyDeActive.h"
#include "STCharacterAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STPolicyDeActive)

USTPolicyDeActive::USTPolicyDeActive(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetType<EPolicyType>(EPolicyType::PCT_DeActive);
}

void USTPolicyDeActive::Enter(const EnterParam& InEnterParam, USTState* InPreviousState)
{
	Super::Enter(InEnterParam, InPreviousState);
	GetOwner<ASTCharacterAIController>()->SetActive(false);
}

void USTPolicyDeActive::Leave(USTState* InNextState)
{
	Super::Leave(InNextState);
	GetOwner<ASTCharacterAIController>()->SetActive(true);
}
