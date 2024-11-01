// Copyright Epic Games, Inc. All Rights Reserved.

#include "STPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "../Character/STCharacter.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"

#include "GameFrameWork/PlayerInput.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/Pawn.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/TimerHandle.h"
#include "TimerManager.h"

#include "Camera/STCameraComponent.h"
#include "GameMode/STGameMode.h"
#include "Character/STPawnExtensionComponent.h"
#include "STPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/STAbilitySystemComponent.h"
#include "Character/STDamageType.h"
#include "Common/STCheatManager.h"


ASTPlayerController::ASTPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

ASTPlayerState* ASTPlayerController::GetSTPlayerState() const
{
	return CastChecked<ASTPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

USTAbilitySystemComponent* ASTPlayerController::GetSTAbilitySystemComponent() const
{
	const ASTPlayerState* STPS = GetSTPlayerState();
	return (STPS ? STPS->GetSTAbilitySystemComponent() : nullptr);
}

AHUD* ASTPlayerController::GetGameHUD() const
{
	return CastChecked<AHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

/// <summary>
/// AActor interface
/// </summary>
void ASTPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASTPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	CheatClass = USTCheatManager::StaticClass();
	AddCheats(true);
}

void ASTPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASTPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);	
}

/// <summary>
/// AController interface
/// </summary>
/// <param name="InPawn"></param>

void ASTPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ASTPlayerController::OnUnPossess()
{	
	// 소유 해제된 폰이 ASC의 아바타 액터로 남지 않도록 nullptr로 설정합니다.
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

void ASTPlayerController::BroadcastOnPlayerStateChanged()
{
}

void ASTPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	BroadcastOnPlayerStateChanged();
}

void ASTPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	BroadcastOnPlayerStateChanged();
}

void ASTPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BroadcastOnPlayerStateChanged();
}

/// <summary>
/// APlayerController interface
/// </summary>

void ASTPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (IsLocalController())
	{
		// 로컬 처리
	}

	if (nullptr!=InPawn)
	{
		ASTCharacter* character = Cast<ASTCharacter>(InPawn);
		if (nullptr!=character)
		{
			// 플레이어 처리
		}
		
		// 치트매니저 등록	
		CheatClass = USTCheatManager::StaticClass();		
	}
}

void ASTPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void ASTPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

}

void ASTPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);	
}

void ASTPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ASTPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (USTAbilitySystemComponent* ASC = GetSTAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ASTPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ASTPlayerController::SetupInputComponent()
{
	// 게임플레이 키 바인딩 설정
	Super::SetupInputComponent();

	// 액션 바인딩 설정
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 마우스 입력 이벤트 설정
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ASTPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ASTPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ASTPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ASTPlayerController::OnSetDestinationReleased);

		// 터치 입력 이벤트 설정
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ASTPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ASTPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ASTPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ASTPlayerController::OnTouchReleased);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASTPlayerController::OnInputStarted()
{
	StopMovement();
}

void ASTPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);
	// 터치 위치 아래에 무엇이 있는지 추적
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// 무언가를 클릭했으면 이동
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ASTPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// 걷기 애니메이션이 올바르게 재생되도록 충분히 멀리 이동 명령을 내려야 한다.
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ASTPlayerController::SetAvatar(ASTCharacter* InAvatar)
{
	Avatar = InAvatar;
}

bool ASTPlayerController::IsAvatar(ASTCharacter* InAvatar)
{
	check(InAvatar);
	return (InAvatar == Avatar.Get());
}

void ASTPlayerController::ProcessInput()
{
	ASTCharacter* MyCharacter = Cast<ASTCharacter>(GetPawn());
	if (nullptr == MyCharacter)
	{
		return;
	}

	float InputScale = MoveInput.Size();
	if (InputScale <= 0.0f)
	{
		return;  
	}
	
	USTCameraComponent* TopDownCameraBoom = MyCharacter->GetSTCameraComponent();
	FVector forwardVector = TopDownCameraBoom->GetForwardVector() * MoveInput.X + TopDownCameraBoom->GetRightVector() * MoveInput.Y;
	forwardVector.Z = 0.f;
	forwardVector.Normalize(); 

	if (MyCharacter->IsManualMovablePolicy())
	{
		MyCharacter->AddMovementInput(forwardVector, InputScale);
	}
}

// 입력이 눌린 상태로 매 프레임 트리거됨
void ASTPlayerController::OnSetDestinationTriggered()
{
	FollowTime += GetWorld()->GetDeltaSeconds();

	// 플레이어가 입력을 누른 위치를 찾습니다.
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// 위치를 저장
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	// 마우스 포인터 또는 터치 방향으로 이동
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void ASTPlayerController::OnSetDestinationPressed()
{
	bMoveToMouseCursor = true;
}

void ASTPlayerController::OnSetDestinationReleased()
{
	bMoveToMouseCursor = false;
	// 짧게 눌렀다면
	if (FollowTime <= ShortPressThreshold)
	{
		// 그쪽으로 이동하고 fx 파티클을 생성한다.
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// 입력이 눌린 상태로 매 프레임 트리거됨
void ASTPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ASTPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void ASTPlayerController::OnJump()
{
	if (bIsJump)
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		// 점프
	}

}

void ASTPlayerController::Client_Death_Implementation()
{
	if (ASTCharacter* c = GetPawn<ASTCharacter>())
	{
		c->Death();		
	}
}

void ASTPlayerController::Server_Death_Implementation(bool bFainting)
{
	ASTCharacter* characterRef = GetPawn<ASTCharacter>();
	if (nullptr == characterRef)
	{
		return;
	}

	if (ASTPlayerState* ps = GetPlayerState<ASTPlayerState>())
	{
		ps->SetIsDeath(true);
	}

	if (bFainting)
	{
		// 기절상태에서 죽었다.		
	}

	if (characterRef)
	{
		if (ASTGameMode* mode = Cast<ASTGameMode>(UGameplayStatics::GetGameMode(characterRef->GetWorld())))
		{		
			//mode->
		}
	}
	Client_Death();
}

void ASTPlayerController::Client_Fainting_Implementation(ASTCharacter* InVictim, const USTDamageType* InDamageType)
{
	if (ASTCharacter* c = GetPawn<ASTCharacter>())
	{
		c->Server_Fainting(InVictim, InDamageType);

		c->ToFainting();	
	}
}

void ASTPlayerController::Server_Fainting_Implementation(ASTCharacter* Victim, const USTDamageType* InDamageType)
{

}

bool ASTPlayerController::Server_Fainting_Validate(ASTCharacter* Victim, const USTDamageType* InDamageType)
{
	return true;
}

void ASTPlayerController::DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
}


