// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/STSeverHelper.h"
#include "Common/STHelper.h"
#include "Misc/MessageDialog.h"

#include "Character/Component/STCapsuleComponent.h"
#include "Game//Objects/STSpawnPoint.h"
#include "AI/STCharacterAIController.h"

namespace STServerHelper
{
	ASTCharacter* SpawnCharacter(UWorld* InWorld, USTCharacterInfo* InCharInfo, const FVector& InLocation, const FRotator& InRotation)
	{
		ensure(InCharInfo);
		
		const FResCharacter* resChar = InCharInfo->GetResCharacter();
		if (nullptr == resChar)
		{
			return nullptr;
		}

		UClass* charBP = nullptr;
		charBP = LoadObject<UClass>(NULL, *resChar->Char_Resource_ID);

		if (nullptr==charBP)
		{
			return nullptr;
		}
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ASTCharacter* spawnChar = InWorld->SpawnActor<ASTCharacter>(charBP, InLocation, InRotation, SpawnParameters);
		if (spawnChar)
		{
			spawnChar->Initialize(InCharInfo, InLocation + FVector(0.0f, 0.0f, spawnChar->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()), InRotation);
		}

		return spawnChar;
	}

	void SetAdjustedLocationAndRotation(ASTCharacter* InActor, FVector InLocation, FRotator InRotation, ETeleportType InTeleportType)		
	{
		FHitResult hitResult;  // 레이케스트를 통해 땅에 닿았는지 확인한다.
		if (hitResult.bBlockingHit)
		{
			InLocation = hitResult.ImpactPoint + FVector(0.0f, 0.0f, InActor->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		}

		InActor->GetWorld()->FindTeleportSpot(InActor, InLocation, InRotation);
		InActor->SetActorLocationAndRotation(InLocation, InRotation, false, nullptr, InTeleportType);
	}

	AActor* FindSpawnPoint(UWorld* InWorld, int32 InIndex)
	{
		ensure(InWorld);
		auto Predicate = [&](ASTSpawnPoint* sp)->bool
		{
			return (sp->Index == InIndex);
		};
		return STHelper::FindActor<ASTSpawnPoint>(InWorld, Predicate);
	}

	bool IsTargetable(ASTCharacter* InChar, ASTCharacter* InTarget)
	{
		if (InTarget == nullptr || InChar == nullptr)
		{
			return false;
		}		
		
		if (!InTarget->IsCanBeTargeted())
		{
			return false;
		}
		return STHelper::IsHostile(InChar, InTarget);
	}

	ASTCharacterAIController* GetAiController(ASTCharacter* InChar)
	{
		ensure(InChar);
		return Cast<ASTCharacterAIController>(InChar->GetController());
	}

} // STServerHelper 

