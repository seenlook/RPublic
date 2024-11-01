// Fill out your copyright notice in the Description page of Project Settings.


#include "STAbilityStatComponent.h"
#include <Net/UnrealNetwork.h>

#include "STCharacter.h"
#include "Infos/STCharacterInfo.h"

// Sets default values for this component's properties
USTAbilityStatComponent::USTAbilityStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	SetNetAddressable();
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void USTAbilityStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

/// <summary>
/// Scene이 설정된 후, CreateRenderState_Concurrent 또는 OnCreatePhysicsState가 호출되기 전에 구성 요소가 등록될 때 호출됩니다.
/// </summary>
void USTAbilityStatComponent::OnRegister()
{
	Super::OnRegister();
}

/// <summary>
/// 리플리케이트 등록 
/// </summary>
/// <param name="OutLifetimeProps"></param>
void USTAbilityStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USTAbilityStatComponent, Hp);
	DOREPLIFETIME(USTAbilityStatComponent, MaxHp);
	DOREPLIFETIME(USTAbilityStatComponent, Sp);
	DOREPLIFETIME(USTAbilityStatComponent, MaxSp);
	DOREPLIFETIME(USTAbilityStatComponent, MoveSpeed);
	DOREPLIFETIME(USTAbilityStatComponent, Level);	
}

/// <summary>
/// 캐릭터별 능력치 초기화 하겠다. 
/// </summary>
void USTAbilityStatComponent::Initialize(USTCharacterInfo* InCharInfo)
{
	ensureMsgf(InCharInfo != nullptr, TEXT("USTAbilityStatComponent - Initialize InCharInfo "));
	if (nullptr!=InCharInfo)
	{
		this->Hp	= InCharInfo->GetHp();
		this->MaxHp = InCharInfo->GetMaxHp();
		this->Sp = InCharInfo->GetSp();
		this->MaxSp = InCharInfo->GetMaxSp();
		this->MoveSpeed = InCharInfo->GetMoveSpeed();
		this->Level = InCharInfo->GetLevel();
	}
}

/// <summary>
/// 캐릭터 가져오기
/// </summary>
/// <returns></returns>
ASTCharacter* USTAbilityStatComponent::GetOwnerChar()
{
	ASTCharacter* ownerChar = Cast<ASTCharacter>(GetOwner());
	ensureMsgf(ownerChar != nullptr, TEXT("USTAbilityStatComponent - GetOwner() isn't registered in ASTCharacter"));
	return ownerChar;
}

/// <summary>
/// 레벨 변경
/// </summary>
void USTAbilityStatComponent::OnRep_Level()
{
	if (GetNetMode() == NM_Client)
	{		
	}
	//ChangeHPBar();
}

/// <summary>
/// HP 변경
/// </summary>
void USTAbilityStatComponent::OnRep_Hp()
{
	if (nullptr!=GetOwnerChar())
	{	
		//GetOwnerChar()->OnUpdateHPUI();
	}
}

/// <summary>
/// maxhp 변경됨
/// </summary>
void USTAbilityStatComponent::OnRep_MaxHp()
{
	//ChangeHPBar();
}

/// <summary>
/// HP 변경
/// </summary>
void USTAbilityStatComponent::OnRep_Sp()
{
	if (nullptr != GetOwnerChar())
	{
		//GetOwnerChar()->OnUpdateHPUI();
	}
}

/// <summary>
/// maxhp 변경됨
/// </summary>
void USTAbilityStatComponent::OnRep_MaxSp()
{
	//ChangeHPBar();
}

/// <summary>
/// 스피드 변경
/// </summary>
void USTAbilityStatComponent::OnRep_MoveSpeed()
{
	
}