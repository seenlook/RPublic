// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "STPlayerState.generated.h"

class AController;
class ASTPlayerController;
class USTPawnData;
class APlayerState;
class FName;
class UObject;
struct FFrame;
struct FGameplayTag;
class USTExperienceDefinition;
class USTAbilitySystemComponent;

/**
 * @class : ASTPlayerState
 * @brief : player 상태값을 저장하고 전달한다.
 */
UCLASS()
class PROJECTR_API ASTPlayerState : public AModularPlayerState , public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ASTPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;	

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnDeactivated() override;
	virtual void OnReactivated() override;
	//~End of APlayerState interface

	// 플레이어의 리플리케이트된 뷰 회전을 가져옵니다. 관전에 사용된다
	FRotator GetReplicatedViewRotation() const;

	// 리플리케이트 된 뷰 회전을 설정합니다. 서버에서만 유효 한다.
	void SetReplicatedViewRotation(const FRotator& NewRotation);
	void OnExperienceLoaded(const USTExperienceDefinition* CurrentExperience);

	static const FName NAME_STAbilityReady;

	UFUNCTION(BlueprintCallable, Category = "STGame|PlayerState")
	ASTPlayerController* GetMyPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "STGame|PlayerState")
	USTAbilitySystemComponent* GetSTAbilitySystemComponent() const { return STAbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const USTPawnData* InPawnData);

	UFUNCTION()
	void SetIsDeath(bool bDeath);

	UFUNCTION()
	void SetIsFainting(bool bFainting);

protected:
	UFUNCTION()
	void OnRep_PawnData();

	UFUNCTION()
	void OnRepKillCount();

	UFUNCTION()
	void OnRepDeathCount();	

protected:

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const USTPawnData> PawnData;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRepKillCount)
	int32 KillCount = 0;

	UPROPERTY(BlueprintReadWrite, Replicated, ReplicatedUsing = OnRepDeathCount)
	int32 DeathCount = 0;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 MainWeaponID;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 SubWeaponID;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 FaintCount = 0;

	UPROPERTY(BlueprintReadWrite, Replicated)
	uint8 IsFainting : 1;
	UPROPERTY(BlueprintReadWrite, Replicated)
	uint8 IsDeath : 1;

	UPROPERTY(BlueprintReadWrite, Replicated)
	uint8 IsToTreat : 1;		// 내가 아군을 회생(치료) 하고 있는 중이다. 

	UPROPERTY(BlueprintReadWrite, Replicated)
	uint8 IsTreatedBy : 1;		// 자신이 회생(치료) 받고 있는 중이다. 

private:

	// 플레이어 캐릭터가 사용하는 능력 시스템 구성 요소 sub-object 입니다.
	UPROPERTY(VisibleAnywhere, Category = "STGame|PlayerState")
	TObjectPtr<USTAbilitySystemComponent> STAbilitySystemComponent;

	UPROPERTY(Replicated)
	FRotator ReplicatedViewRotation;

#pragma region Armor 
protected:
	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 MaxArmor = 0;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 CurrentArmor = 0;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 ArmorGrade = 0;

public:
	UFUNCTION()
	int32 GetCurrentArmor() { return CurrentArmor; };

	UFUNCTION()
	int32 GetMaxArmor() { return MaxArmor; };

	UFUNCTION()
	int32 GetArmorGrade() { return ArmorGrade; };

	UFUNCTION()
	void SetCurrentArmor(int32 InArmor);

	UFUNCTION()
	void SetMaxArmor(int32 InArmor);

	UFUNCTION()
	void SetArmorGrade(int32 InArmorGrade);

#pragma endregion Armor
	
};
