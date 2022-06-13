// Fill out your copyright notice in the Description page of Project Settings.


#include "TextWidget.h"
#include "Components/TextBlock.h"

void UTextWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	TextLabel->SetText(Text);
}
