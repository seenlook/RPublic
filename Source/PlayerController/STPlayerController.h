// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "CommonPlayerController.h"
#include "STPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class AHUD;
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class ASTCharacter;
struct FTimerHandle;

/**
* @class : ASTPlayerController
* @brief : 플레이어 입출력 - 구체적으로 '입력'과 'UI 액션'- 를 담당한다
*/

UCLASS(Config = Game, Meta = (ShortTooltip = "STGame Base PlayerController"))
class ASTPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:
	ASTPlayerController();

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	//~End of AActor interface

	//~AController interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	virtual void SetPawn(APawn* InPawn) override;

	void BroadcastOnPlayerStateChanged();
	//~End of AController interface

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface
	virtual void SetupInputComponent() override;
	// 컨트롤 캐릭터 설정
	void SetAvatar(ASTCharacter* InAvatar);
	bool IsAvatar(ASTCharacter* InAvatar);

	UFUNCTION(BlueprintCallable, Category = "STGame|PlayerController")
	ASTPlayerState* GetSTPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "STGame|PlayerController")
	USTAbilitySystemComponent* GetSTAbilitySystemComponent() const;	

	UFUNCTION(BlueprintCallable, Category = "STGame|PlayerController")
	AHUD* GetGameHUD() const;	

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "STGame|PlayerController")
	void Server_Death(bool bFainting = false);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "STGame|PlayerController")
	void Client_Death();

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "STGame|PlayerController")
	void Client_Fainting(ASTCharacter* Victim = nullptr, const USTDamageType* STDamageType = nullptr);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "STGame|PlayerController")
	void Server_Fainting(ASTCharacter* InVictim, const USTDamageType* InDamageType);
	void Server_Fainting_Implementation(ASTCharacter* InVictim, const USTDamageType* InDamageType);
	bool Server_Fainting_Validate(ASTCharacter* InVictim, const USTDamageType* InDamageType);

protected:
	/** 액션에 대한 입력 핸들러 */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnTouchTriggered();
	void OnTouchReleased();
	void OnJump();
	void ProcessInput();
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	void SetNewMoveDestination(const FVector DestLocation);			

protected:
	// 짧게 눌렀는지 알 수 있는 시간 임계값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** 클릭할 때 생성할 FX 클래스입니다. */ 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	// 매핑 컨텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	// 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	// 점프 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	/** 제어되는 캐릭터가 마우스 커서로 이동해야하는 경우 true. */
	uint32 bMoveToMouseCursor : 1;	
	
	// 캐릭터가 컨트롤 하고 있는 케릭터
	TWeakObjectPtr<ASTCharacter> Avatar;

	FVector2D MoveInput;	

private:
	FVector CachedDestination;
	// 터치 장치인지 아닌지
	bool bIsTouch; 
	// 얼마나 오래 눌렀는지
	float FollowTime;
	// 점프 여부
	bool bIsJump = false;
	// 무기 착용여부
	bool bIsEquip = false;	
};


