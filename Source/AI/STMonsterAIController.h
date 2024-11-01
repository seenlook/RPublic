// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/STCharacterAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"

#include "STMonsterAIController.generated.h"

/**
 * @class : ASTMonsterAIController
 * @brief : 몬스터 전용 AiController
 */
UCLASS()
class PROJECTR_API ASTMonsterAIController : public ASTCharacterAIController
{
	GENERATED_BODY()
public:
	ASTMonsterAIController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY()
	UBehaviorTree* BehaviorTree;

	UPROPERTY()
	UBlackboardData* BlackboardData;

};
