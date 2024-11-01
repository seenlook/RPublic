// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/STPolicy.h"
#include "STPolicyDeActive.generated.h"

/**
 * @class : USTPolicyDeActive
 * @brief : 상태정책 에서 제거될때 호출된다. 
 */
UCLASS()
class PROJECTR_API USTPolicyDeActive : public USTPolicy
{
	GENERATED_BODY()
	USTPolicyDeActive(const FObjectInitializer& ObjectInitializer);

public:
	virtual void Enter(const EnterParam& InEnterParam, USTState* InPreviousState) override;
	virtual void Leave(USTState* InNextState) override;

	virtual bool IsAttackable() override	{ return false; }		// 공격 x
	virtual bool IsHittable() override		{ return false; }		// 피격 x
	virtual bool IsMovable() override		{ return false; }		// 이동 x
	
};
