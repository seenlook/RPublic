#include "STTreatComponent.h"
#include "../STCharacter.h"
#include "../../FX/STFXActor.h"
#include "../../PlayerController/STPlayerState.h"
#include "../../PlayerController/STPlayerController.h"
#include "STInteractionComponent.h"

USTTreatComponent::USTTreatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTTreatComponent::BeginPlay()
{
	Super::BeginPlay();
}



void USTTreatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bReviveFlag)
	{
		Tick_Revive(DeltaTime);
	}
}

bool USTTreatComponent::DoRevive(ASTCharacter* InTarget)
{
	if (InTarget == nullptr)
	{
		return false;
	}

	if (InTarget->IsActorDead())
	{
		return false;
	}

	if(InTarget->GetMovementState() != EALSMovementState::Fainting)
	{
		return false;
	}

	if (ASTPlayerState* ps = Cast<ASTPlayerState>(InTarget->GetPlayerState()))
	{
		if (ps->IsTreatedBy)
		{
			return false;
		}
	}

	ASTCharacter* ownerChar = GetOwnerCharacter();
	if (ownerChar == nullptr)
	{
		return false;
	}	

	switch (ownerChar->GetMovementState())
	{
		case EALSMovementState::InAir:	
		case EALSMovementState::Fainting:		
		case EALSMovementState::Vehicle:		
		case EALSMovementState::Death:		
		case EALSMovementState::Climbing:		
		{
			return false;
		}
		default:break;
	}

	return true;
}

void USTTreatComponent::StartRevive(ASTCharacter* InTarget)
{
	if (DoRevive(InTarget) == false)
	{
		return;
	}

	ASTCharacter* ownerChar = GetOwnerCharacter();
	if (ownerChar == nullptr)
	{
		return;
	}

	if (ASTPlayerState* targetPS = Cast<ASTPlayerState>(InTarget->GetPlayerState()))
	{
		int32 playerID = targetPS->GetPlayerId();
		Server_StartRevive(playerID);
	}
}

void USTTreatComponent::StopRevive()
{
	Server_StopRevive();
}

float USTTreatComponent::GetMaxReviveTime()
{
	float baseReciveMaxTime = ReviveMaxTime;

	ASTCharacter* ownerChar = GetOwnerCharacter();

	if (ownerChar == nullptr)
	{
		return baseReciveMaxTime;
	}

	if (ownerChar->StatusEffectComp)
	{
		baseReciveMaxTime -= (baseReciveMaxTime / 100.f) * ownerChar->StatusEffectComp->SaveForceEffects[(int32)EStatusBuffType::TreatSpeedUp].StatusWeaponTypes[(int32)ECharacterWeaponState::All].Value;
	}

	return baseReciveMaxTime;
}

void USTTreatComponent::Server_StartRevive_Implementation(int32 PlayerID)
{
	ASTCharacter* ownerChar = GetOwnerCharacter();

	if (ownerChar == nullptr)
	{
		return;
	}

	RevivePlayerID = PlayerID;

	ASTPlayerState* targetPS = GetReviveTargetPlayerState();

	if (targetPS == nullptr)
	{
		return;
	}		
	//상대 회생상태 On
	targetPS->IsTreatedBy = true;

	//회생 타이머 초기화
	CurrentReviveTime = 0.0f;

	//회생 플래그 On
	bReviveFlag = true;

	//치유 상태로 변경
	ownerChar->Server_SetMovementState(EALSMovementState::ToTreat);

	if (SprayEffect != nullptr)
	{
		SprayEffect->Destroy();
		SprayEffect = nullptr;
	}

	TArray<ASTFXActor*> ReturnArray;
	USTFXUtil::STPlayNiagara(&ReturnArray, GetWorld(), STGameConst::SprayEffectID, FTransform::Identity, ownerChar->GetMesh());
	if (ReturnArray.Num() != 0)
	{
		SprayEffect = ReturnArray[0];
		SprayEffect->SetFloatParam(0, 1.f);
	}

	if (ASTCharacter* targetChar = GetReviveTarget())
	{
		if (targetChar->GetMovementState() == EALSMovementState::Fainting)
		{
			targetChar->StopFaintHandle();
		}
	}
}

void USTTreatComponent::Server_StopRevive_Implementation()
{
	if (ASTCharacter* targetChar = GetReviveTarget())
	{
		if (targetChar->GetMovementState() == EALSMovementState::Fainting)
		{
			targetChar->StartFaintHandle();
		}
	}
	//살리기 캔슬
	CancleRevive();
}

