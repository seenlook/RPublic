// Copyright Epic Games, Inc. All Rights Reserved.

#include "STCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"

#include "AI/STCharacterAIController.h"
#include "STPawnExtensionComponent.h"
#include "AbilitySystem/STAbilitySystemComponent.h"
#include "STCharacterMovementComponent.h"
#include "Infos/STGameplayTags.h"
#include "Infos/STLogChannels.h"
#include "Camera/STCameraComponent.h"
#include "PlayerController/STPlayerController.h"
#include "STAbilityStatComponent.h"
#include "DataTable/STDataTableManager.h"
#include "Infos/STCharacterInfo.h"
#include "Common/STHelper.h"
#include "Common/STSeverHelper.h"


#define DEFAULT_AICONTROLLER_PATH TEXT("/Script/Engine.Blueprint'/Game/Resources/Blueprints/AIController/BP_STAIController.BP_STAIController_C'")
#define MONSTER_AICONTROLLER_PATH TEXT("/Script/Engine.Blueprint'/Game/Resources/Blueprints/AIController/BP_STMonsterAIController.BP_STMonsterAIController_C'")

ASTCharacter::ASTCharacter(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	// true인 경우 이 Pawn의 Pitch,Yaw,Roll 는 PlayerController에 의해 제어되는 경우 컨트롤러의 ControlRotation 요와 일치하도록 업데이트됩니다.
	// 디폴트 false 일때 WASD 방향 으로 캐릭터가 회전한다.  true 인경우 회전하지 않음.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	CameraBoom->bUsePawnControlRotation = true;

	// Create a camera...
	STCameraComponent = CreateDefaultSubobject<USTCameraComponent>(TEXT("STCamera"));
	STCameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	STCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	STCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//
	PawnExtComponent = CreateDefaultSubobject<USTPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	AbilityStatComponent = CreateDefaultSubobject<USTAbilityStatComponent>(TEXT("USTAbilityStatComponent"));

	AIControllerClass = ASTCharacterAIController::StaticClass();

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASTCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASTCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASTCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASTCharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void ASTCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);

	DOREPLIFETIME(ThisClass, bReplicatedActive);
	DOREPLIFETIME(ThisClass, bReplicatedAction);
}

void ASTCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		ReplicatedAcceleration.AccelXYRadians = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);     // [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);   // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void ASTCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	USTCharacterMovementComponent* MoveComp = CastChecked<USTCharacterMovementComponent>(GetCharacterMovement());
	MoveComp->StopMovementImmediately();
	MoveComp->DisableMovement();
}

void ASTCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor) 
	if (USTAbilitySystemComponent* MyASC = GetSTAbilitySystemComponent())
	{
		if (MyASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}
	SetActorHiddenInGame(true);
}

void ASTCharacter::OnRep_CurrentPolicyType(const EPolicyType& InPreType)
{
	ASTPlayerController* pc = Cast<ASTPlayerController>(GetWorld()->GetFirstPlayerController());

	if (IsNetMode(NM_DedicatedServer))
	{
		return;
	}
	
	if (nullptr!=pc && pc->IsAvatar(this))
	{
		// 이전 상태 처리 값 삭제등 처리
		switch (InPreType)
		{
		case EPolicyType::PCT_MoveTo:			
			break;		
		}

		// 새로운 진입 상태 처리
		switch (CurrentAIPolicyType)
		{
		case EPolicyType::PCT_Dead:			
			break;
		case EPolicyType::PCT_MoveTo:			
			break;
		}
	}
}

void ASTCharacter::SetCurrentAIPolicyType(EPolicyType InType)
{
	if (CurrentAIPolicyType == InType)
	{
		return;
	}

	EPolicyType preAIPolicyType = CurrentAIPolicyType;
	CurrentAIPolicyType = InType;

	OnRep_CurrentPolicyType(preAIPolicyType);
}

bool ASTCharacter::IsManualMovablePolicy()
{
	bool bResult = true;
	switch (CurrentAIPolicyType)
	{
	case EPolicyType::PCT_Spawn:	bResult = false; break;
	case EPolicyType::PCT_Dead:		bResult = false; break;
	case EPolicyType::PCT_Disabled: bResult = false; break;
	default:
		break;
	}
	
	return bResult;
}

bool ASTCharacter::IsCanBeDamaged(bool InOnlyCanbeDamaged)
{
	//
	return true;
}

bool ASTCharacter::IsCanBeTargeted()
{
	//
	return true;
}

void ASTCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ASTCharacter::DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
}

