// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/STPolicy.h"
#include "STPolicyActive.generated.h"

/**
 * @class : USTPolicyActive
 * @class : 상호 작용(interaction)이 가능하고 play 가능상태 
 */
UCLASS()
class PROJECTR_API USTPolicyActive : public USTPolicy
{
	GENERATED_BODY()
	USTPolicyActive(const FObjectInitializer& ObjectInitializer);

public:
	virtual void Enter(const EnterParam& InEnterParam, USTState* InPreviousState) override;
	virtual void Leave(USTState* InNextState) override;
};