void USTTreatComponent::Client_StopRevive_Implementation()
{
	if (ASTCharacter* ownerChar = GetOwnerCharacter())
	{
		if (ASTPlayerController* ownerPC = ownerChar->GetSSPlayerController())
		{
			if (USTInteractionComponent* interactionComp = ownerPC->GetInteractionComp())
			{
				interactionComp->StopRevive();
			}
		}
	}
}

ASTCharacter* USTTreatComponent::GetOwnerCharacter()
{
	if (OwnerCharacter.Get() == nullptr)
	{
		OwnerCharacter = Cast<ASTCharacter>(GetOwner());
	}

	return OwnerCharacter.Get();
}

void USTTreatComponent::Tick_Revive(float DeltaTime)
{
	CurrentReviveTime += DeltaTime;

	float TimeRate = CurrentReviveTime / GetMaxReviveTime();

	TimeRate = FMath::Clamp(TimeRate, 0.0f, 1.0f);

	//회생 게이지 증가
	if (ASTCharacter* ownerChar = GetOwnerCharacter())
	{
		ownerChar->ReviveValue = 100 * TimeRate;
	}

	bool bCancel = false;
	if (ASTCharacter* targetChar = GetReviveTarget())
	{	
		bCancel = targetChar->IsActorDead() || (targetChar->GetMovementState() == EALSMovementState::Death);
	}
	else
	{
		bCancel = true;
	}

	if (bCancel)
	{
		CancleRevive();
		return;
	}

	if (TimeRate >= 1.0f)
	{
		FinishRevive();
	}
}

ASTCharacter* USTTreatComponent::GetReviveTarget()
{	
	ASTPlayerState* targetPS = GetReviveTargetPlayerState();
	if (targetPS == nullptr)
	{
		return nullptr;
	}

	if (ASTCharacter* targetChar = Cast<ASTCharacter>(targetPS->GetPawn()))
	{
		return targetChar;
	}

	return nullptr;
}

ASTPlayerState* USTTreatComponent::GetReviveTargetPlayerState()
{
	ASTCharacter* ownerChar = GetOwnerCharacter();
	if (ownerChar == nullptr)
	{
		return nullptr;
	}

	ASTPlayerState* targetPS = nullptr;
	if (AGameStateBase* gameState = UGameplayStatics::GetGameState(ownerChar->GetWorld()))
	{
		for (auto playerState : gameState->PlayerArray)
		{
			if (ASTPlayerState* ps = Cast<ASTPlayerState>(playerState))
			{
				if (ps->GetPlayerId() == RevivePlayerID)
				{
					targetPS = ps;
					break;
				}
			}
		}
	}

	return targetPS;
}

void USTTreatComponent::FinishRevive()
{
	//Called Server
	
	//일반 상태로 변경
	if (ASTCharacter* ownerCharacterRef = GetOwnerCharacter())
	{
		ownerCharacterRef->Server_SetMovementState(EALSMovementState::Grounded);
	}

	//상대 기절상태 해제
	if (ASTPlayerState* targetPS = GetReviveTargetPlayerState())
	{
		targetPS->IsFainting = false;
	}

	//상대 상태 원복 및 체력 15%회복
	if (ASTCharacter* targetChar = GetReviveTarget())
	{
		targetChar->HP = targetChar->GetMaxHp() * 0.15f;
		targetChar->HP_Faint = targetChar->MaxHP_Faint;
		targetChar->Server_SetMovementState(EALSMovementState::Grounded, true);
	}

	//상태 초기화
	ResetReviveState();
}

void USTTreatComponent::CancleRevive()
{
	//일반 상태로 변경 및 회생 게이지 초기화
	if (ASTCharacter* ownerCharacterRef = GetOwnerCharacter())
	{
		EALSMovementState CurrentState = ownerCharacterRef->GetMovementState();
		if(CurrentState != EALSMovementState::Fainting && CurrentState != EALSMovementState::Death)
		{
			ownerCharacterRef->Server_SetMovementState(EALSMovementState::Grounded);
		}
	}

	//상태 초기화
	ResetReviveState();
}

void USTTreatComponent::ResetReviveState()
{	
	bReviveFlag = false;

	if (ASTPlayerState* targetPS = GetReviveTargetPlayerState())
	{
		//상대 회생상태 Off
		targetPS->IsTreatedBy = false;
	}

	//회생 타이머 초기화
	CurrentReviveTime = 0.0f;

	//RevivePlayer ID 초기화
	RevivePlayerID = -1;

	if (SprayEffect != nullptr)
	{
		SprayEffect->Destroy();
		SprayEffect = nullptr;
	}

	//회생 종료 클라메시지 인터렉션 UI 갱신
	Client_StopRevive();
}

