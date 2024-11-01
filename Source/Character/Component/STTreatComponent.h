#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STTreatComponent.generated.h"

class ASTCharacter;
class ASTPlayerState;
class ASTFXActor;

/*
* @class : USTTreatComponent
* @brief : 캐릭터를 치료하는 컴포넌트
*/


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTR_API USTTreatComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	USTTreatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION()
	bool DoRevive(ASTCharacter* Target);

	UFUNCTION()
	void StartRevive(ASTCharacter* Target);

	UFUNCTION()
	void StopRevive();

	UFUNCTION()
	float GetMaxReviveTime();

protected:
	UFUNCTION(Server, Reliable)
	void Server_StartRevive(int32 PlayerID);
	void Server_StartRevive_Implementation(int32 PlayerID);

	UFUNCTION(Server, Reliable)
	void Server_StopRevive();
	void Server_StopRevive_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_StopRevive();
	void Client_StopRevive_Implementation();

private:
	UFUNCTION()
	ASTCharacter* GetOwnerCharacter();

	UFUNCTION()
	void Tick_Revive(float DeltaTime);

	UFUNCTION()
	ASTCharacter* GetReviveTarget();

	UFUNCTION()
	ASTPlayerState* GetReviveTargetPlayerState();

	UFUNCTION()
	void FinishRevive();

	UFUNCTION()
	void CancleRevive();

	UFUNCTION()
	void ResetReviveState();

private:
	UPROPERTY()
	TWeakObjectPtr<ASTCharacter> OwnerCharacter = nullptr;

	UPROPERTY()
	ASTFXActor* SprayEffect = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float ReviveMaxTime = 5.0f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentReviveTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bReviveFlag = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 RevivePlayerID = -1;
	
		
};
