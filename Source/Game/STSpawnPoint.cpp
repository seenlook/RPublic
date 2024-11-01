// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Objects/STSpawnPoint.h"

#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"

ASTSpawnPoint::ASTSpawnPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	if (GetSpriteComponent() != nullptr)
	{
		GetSpriteComponent()->SetRelativeScale3D( FVector(0.75f, 0.75f, 0.75f));
	}
	
	if (GetArrowComponent() != nullptr)
	{
		GetArrowComponent()->ArrowSize = 1.6f;
	}

	CreateRenderText();
#endif
}

void ASTSpawnPoint::PostActorCreated()
{
	Super::PostActorCreated();
#if WITH_EDITOR
	ReIndex();
	RenderText();
#endif
}

void ASTSpawnPoint::PostLoad()
{
	Super::PostLoad();
#if WITH_EDITOR
	ReIndex();
	RenderText();
#endif
}

#if WITH_EDITOR
void ASTSpawnPoint::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	RenderText();
}

void ASTSpawnPoint::CreateRenderText()
{
	TextRenderComp = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("NewTextRenderComponent"));
	if (TextRenderComp)
	{
		TextRenderComp->SetupAttachment(RootComponent);
		TextRenderComp->SetText(FText::FromString(TEXT("P")));
		TextRenderComp->SetYScale(1.0f);
		TextRenderComp->SetXScale(1.0f);
		TextRenderComp->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
		TextRenderComp->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextBottom);
		TextRenderComp->SetWorldSize(100.f);
		// Color the text 
		TextRenderComp->SetTextRenderColor(FLinearColor(0.5f, 0.f, 0.075f).ToFColor(false));
		TextRenderComp->bAffectDynamicIndirectLighting = false;
		TextRenderComp->bAffectDistanceFieldLighting = false;
		TextRenderComp->SetVisibility(false);
	}
}

void ASTSpawnPoint::ReIndex()
{
	bool bUnique = true;
	for (AActor* Actor : GetLevel()->Actors)
	{
		if (ASTSpawnPoint* sp = Cast<ASTSpawnPoint>(Actor))
		{
			if (sp == this)
			{
				continue;
			}
			if (sp->Index == Index)
			{
				bUnique = false;
				break;
			}
		}
	}
	
	if (bUnique)
	{
		return;
	}

	int32 lastIndex = 0;
	for (AActor* Actor : GetLevel()->Actors)
	{
		if (ASTSpawnPoint* sp = Cast<ASTSpawnPoint>(Actor))
		{
			if (sp == this)
			{
				continue;
			}
			if (sp->Index > lastIndex)
			{
				lastIndex = sp->Index;
			}
		}
	}	
	Index = lastIndex + 1;
}

void ASTSpawnPoint::RenderText()
{
	if (TextRenderComp)
	{
		if (GetWorld()->WorldType != EWorldType::Game && GetWorld()->WorldType != EWorldType::PIE)
		{
			FString sp = GetRenderingString();		
			TextRenderComp->SetTextRenderColor(FLinearColor::Yellow.ToFColor(false));
			TextRenderComp->SetText(FText::FromString(sp));
			TextRenderComp->SetVisibility(true);
		}
		else
		{
			TextRenderComp->SetVisibility(false);
		}
	}
}

FString ASTSpawnPoint::GetRenderingString()
{
	return FString::Printf(TEXT("%d"), Index);
}
#endif //WITH_EDITOR

