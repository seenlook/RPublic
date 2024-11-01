#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STArmorComponent.generated.h"

/*
* @class : USTArmorComponent
* @brief : 캐릭터의 방어구 획득 , 착용 , 데미지 감소 처리 관리하는 컴포넌트
* */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTR_API USTArmorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USTArmorComponent();

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY()
	class ASTCharacter* OwnerChar;

	UPROPERTY(Replicated, ReplicatedUsing = "OnArmorActiveFlag")
	bool bArmorActiveFlag = false;

	UPROPERTY(EditAnywhere, Category = "STGame Armor")
	int32 ActiveArmorEffectIndex = 4000;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 LocalArmorGrade = 0;

public:

	UFUNCTION()
	int32 GetArmorAmount();

	UFUNCTION()
	int32 GetArmorMaxAmount();

	UFUNCTION()
	int32 GetArmorGrade();

	UFUNCTION()
	void SetArmorAmount(int32 InAmount);

public:
	UFUNCTION(Server, Reliable, Category = "STGame Armor")
	void Server_MountArmor();
	void Server_MountArmor_Implementation();

	UFUNCTION(Server, Reliable, Category = "STGame Armor")
	void Server_UnMountArmor();
	void Server_UnMountArmor_Implementation();

	UFUNCTION(Server, Reliable, Category = "STGame Armor")
	void Server_StopArmorAction();
	void Server_StopArmorAction_Implementation();

	UFUNCTION(Server, Reliable, Category = "STGame Armor")
	void Multi_UseArmorBatteryCharge(float InAmountValue);
	void Multi_UseArmorBatteryCharge_Implementation(float InAmountValue);

	UFUNCTION()
	void OnArmorActiveFlag();
		
};
