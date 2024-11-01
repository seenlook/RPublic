// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STInteractionComponent.h"

#include "PlayerController/STPlayerController.h"
#include "Character/STCharacter.h"
#include "Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(STInteractionComponent)

// Sets default values for this component's properties
USTInteractionComponent::USTInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USTInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USTInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (GetOwnerRole() != ROLE_Authority)
	{
		if (nullptr != GetOwnerPlayerController())
		{
			FHitResult HitResult;
			bool binteractif = IsInteractionActor(HitResult);
			ASTCharacter* MyOwner = Cast<ASTCharacter>(GetOwner());
			if (MyOwner != nullptr)
			{
				ASTPlayerController* OwnerController = Cast<ASTPlayerController>(MyOwner->GetController());
				if (OwnerController != nullptr)
				{
					// interation target 처리한다.
					// if binteractif ? OwnerController->SetInteractionTarget(HitResult.GetActor()) : OwnerController->SetInteractionTarget(nullptr);
				}
			}			
		}
	}
}

ASTPlayerController* USTInteractionComponent::GetOwnerPlayerController()
{
	if (GetOwner() != nullptr)
	{
		ASTCharacter* MyOwner = Cast<ASTCharacter>(GetOwner());
		if (MyOwner != nullptr)
		{
			ASTPlayerController* OwnerController = Cast<ASTPlayerController>(MyOwner->GetController());
			return OwnerController;
		}
	}
	return nullptr;
}

bool USTInteractionComponent::IsInteractionActor(FHitResult& OutHitResult)
{
	UWorld* World = GetWorld();
	if (nullptr == World)
	{
		return false;
	}

	ASTCharacter* MyOwner = Cast<ASTCharacter>(GetOwner());
	if (nullptr == MyOwner)
	{
		return false;
	}

	FCollisionShape collisionShape;
	collisionShape.SetSphere(InteractionRadius);

	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(MyOwner);
	collisionQueryParams.bReturnPhysicalMaterial = false;

	FCollisionObjectQueryParams objectQueryParams;
	objectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel2);  
	objectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel11); 
	objectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel12); 
	
	objectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	objectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	objectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);
	objectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	objectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	objectQueryParams.IgnoreMask = ECC_Camera | ECC_Visibility;
		
	const FVector Direction = MyOwner->GetActorForwardVector();
	const FVector StartLoc	= MyOwner->GetActorLocation();
	const FVector EndLoc	= StartLoc + Direction * InteractionDistance;

	TArray<FHitResult> hitResults;
	if (World->SweepMultiByObjectType(hitResults, StartLoc, EndLoc, FQuat::Identity, objectQueryParams, collisionShape, collisionQueryParams))
	{
		OutHitResult.Distance = -1;
		hitResults.Sort([&](const FHitResult& a, const FHitResult& b)
			{
				return a.Distance < b.Distance;
			});


		for (auto hit : hitResults)
		{
			if (hit.GetActor() == nullptr)
			{
				continue;
			}

			if (hit.GetComponent() == nullptr)
			{
				continue;
			}

			ECollisionChannel CollisionObjectType = hit.GetComponent()->GetCollisionObjectType();
			if (CollisionObjectType == ECollisionChannel::ECC_WorldStatic ||
				CollisionObjectType == ECollisionChannel::ECC_WorldDynamic)
			{
				break;
			}			

			if (OutHitResult.Distance >= 0)
			{
				return true;
			}
		}
	}
	return false;
}