// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DebugToolsComponent.generated.h"

enum class EWidgetSpace : uint8;

//TODO: Rename to DebugToolsComponent, makes more sense in editor
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLETCHED_API UDebugToolsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDebugToolsComponent();

	//Calculates the dimension of the mesh this component is attached to
	FText CalculateObjectDimensions(AActor* ActorToCalculate) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, Category = "Debug Tools")
	bool bEnableNameTag;
	UPROPERTY(EditAnywhere, Category = "Debug Tools", meta=(EditCondition="bEnableNameTag"))
	bool bEnableDimensionNameTag;


	//Set the Widget BP here, BP must have TextWidget as parent
	UPROPERTY(EditAnywhere, Category="NameTag", meta=(EditCondition="bEnableNameTag"))
	UUserWidget* DebugTextWidgetObject;

	//Name Tag can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NameTag",
		meta=(EditCondition="bEnableNameTag && !bEnableDimensionNameTag"))
	FText DebugNameTagText;

	//Set the coordinate space to render the Name Tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NameTag", meta=(EditCondition="bEnableNameTag"))
	EWidgetSpace DebugNameTagSpace;

	//Color can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere, Category="NameTag", meta=(EditCondition="bEnableNameTag"))
	bool bOverrideWidgetColor;
	
	//Color can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere, Category="NameTag", meta=(EditCondition="bEnableNameTag && bOverrideWidgetColor"))
	FColor DebugNameTagColor;

	//Set custom offset for repositioning the Name Tag
	UPROPERTY(EditAnywhere, Category="NameTag", meta=(EditCondition="bEnableNameTag"))
	FVector DebugNameTagOffset = FVector(0, 0, 20.0f);	

	UPROPERTY()
	class UNameTagComponent* DebugNameTagComponent;
	

	UPROPERTY(EditAnywhere, Category="DimensionNameTag", meta=(EditCondition="bEnableDimensionNameTag"))
	bool bShowXDimension = false;

	UPROPERTY(EditAnywhere, Category="DimensionNameTag", meta=(EditCondition="bEnableDimensionNameTag"))
	bool bShowYDimension = false;

	UPROPERTY(EditAnywhere, Category="DimensionNameTag", meta=(EditCondition="bEnableDimensionNameTag"))
	bool bShowZDimension = false;
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
