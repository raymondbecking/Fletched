// Fill out your copyright notice in the Description page of Project Settings.


#include "TextWidget.h"
#include "Components/TextBlock.h"

void UTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(TextLabel == nullptr)
	{		
		UE_LOG(LogTemp, Warning, TEXT("WidgetBP does not have a TextBlock attached!"))		
	}
}

void UTextWidget::SetTextLabel(FText &Text)
{
	if(TextLabel != nullptr)
	{
		TextLabel->SetText(Text);		
	}
}

void UTextWidget::SetColor(FColor &Color)
{
	if(TextLabel != nullptr)
	{		
		TextLabel->SetColorAndOpacity(Color);		
	}
}
	
