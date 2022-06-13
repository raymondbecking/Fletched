// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NameTagComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLETCHED_API UNameTagComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNameTagComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void CreateNameTagWidget();

	virtual void SetNameTagPosition(FVector& Offset);

	virtual void SetNameTagText(FText& Text, FColor& Color);
		
	//NameTag can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NameTag")
	FText NameTagText;

	//Color can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere)
	FColor NameTagColor;

	UPROPERTY(EditAnywhere)
	FVector NameTagOffset = FVector(0, 0, 1.0f);

	UPROPERTY()
	class UTextWidget* NameTagTextWidget;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> TextWidgetClass;

	UPROPERTY()
	class UWidgetComponent* NameTagWidget;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
