// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NameTagComponent.generated.h"

enum class EWidgetSpace : uint8;

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
		
	//Name Tag can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NameTag")
	FText NameTagText;

	//Default color is set in the Widget BP
	UPROPERTY(EditAnywhere, Category = "NameTag")
	bool bOverrideDefaultColor;

	//Color can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere, Category = "NameTag")
	FColor NameTagColor;

	//Set the coordinate space to render the Name Tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NameTag")
	EWidgetSpace NameTagSpace;

	//Set custom offset for repositioning the Name Tag
	UPROPERTY(EditAnywhere, Category = "NameTag")
	FVector NameTagOffset = FVector(0, 0, 1.0f);
	
	UPROPERTY()
	class UTextWidget* NameTagTextWidget;

	//Set the Widget BP here, BP must have TextWidget as parent
	UPROPERTY(EditAnywhere, Category = "NameTag")
	UUserWidget* TextWidgetObject;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> TextWidgetClass;

	UPROPERTY()
	class UWidgetComponent* NameTagWidget;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
