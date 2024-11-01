// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/STWidget_PlayerInfo.h"
#include "UI/UMGWidgets/STProgressBar.h"
#include "UI/UMGWidgets/STTextBlock.h"
#include "UI/UMGWidgets/STWidget_GradientBarBase.h"


void USTWidget_PlayerInfo::SetHP(int32 InHp)
{
	if (nullptr!=b_hp_w)
	{
		b_hp_w->BP_RefreshBar();
	}
}

void USTWidget_PlayerInfo::SetSP(int32 InSp)
{
	if (nullptr != b_stamina_w)
	{
		b_stamina_w->BP_RefreshBar();
	}
}

void USTWidget_PlayerInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}
