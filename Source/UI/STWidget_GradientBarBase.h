// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "UI/Common/STCommonUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "STWidget_GradientBarBase.generated.h"

class UMaterialInstance;
class USTImage;


/**
* @class : USTWidget_GradientBarBase
* @brief : Gradient ProgressBar 효과
*/
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "STGame GradientBarBase"))
class PROJECTR_API USTWidget_GradientBarBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "STGame Widget", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* BarMID = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "STGame Widget", meta = (AllowPrivateAccess = "true"))
	USTImage* BarImage = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "STGame Widget", meta = (AllowPrivateAccess = "true"))
	float BarPrePercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "STGame Widget", meta = (AllowPrivateAccess = "true"))
	float BarPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "STGame Widget", meta = (AllowPrivateAccess = "true"))
	float CurAlpha = 1.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "STGame Widget", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* BarMI = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "STGame Widget", meta = (AllowPrivateAccess = "true"))
	FName BarID = FName();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "STGame Widget", meta = (AllowPrivateAccess = "true"))
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "STGame Widget", meta = (AllowPrivateAccess = "true"))
	float ResponseTime = 0.1f;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;	
	virtual void NativeConstruct() override;

	virtual void UpdateBar();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_RefreshBar();
	
};
