// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STAbilityStatComponent.generated.h"

class USTCharacterInfo;

/*
* @class : USTAbilityStatComponent
* @brief : 캐릭터 능력치 를 관리한다. 
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTR_API USTAbilityStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTAbilityStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE FString GetCharName() const { return CharName; }
	FORCEINLINE void SetCharName(const FString& name) { CharName = name; }
	FORCEINLINE int64 GetHp() const { return Hp; }
	FORCEINLINE int64 GetMaxHp() const { return MaxHp; }
	FORCEINLINE int64 GetSp() const { return Sp; }
	FORCEINLINE int64 GetMaxSp() const { return MaxSp; }

	class ASTCharacter* GetOwnerChar();
	void Initialize(USTCharacterInfo* InCharInfo);

private:
	UPROPERTY(EditInstanceOnly)
	FString CharName = TEXT("NONE");
	
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_Hp)
	int64 Hp = 0; 
	UPROPERTY(EditInstanceOnly)
	int64 BasicMaxHp = 0;
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_MaxHp)
	int64 MaxHp = 0;
	
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_Sp)
	int64 Sp = 0;
	UPROPERTY(EditInstanceOnly)
	int64 BasicMaxSp = 0;
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_MaxSp)
	int64 MaxSp = 0;
	
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_MoveSpeed)
	int32 MoveSpeed = 00;
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_Level)
	int32 Level = 0;

	UFUNCTION()
	void OnRep_Level();
	UFUNCTION()
	void OnRep_Hp();
	UFUNCTION()
	void OnRep_MaxHp();
	UFUNCTION()
	void OnRep_Sp();
	UFUNCTION()
	void OnRep_MaxSp();
	UFUNCTION()
	void OnRep_MoveSpeed();
};