void ASTCharacter::SpawnDefaultController()
{
	FString aiControllerPath = DEFAULT_AICONTROLLER_PATH;
	if (!aiControllerPath.IsEmpty())
	{
		UClass* const newClass = LoadObject<UClass>(NULL, *aiControllerPath);
		if (newClass && newClass->IsChildOf(ASTCharacterAIController::StaticClass()))
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = this->GetInstigator();
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.OverrideLevel = GetLevel();
			SpawnInfo.ObjectFlags |= RF_Transient;	
			AController* NewController = GetWorld()->SpawnActor<AController>(newClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
			if (nullptr!=NewController)
			{
				NewController->Possess(this);
			}
			return;
		}
		ensureMsgf(false, TEXT("Failed SpawnDefaultController, AIControllerClass(%s)"), *aiControllerPath);
		
	}
	
	Super::SpawnDefaultController();
}

void ASTCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();
}

void ASTCharacter::UnPossessed()
{
	AController* const OldController = Controller;

	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();
}

void ASTCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void ASTCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void ASTCharacter::OnRep_ReplicatedAcceleration()
{
	if (USTCharacterMovementComponent* MovementComponent = Cast<USTCharacterMovementComponent>(GetCharacterMovement()))
	{
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const double AccelXYRadians = (ReplicatedAcceleration.AccelXYRadians / 255.0) * TWO_PI;	// [0, 255] -> [0, 2PI]
		const double AccelXYMagnitude = (ReplicatedAcceleration.AccelXYMagnitude / 255.0) * MaxAccel;	// [0, 255] -> [0, MaxAccel]
		const double AccelZ = (ReplicatedAcceleration.AccelZ / 127.0) * MaxAccel;	// [-127, 127] -> [-MaxAccel, MaxAccel]

		FVector NewAccel;
		FMath::PolarToCartesian(AccelXYRadians, AccelXYMagnitude, NewAccel.X, NewAccel.Y);
		NewAccel.Z = AccelZ;

		MovementComponent->AddInputVector(NewAccel);
	}
}

void ASTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void ASTCharacter::InitializeGameplayTags()
{
	
}

void ASTCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
}

void ASTCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void ASTCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
}

USTAbilitySystemComponent* ASTCharacter::GetSTAbilitySystemComponent() const
{
	return Cast<USTAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ASTCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetSTAbilitySystemComponent();
}

void ASTCharacter::OnAbilitySystemInitialized()
{
	USTAbilitySystemComponent* STASC = GetSTAbilitySystemComponent();
	check(STASC);

	InitializeGameplayTags();
}

void ASTCharacter::OnAbilitySystemUninitialized()
{
}

void ASTCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const USTAbilitySystemComponent* STASC = GetSTAbilitySystemComponent())
	{
		STASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ASTCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const USTAbilitySystemComponent* STASC = GetSTAbilitySystemComponent())
	{
		return STASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ASTCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const USTAbilitySystemComponent* STASC = GetSTAbilitySystemComponent())
	{
		return STASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ASTCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const USTAbilitySystemComponent* STASC = GetSTAbilitySystemComponent())
	{
		return STASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void ASTCharacter::ToggleWeaponEquip()
{
	UE_LOG(LogSTGame, Verbose, TEXT("[ASTCharacter::ToggleWeaponEquip] Start === "));
	if (bIsEquippedWeapon)
	{
		WeaponUnEquip();
	}
	else
	{
		WeaponEquip();
	}
	UE_LOG(LogSTGame, Verbose, TEXT("[ASTCharacter::ToggleWeaponEquip] End === "));
}

void ASTCharacter::WeaponEquip()
{
	UE_LOG(LogSTGame, Verbose, TEXT("[ASTCharacter::WeaponEquip] Start === "));
	bIsEquippedWeapon= true;
	UE_LOG(LogSTGame, Verbose, TEXT("[ASTCharacter::WeaponEquip] End === "));
}

void ASTCharacter::WeaponUnEquip()
{
	UE_LOG(LogSTGame, Verbose, TEXT("[ASTCharacter::WeaponUnEquip] Start === "));
	bIsEquippedWeapon = false;
	UE_LOG(LogSTGame, Verbose, TEXT("[ASTCharacter::WeaponUnEquip] End === "));
}

void ASTCharacter::OnRep_IsEquippedWeapon()
{
	if (bIsEquippedWeapon)
	{
		WeaponEquip();
	}
	else
	{
		WeaponUnEquip();
	}
}

void ASTCharacter::Attack()
{
	UE_LOG(LogSTGame, Verbose, TEXT("[ASTCharacter::Attack] Start === "));

	if (USTAbilitySystemComponent* MyASC = GetSTAbilitySystemComponent())
	{
		if (MyASC->GetAvatarActor() == this)
		{
			FGameplayTagContainer Container;
			static const FName InTagName = TEXT("Ability.Attack.C");
			FGameplayTag Tag = FGameplayTag::RequestGameplayTag(InTagName);
			Container.AddTag(Tag);
			bool bSucess = MyASC->STTryActivateAbilitiesByTag(Container);
			UE_LOG(LogSTGame, Verbose, TEXT("[ASTCharacter::Attack] bSucess === %d"), bSucess);
		}
	}

	UE_LOG(LogSTGame, Verbose, TEXT("[ASTCharacter::Attack] End === "));
}

void ASTCharacter::Initialize(USTCharacterInfo* InCharInfo, const FVector& location, const FRotator& rotation)
{
	// 시작 위치와 방향 지정
	FTransform xform(FQuat(rotation), location);
	SetActorTransform(xform);
	
	int32 unitIndex = InCharInfo->GetUID();
	CharTable = FSTDataTableManager::GetResCharacter(unitIndex);
	CharType = CharTable->Char_Type;	

	// 크기 지정
	//GetCapsuleComponent()->SetRelativeScale3D(FVector(CharTable->Scale));

	// 설정 컨트롤러 연결
	SpawnDefaultController();		

	// 스탯 수치 설정
	AbilityStatComponent->Initialize(InCharInfo);
	// 초기화가 완료되었다.
	bInitialized = true;
}

void ASTCharacter::SetActive(bool bEnable)
{
	//해당 캐릭터 상태 활성화 처리한다.
	bReplicatedActive = bEnable;
	OnRep_Active();
	//서버측도 처리할거 처리한다. 
	if (GetNetMode() != ENetMode::NM_Client)
	{
		
	}
	// 캐릭터 활성화 여부에 따라 액션에 대한 처리를 활성화
	SetAction(bEnable);
}

void ASTCharacter::OnRep_Active()
{
	// 활성화 여부에 따라 게임에서 보일지 설정.
	SetActorHiddenInGame(!bReplicatedActive);
	// 활성화에 따라 틱들 끄고 켠다.
	STHelper::SetActorTickActivate(this, bReplicatedActive);
}

void ASTCharacter::SetAction(bool bEnable)
{
	//해당 캐릭터 액션 가능을 처리한다.
	bReplicatedAction = bEnable;
	OnRep_Action();
	// 서버측도 처리할거 처리한다. 
	if (GetNetMode() != ENetMode::NM_Client)
	{
		
	}
}

void ASTCharacter::OnRep_Action()
{

}

#pragma region RepMovement

FSharedRepMovement::FSharedRepMovement()
{
	RepMovement.LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
}

bool FSharedRepMovement::FillForCharacter(ACharacter* Character)
{
	if (USceneComponent* PawnRootComponent = Character->GetRootComponent())
	{
		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();

		RepMovement.Location = FRepMovement::RebaseOntoZeroOrigin(PawnRootComponent->GetComponentLocation(), Character);
		RepMovement.Rotation = PawnRootComponent->GetComponentRotation();
		RepMovement.LinearVelocity = CharacterMovement->Velocity;
		RepMovementMode = CharacterMovement->PackNetworkMovementMode();
		bProxyIsJumpForceApplied = Character->bProxyIsJumpForceApplied || (Character->JumpForceTimeRemaining > 0.0f);
		bIsCrouched = Character->bIsCrouched;

		// Timestamp is sent as zero if unused
		if ((CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Linear) || CharacterMovement->bNetworkAlwaysReplicateTransformUpdateTimestamp)
		{
			RepTimeStamp = CharacterMovement->GetServerLastTransformUpdateTimeStamp();
		}
		else
		{
			RepTimeStamp = 0.f;
		}

		return true;
	}
	return false;
}

bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const
{
	if (RepMovement.Location != Other.RepMovement.Location)
	{
		return false;
	}

	if (RepMovement.Rotation != Other.RepMovement.Rotation)
	{
		return false;
	}

	if (RepMovement.LinearVelocity != Other.RepMovement.LinearVelocity)
	{
		return false;
	}

	if (RepMovementMode != Other.RepMovementMode)
	{
		return false;
	}

	if (bProxyIsJumpForceApplied != Other.bProxyIsJumpForceApplied)
	{
		return false;
	}

	if (bIsCrouched != Other.bIsCrouched)
	{
		return false;
	}

	return true;
}

bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	RepMovement.NetSerialize(Ar, Map, bOutSuccess);
	Ar << RepMovementMode;
	Ar << bProxyIsJumpForceApplied;
	Ar << bIsCrouched;

	// Timestamp, if non-zero.
	uint8 bHasTimeStamp = (RepTimeStamp != 0.f);
	Ar.SerializeBits(&bHasTimeStamp, 1);
	if (bHasTimeStamp)
	{
		Ar << RepTimeStamp;
	}
	else
	{
		RepTimeStamp = 0.f;
	}

	return true;
}

#pragma endregion RepMovement
