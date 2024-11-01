// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/STWidgetBase.h"
#include "STWidget_PlayerInfo.generated.h"

class USTTextBlock;
class USTProgressBar;
class USTWidget_GradientBarBase;


/**
 * @class : USTWidget_PlayerInfo
 * @brief : 플레이어 정보 표시 - hp 등
 */
UCLASS(BlueprintType,Blueprintable)
class PROJECTR_API USTWidget_PlayerInfo : public USTWidgetBase
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	USTTextBlock* b_txt_hp;	

	UPROPERTY(meta = (BindWidget))
	USTWidget_GradientBarBase* b_hp_w;

	UPROPERTY(meta = (BindWidget))
	USTWidget_GradientBarBase* b_stamina_w;

public:
	void SetHP(int32 InHp);
	void SetSP(int32 InSp);
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
