// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Logging/LogMacros.h"  
#include "Common/STState.h"
#include "STPolicy.h"
#include "STCharacterAIController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(STCharacterAIControllerLogCategory, Log, All);

class ASTCharacter;
enum class EPolicyType : uint8;

/**
 * @class : ASTCharacterAIController
 * @brief : 캐릭터의 AI 행동을 지시하고 , 상태 정책을 관리한다.
 *          
 */
UCLASS()
class PROJECTR_API ASTCharacterAIController : public AAIController
{
	GENERATED_BODY()
   
protected:
	ASTCharacterAIController(const FObjectInitializer& ObjectInitializer);
    virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
    virtual void Tick(float DeltaSeconds) override;

	void AddPoliciesToMap(EPolicyType eType, TSubclassOf<USTPolicy> InPolicyClass);

public:

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

	virtual void SetAIPolicies();
	virtual void InitPolicyState();
	virtual bool StartInActive();
	virtual bool StartActive();
	virtual bool StartDeActive();
	virtual bool StartIdle();

	void SetActive(bool InEnable);
	void SetAction(bool InEnable);

	USTPolicy* ChangePolicy(EPolicyType eType, const USTState::EnterParam& newPolicyEnterParam);
	ASTCharacter* FindBestTarget();
	ASTCharacter* GetAvatar();
	
	FORCEINLINE bool IsTypeOfPolicy(EPolicyType InType) const { return CurrentPolicy->IsType(InType); }
	FORCEINLINE bool IsAttackablePolicy() { return CurrentPolicy->IsAttackable(); }
	FORCEINLINE bool IsMovablePolicy() { return CurrentPolicy->IsMovable(); }
	FORCEINLINE bool IsHittablePolicy() { return CurrentPolicy->IsHittable(); }
	FORCEINLINE EPolicyType GetCurrentPolicyType() const { return CurrentPolicy->GetType<EPolicyType>(); }

private:
protected:

	UPROPERTY()
	USTPolicy* CurrentPolicy;

	UPROPERTY()
	TMap<EPolicyType, USTPolicy*> AllPolicies;
	
};
