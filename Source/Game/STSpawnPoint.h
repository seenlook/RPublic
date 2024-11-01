// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "STSpawnPoint.generated.h"

/**
 * @class : ASTSpawnPoint
 * @brief : NPC 스폰 포인트
 */
UCLASS()
class PROJECTR_API ASTSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	ASTSpawnPoint(const FObjectInitializer& ObjectInitializer);	
	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	void CreateRenderText();
	void ReIndex();
	void RenderText();
protected:
	virtual FString GetRenderingString();
#endif

public:
	UPROPERTY(EditAnywhere, TextExportTransient, BlueprintReadWrite, Category = "STGame|SpawnPoint")
	int32 Index = -1;

	UPROPERTY(EditAnywhere, TextExportTransient, BlueprintReadWrite, Category = "STGame|SpawnPoint")
	float Radius = 2.f;

#if	WITH_EDITORONLY_DATA
	// ExposeFunctionCategories - 블루프린트 에디터에서 함수 목록을 만들 때 들어있는 함수를 노출시킬 카테고리 목록을 지정합니다.
	UPROPERTY(Category = GameData, BlueprintReadOnly, Meta = (ExposeFunctionCategories = "Rendering|Components|TextRender", AllowPrivateAccess = "true"))
	class UTextRenderComponent* TextRenderComp;
#endif
	
};
