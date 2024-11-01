// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Common/STState.h"
#include "../Game/STGameType.h"
#include "STPolicy.generated.h"

/**
 * @class : USTPolicy
 * @brief : 캐릭터의 상태를 나타내는 정책을 담당한다.
 */
UCLASS()
class PROJECTR_API USTPolicy : public USTState
{
	GENERATED_BODY()
protected:
	USTPolicy(const FObjectInitializer& ObjectInitializer);

public:
	virtual void Enter(const EnterParam& InEnterParam, USTState* InPreState) override;
	virtual void Leave(USTState* InNexState) override;	

	virtual bool IsAttackable() { return true; }	// 공격 가능
	virtual bool IsHittable() { return true; }		// 피격 가능 
	virtual bool IsMovable() { return true; }		// 이동 가능 

	FORCEINLINE EPolicyType GetPolicyType() { return GetType<EPolicyType>(); }
	
};
