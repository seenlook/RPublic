// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Logging/LogMacros.h"

/**
 * @brief : Logs 
 */

class UObject;


PROJECTR_API DECLARE_LOG_CATEGORY_EXTERN(LogSTGame, Log, All);
PROJECTR_API DECLARE_LOG_CATEGORY_EXTERN(LogSTExperience, Log, All);
PROJECTR_API DECLARE_LOG_CATEGORY_EXTERN(LogSTAbilitySystem, Log, All);
PROJECTR_API DECLARE_LOG_CATEGORY_EXTERN(LogSTTeams, Log, All);
PROJECTR_API DECLARE_LOG_CATEGORY_EXTERN(LogST_UI, Log, All);

#define ST_LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define ST_LOG_FUNCTION(Verbosity) UE_LOG(LogSTGame, Verbosity, TEXT("%s"), *ST_LOG_CALLINFO)
#define ST_LOG_FUNCTION_COMMENT(Verbosity, Format, ...) UE_LOG(LogSTGame, Verbosity, TEXT("%s %s"), *ST_LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__)) 
#define ST_LOG_FUNCTION_CATEGORY(Category, Verbosity) UE_LOG(Category, Verbosity, TEXT("%s"), *ST_LOG_CALLINFO)
#define ST_LOG_FUNCTION_CATEGORY_COMMENT(Category, Verbosity, Format, ...) UE_LOG(Category, Verbosity, TEXT("%s %s"), *ST_LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__)) 

