// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STInteractionComponent.generated.h"

class ASTCharacter;
class ASTPlayerController;
struct FHitResult;

/* <summary>
* @class : class USTInteractionComponent
* @brief : 캐릭터간의 상호 작용을 처리 한다. 
*/ 

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTR_API USTInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	ASTPlayerController* GetOwnerPlayerController();

	bool IsInteractionActor(FHitResult& OutHitResult);


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	float InteractionRadius = 10.f;

	UPROPERTY()
	float InteractionDistance = 1000.f;

	UPROPERTY()
	float InteractionRate = 0.1f;

	UPROPERTY()
	TWeakObjectPtr<ASTCharacter> OwnerCharacter = nullptr;
	

		
};
