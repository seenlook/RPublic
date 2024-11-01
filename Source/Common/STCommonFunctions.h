// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Containers/UnrealString.h>
#include "STCommonFunctions.generated.h"

/**
 * @class : USTCommonFunctions
 * @brief : 편의성을 위한 함수 유틸
 */
UCLASS()
class PROJECTR_API USTCommonFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	USTCommonFunctions() { }

	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="EnumType"></typeparam>
	/// <param name="InEnumName"></param>
	/// <param name="InValue"></param>
	/// <returns></returns>
	template <typename EnumType>
	static FORCEINLINE EnumType GetEnumValueFromValue(const FString& InEnumName, const int32 InValue)
	{
		const UEnum* Enum = FindFirstObjectSafe<UEnum>(*InEnumName, EFindFirstObjectOptions::ExactClass);
		if (nullptr == Enum)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid EnumName %s ,"), *InEnumName);
			return EnumType(0);
		}
		FName Name = Enum->GetNameByValue(InValue);
		if (Name.IsEqual(NAME_None))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid EnumName %s , InValue - %d"), *InEnumName, InValue);
			return EnumType(0);
		}
		const int64 ElemValue = Enum->GetValueByName(Name, EGetByNameFlags::ErrorIfNotFound);
		if (Enum->IsValidEnumValue(ElemValue))
		{
			return (EnumType)ElemValue;
		}
		UE_LOG(LogTemp, Error, TEXT("Invalid EnumName %s , InValue - %d"), *InEnumName, InValue);
		return EnumType(0);
	}
	
};

