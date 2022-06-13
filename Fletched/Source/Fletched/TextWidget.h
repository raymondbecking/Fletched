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
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	void SetValue(FText text) { Text = text;};

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TextLabel;

	virtual void NativeConstruct() override;

private:
	FText Text;
	
};
