#include "STArmorComponent.h"
#include <Net/UnrealNetwork.h>

#include "../STCharacter.h"
#include "../../FX/STFXActor.h"
#include "../../PlayerController/STPlayerState.h"
#include "../../PlayerController/STPlayerController.h"

USTArmorComponent::USTArmorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTArmorComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerChar = Cast<ASTCharacter>(GetOwner());
}

void USTArmorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USTArmorComponent, LocalArmorGrade);
	DOREPLIFETIME(USTArmorComponent, bArmorActiveFlag);
}

int32 USTArmorComponent::GetArmorAmount()
{
	if (OwnerChar)
	{
		if (ASTPlayerState* PS = Cast<ASTPlayerState>(OwnerChar->GetPlayerState()))
		{
			return PS->GetCurrentArmor();
		}
	}

	return 0;
}

int32 USTArmorComponent::GetArmorMaxAmount()
{
	if (OwnerChar)
	{
		if (ASTPlayerState* PS = Cast<ASTPlayerState>(OwnerChar->GetPlayerState()))
		{
			return PS->GetMaxArmor();
		}
	}

	return 0;
}

int32 USTArmorComponent::GetArmorGrade()
{
	if (OwnerChar)
	{
		if (ASTPlayerState* PS = Cast<ASTPlayerState>(OwnerChar->GetPlayerState()))
		{
			return PS->GetArmorGrade();
		}
	}

	return 0;
}

void USTArmorComponent::SetArmorAmount(int32 InAmount)
{
	if (OwnerChar)
	{
		if (ASTPlayerState* PS = Cast<ASTPlayerState>(OwnerChar->GetPlayerState()))
		{
			int32 Temp = SetAmount;

			Temp = FMath::Clamp(Temp, 0, PS->GetMaxArmor());

			PS->SetCurrentArmor(Temp);
		}
	}
}

void USTArmorComponent::Server_MountArmor_Implementation()
{
	if (nullptr == OwnerChar || OwnerChar->IsActorDead())
	{
		return;
	}
	ASTPlayerState* myPS = Cast<ASTPlayerState>(OwnerChar->GetPlayerState())
	if (nullptr == myPS)
	{
		return;
	}

	if(USTInven_Character* invenComp = OwnerChar->GetInvenComponent())
	{
		if (USTItemArmor* armorInfo = invenComp->GetItemArmor())
		{
			myPS->SetCurrentArmor(armorInfo->GetDurabilityAmount());
			FDT_ITEM_Armor armorTable;
			if (USTDT_ITEM_Armor::Find(armorInfo->GetIndex(), armorTable))
			{
				LocalArmorGrade = armorInfo->BaseTableData.GetItemGrade();
				myPS->SetMaxArmor(armorTable.GetArmorHealth());
				myPS->SetArmorGrade(LocalArmorGrade);			
			}
			bArmorActiveFlag = true;
		}
	}
}

void USTArmorComponent::Server_UnMountArmor_Implementation()
{
	if (nullptr == OwnerChar || OwnerChar->IsActorDead())
	{
		return;
	}

	SetArmorAmount(0);
	bArmorActiveFlag = false;
}

void USTArmorComponent::Server_StopArmorAction_Implementation()
{
	bArmorActiveFlag = false;
}

void USTArmorComponent::Multi_UseArmorBatteryCharge_Implementation(float InAmountValue)
{
	SetArmorAmount(GetArmorAmount() + InAmountValue);

	bArmorActiveFlag = true;

	if (USTInven_Character* invenComp = OwnerChar->GetInvenComponent())
	{
		USTItemArmor* armorInfo = Cast<USTItemArmor>(invenComp->GetItemArmor());
		if (armorInfo)
		{
			if (invenComp)
			{
				invenComp->Server_SyncArmor(armorInfo->GetServerKey(), GetArmorAmount());
			}
		}
	}
}

void USTArmorComponent::OnArmorActiveFlag()
{
	if (OwnerChar)
	{
		if (bArmorActiveFlag)
		{
			TArray<ASTFXActor*> returnArray;
			USTFXUtil::STPlayNiagara(&returnArray, OwnerChar->GetWorld(), ActiveArmorEffectIndex, FTransform::Identity, OwnerChar->GetMesh());

			USTMaterialColorTable* colorTable = USTMaterialColorTable::GetTable();
			if (colorTable)
			{
				FLinearColor hitGradeColor = colorTable->GetColorByValue(FName("Mesh"), FName("Armor"), LocalArmorGrade);
				for (int32 i = 0; i < returnArray.Num(); ++i)
				{
					returnArray[i]->SetColorParam(6, hitGradeColor);
				}
			}
			bArmorActiveFlag = false;
			Server_StopArmorAction();
		}
	}
}
