// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ModularCharacter.h"
#include "GameFramework/Character.h"

#include "Game/STGameType.h"
#include "DataTable/STResourceTypes.h"

#include "STCharacter.generated.h"

class AActor;
class AController;
class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UAbilitySystemComponent;
class UInputComponent;
class USTCameraComponent;
class USpringArmComponent;
class UObject;
struct FFrame;
struct FGameplayTag;
struct FGameplayTagContainer;
class ASTPlayerState;
class USTAbilitySystemComponent;
class USTPawnExtensionComponent;
class USTAbilityStatComponent;
class USTCharacterInfo;

/**
 * FSTReplicatedAcceleration: Compressed representation of acceleration 
 * 가속도의 압축 표현입니다.
 */
USTRUCT()
struct FSTReplicatedAcceleration
{
	GENERATED_BODY()

	// - XY 가속도 구성 요소의 방향, [0, 2 * pi]를 나타내도록 양자화됩니다.
	UPROPERTY()
	uint8 AccelXYRadians = 0;	// Direction of XY accel component, quantized to represent [0, 2*pi] 
	
	// -XY 구성 요소의 가속도율, [0, MaxAcceleration]을 나타내도록 양자화됩니다.
	UPROPERTY()
	uint8 AccelXYMagnitude = 0;	//Accel rate of XY component, quantized to represent [0, MaxAcceleration] 

	// -Raw Z 가속도율 구성 요소, [-MaxAcceleration, MaxAcceleration]을 나타내도록 양자화됩니다.
	UPROPERTY()
	int8 AccelZ = 0;	// Raw Z accel rate component, quantized to represent [-MaxAcceleration, MaxAcceleration] 
};

/** The type we use to send FastShared movement updates. */   
// FastShared 이동 업데이트를 보내는 데 사용하는 유형입니다.
USTRUCT()
struct FSharedRepMovement
{
	GENERATED_BODY()

	FSharedRepMovement();

	bool FillForCharacter(ACharacter* Character);
	bool Equals(const FSharedRepMovement& Other, ACharacter* Character) const;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	UPROPERTY(Transient)
	FRepMovement RepMovement;

	UPROPERTY(Transient)
	float RepTimeStamp = 0.0f;

	UPROPERTY(Transient)
	uint8 RepMovementMode = 0;

	UPROPERTY(Transient)
	bool bProxyIsJumpForceApplied = false;

	UPROPERTY(Transient)
	bool bIsCrouched = false;
};

template<>
struct TStructOpsTypeTraits<FSharedRepMovement> : public TStructOpsTypeTraitsBase2<FSharedRepMovement>
{
	enum
	{
		WithNetSerializer = true,
		WithNetSharedSerialization = true,
	};
};


/**
* @class : ASTCharacter
* @brief :  1. 캐릭터 애니메이션 관리
*			2. 캐릭터 기본능력 및  스킬 보유관리
*			3. AI Controller
*/

UCLASS(Blueprintable, Config = Game, meta = (ShortTooltip = "STGame base character"))
class ASTCharacter : public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ASTCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Spawn default controller for this Pawn, and get possessed by it. */
	virtual void SpawnDefaultController() override;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	//~End of AActor interface

	// ~ IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ IAbilitySystemInterface
	// 
	// ~ IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	// ~ IGameplayTagAssetInterface

	void DisableMovementAndCollision();
	void UninitAndDestroy();

	// 스폰되면서 데이터에 의한 초기화를 한다. 
	void Initialize(USTCharacterInfo* InCharInfo, const FVector& location, const FRotator& rotation);
	// 입력에 의한 이동 정책을 확인한다. 
	bool IsManualMovablePolicy();
	bool IsCanBeDamaged(bool InOnlyCanbeDamaged = false);
	bool IsCanBeTargeted();
		
	/**  CameraBoom **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/**  Camera 서브오젝트**/
	FORCEINLINE class USTCameraComponent* GetSTCameraComponent() const { return STCameraComponent; }	

	// 캐릭터 능력치 스탯을 담은 정보
	FORCEINLINE class USTAbilityStatComponent* GetAbilityStatComponent() { return AbilityStatComponent; }

	UFUNCTION(BlueprintCallable, Category = "STGame|Character")
	USTAbilitySystemComponent* GetSTAbilitySystemComponent() const;

	// hand attach , slot equip , attack
	virtual void ToggleWeaponEquip();	
	virtual void WeaponEquip();	
	virtual void WeaponUnEquip();	
	virtual void Attack();

	void SetCurrentAIPolicyType(EPolicyType InType);
	void SetActive(bool bEnable);
	void SetAction(bool bEnable);

	FORCEINLINE FName GetUUID() const { return TEXT("NONE"); }
	FORCEINLINE uint32 GetCharID() const { return 0; }
	FORCEINLINE ECharType GetCharType() const { return CharType; }	
	FORCEINLINE ECharacterClass GetCharClassType() { return CharTable->Class; }

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	UFUNCTION()
	void OnRep_ReplicatedAcceleration();

	UFUNCTION()
	void OnRep_IsEquippedWeapon();

	UFUNCTION()
	void OnRep_CurrentPolicyType(const EPolicyType& InPreType);	

	UFUNCTION()
	void OnRep_Active();

	UFUNCTION()
	void OnRep_Action();

private:
protected:
	
	/** 카메라 붐 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "STGame|Character", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "STGame|Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTCameraComponent> STCameraComponent;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "STGame|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTPawnExtensionComponent> PawnExtComponent;

	/** 캐릭터 능력치 스탯 */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "STGame|Character", Meta = (AllowPrivateAccess = "true"))
	USTAbilityStatComponent* AbilityStatComponent;

	// 클라이언트 로컬 AI상태를 알 수 있도록 리플리케이트 시킨다. 
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentPolicyType)
	EPolicyType CurrentAIPolicyType = EPolicyType::PCT_Idle;

	UPROPERTY(ReplicatedUsing = OnRep_Active)
	bool bReplicatedActive = true;
	
	UPROPERTY(ReplicatedUsing = OnRep_Action)
	bool bReplicatedAction = true;

	// 캐릭터 테이블
	const FResCharacter* CharTable=nullptr;
	// 캐릭터 타입
	ECharType CharType = ECharType::None;
	// 초기화 확인 변수
	bool bInitialized = false;
	// 무기 장착 여부
	int8 bIsEquippedWeapon = 0;

private:

	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
	FSTReplicatedAcceleration ReplicatedAcceleration;
		
};

