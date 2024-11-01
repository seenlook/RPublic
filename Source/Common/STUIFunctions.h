// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "STUIFunctions.generated.h"

class UTexture2D;
class UMaterialInstance;
class UPaperSprite;

UENUM(BlueprintType)
enum class ESSUIPathCategory : uint8
{
	UI = 0,
	WIDGET,
	WIDGETCOMPONENT,
	SOUND,
	SPRITE,
	TEXTURE,
	MATERIAL,
};

/**
 * 
 */
UCLASS()
class PROJECTR_API USTUIFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	static constexpr auto UI_PATH = TEXT("/Game/");
	static constexpr auto WIDGET_PATH = TEXT("/Game/");
	static constexpr auto WIDGET_COMPONENT_PATH = TEXT("/Game/");
	static constexpr auto SOUND_PATH = TEXT("/Game/Sounds/SFX/UI/");
	static constexpr auto SPRITE_PATH = TEXT("/Game/UI/Sprite/");
	static constexpr auto TEXTURE_PATH = TEXT("/Game/UI/Texture/");
	static constexpr auto MATERIAL_PATH = TEXT("/Game/UI/Material/");

public:
	static UTexture2D* LoadTextureFromPath(const FString& InName);
	static UMaterialInstance* LoadMaterialFromPath(const FString& InName);

	template<class T>
	static T* LoadObjectFromPath(ESSUIPathCategory InCategory, const FString& InPath);

	static FString GetFullPath(ESSUIPathCategory InCategory, const FString& InName);
};

template<class T>
T* USTUIFunctions::LoadObjectFromPath(ESSUIPathCategory InCategory, const FString& InPath)
{
	if (InPath.IsEmpty())
	{
		return nullptr;
	}

	const FString imagePath = USTUIFunctions::GetFullPath(InCategory, InPath);
	return Cast<T>(StaticLoadObject(T::StaticClass(), nullptr, *(imagePath)));
}
