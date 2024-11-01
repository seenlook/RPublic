// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/STUIFunctions.h"

#include <Kismet/KismetMathLibrary.h>
#include "Engine/Texture2D.h"
#include "PaperSprite.h"
#include "Materials/MaterialInstance.h"

UTexture2D* USTUIFunctions::LoadTextureFromPath(const FString& InName)
{
	if (InName.IsEmpty())
	{
		return nullptr;
	}
	return Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *(InName)));
}

UMaterialInstance* USTUIFunctions::LoadMaterialFromPath(const FString& InName)
{
	if (InName.IsEmpty())
	{
		return nullptr;
	}

	return Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr, *(InName)));
}

FString USTUIFunctions::GetFullPath(ESSUIPathCategory InCategory, const FString& InName)
{
	FString path = TEXT("");
	switch (InCategory)
	{
	case ESSUIPathCategory::UI:
		path.Append(UI_PATH);
		break;
	case ESSUIPathCategory::WIDGET:
		path.Append(WIDGET_PATH);
		break;
	case ESSUIPathCategory::WIDGETCOMPONENT:
		path.Append(WIDGET_COMPONENT_PATH);
		break;
	case ESSUIPathCategory::TEXTURE:
		path.Append(TEXTURE_PATH);
		break;
	case ESSUIPathCategory::MATERIAL:
		path.Append(MATERIAL_PATH);
		break;
	case ESSUIPathCategory::SPRITE:
		path.Append(SPRITE_PATH);
		break;
	case ESSUIPathCategory::SOUND:
		path.Append(SOUND_PATH);
		break;
	default:
		break;
	}
	path.Append(InName);
	return path;
}

