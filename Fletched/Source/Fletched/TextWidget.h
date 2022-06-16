// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLETCHED_API UTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//Set the text value
	UFUNCTION(BlueprintInternalUseOnly)
	void SetTextLabel(FText &Text);

	//Set the color of the text
	UFUNCTION(BlueprintInternalUseOnly)
	void SetColor(FColor &Color);
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TextLabel = nullptr;

	virtual void NativeConstruct() override;

private:
	
};
