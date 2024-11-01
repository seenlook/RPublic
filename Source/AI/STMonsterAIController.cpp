// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STMonsterAIController.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/STCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STMonsterAIController)

ASTMonsterAIController::ASTMonsterAIController(const FObjectInitializer& ObjectInitializer)  
	: Super(ObjectInitializer)
{
	// AIController가 사용할 BlackboardComponent를 생성합니다.
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	// AIController가 사용할 BehaviorTreeComponent를 생성합니다.
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	// BehaviorTreeComponent가 사용할 BehaviorTree를 설정합니다.
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/ST/Character/AI/BT_Monster.BT_Monster'"));
	if (BTObject.Succeeded())
	{
		BehaviorTree = BTObject.Object;
	}

	// AIController가 사용할 BlackboardComponent와 BehaviorTreeComponent를 초기화합니다.
	if (BehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BehaviorTree is not loaded"));
	}

}	

void ASTMonsterAIController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASTMonsterAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASTMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ASTMonsterAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASTMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASTMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// AIController가 사용할 BlackboardComponent와 BehaviorTreeComponent를 초기화합니다.
	BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	BehaviorTreeComponent->StartTree(*BehaviorTree);
}

void ASTMonsterAIController::OnUnPossess()
{
	Super::OnUnPossess();

	// BehaviorTreeComponent를 중단합니다.
	BehaviorTreeComponent->StopTree();
}

