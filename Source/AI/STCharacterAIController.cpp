// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STCharacterAIController.h"

#include "Character/STCharacter.h"
#include "Game/STGameType.h"
#include "Common/STHelper.h"
#include "Common/STSeverHelper.h"

#include "STPolicy.h"
#include "STPolicyActive.h"
#include "STPolicyDeActive.h"
#include "STPolicyInActive.h"
#include "STPolicyDead.h"
#include "STPolicyIdle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STCharacterAIController)

DEFINE_LOG_CATEGORY(STCharacterAIControllerLogCategory);

ASTCharacterAIController::ASTCharacterAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void ASTCharacterAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	SetAIPolicies();	
}

void ASTCharacterAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ASTCharacterAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

	InitPolicyState();
    
    UE_LOG(STCharacterAIControllerLogCategory, Log, TEXT("ASTCharacterAIController::OnPossess: %s"), *(InPawn->GetName()));
}

void ASTCharacterAIController::OnUnPossess()
{
    Super::OnUnPossess();

    UE_LOG(STCharacterAIControllerLogCategory, Log, TEXT("ASTCharacterAIController::OnUnpossess"));
}

void ASTCharacterAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	ensure(CurrentPolicy);
	CurrentPolicy->Tick(DeltaSeconds);	
}

void ASTCharacterAIController::SetAIPolicies()
{
	AllPolicies.Empty();
	// AI 상태 맵핑
	// 클라이언트 실행했을때의 캐릭터 상태 - 미리 로드 시켜놓거나 , 
	AddPoliciesToMap(EPolicyType::PCT_InActive, USTPolicyInActive::StaticClass());
	AddPoliciesToMap(EPolicyType::PCT_Active, USTPolicyActive::StaticClass());
	AddPoliciesToMap(EPolicyType::PCT_DeActive, USTPolicyDeActive::StaticClass());
	// 게임 상에서 캐릭터의 상태
	AddPoliciesToMap(EPolicyType::PCT_Idle, USTPolicyIdle::StaticClass());
	AddPoliciesToMap(EPolicyType::PCT_Dead, USTPolicyDead::StaticClass());	
}

void ASTCharacterAIController::AddPoliciesToMap(EPolicyType eType, TSubclassOf<USTPolicy> InPolicyClass)
{
	USTPolicy* policy = NewObject<USTPolicy>(this, InPolicyClass);
	checkf(policy, TEXT("AddPoliciesToMap - NewObject failed ") );
	policy->Initialize(this);
	AllPolicies.Emplace(eType, policy);
}

USTPolicy* ASTCharacterAIController::ChangePolicy(EPolicyType eType, const USTState::EnterParam& newPolicyEnterParam)
{
	USTPolicy* nextPolicy = AllPolicies.FindRef(eType);
	if (nullptr == nextPolicy)
	{	
		return CurrentPolicy;
	}

	USTPolicy* leavePolicy = CurrentPolicy;
	if (nullptr!= leavePolicy)
	{
		leavePolicy->Leave(nextPolicy);
	}
	
	GetAvatar()->SetCurrentAIPolicyType(nextPolicy->GetPolicyType());
	nextPolicy->Enter(newPolicyEnterParam, leavePolicy);
	
	CurrentPolicy = nextPolicy;
	return leavePolicy;
}

void ASTCharacterAIController::InitPolicyState()
{
	CurrentPolicy = AllPolicies[EPolicyType::PCT_Idle];
	StartIdle();
}

bool ASTCharacterAIController::StartIdle()
{
	ChangePolicy(EPolicyType::PCT_Idle, USTState::EnterParam());
	return true;
}

bool ASTCharacterAIController::StartInActive()
{
	ChangePolicy(EPolicyType::PCT_InActive, USTState::EnterParam());
	return true;
}

bool ASTCharacterAIController::StartActive()
{
	ChangePolicy(EPolicyType::PCT_Active, USTState::EnterParam());
	return true;
}

bool ASTCharacterAIController::StartDeActive()
{
	ChangePolicy(EPolicyType::PCT_DeActive, USTState::EnterParam());
	return true;
}

ASTCharacter* ASTCharacterAIController::FindBestTarget()
{
	//TODO : 타겟을 찾는 코드 추가 하기 
	ASTCharacter* target = nullptr;
	return target;
}

void ASTCharacterAIController::SetActive(bool InEnable)
{
	STHelper::SetActorTickActivate(this, InEnable);
	GetAvatar()->SetActive(InEnable);
}

void ASTCharacterAIController::SetAction(bool InEnable)
{
	STHelper::SetActorTickActivate(this, InEnable);
	GetAvatar()->SetAction(InEnable);
}

ASTCharacter* ASTCharacterAIController::GetAvatar()
{
	return Cast<ASTCharacter>(GetPawn());
}	
