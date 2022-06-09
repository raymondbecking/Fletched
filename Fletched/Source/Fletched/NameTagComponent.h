// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TextRenderComponent.h"
#include "NameTagComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeNameTag, FString, NameTagText);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLETCHED_API UNameTagComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNameTagComponent();

	//Text component to be attached to the owner
	UPROPERTY()
	UTextRenderComponent* NameTagObject;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void CreateNameTagWidget();

	virtual void SetNameTagPosition(FVector& Offset);

	virtual void SetNameTagText(FString& Text, FColor& Color);

	UPROPERTY(EditAnywhere, Category = "Class Types")
	TSubclassOf<UUserWidget> TextWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	class UWidgetComponent* NameTagWidget;

	//UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	//FChangeNameTag ChangeNameTag;
	
	//NameTag can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NameTag")
	FString NameTagText;

	//Color can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere)
	FColor NameTagColor;

	UPROPERTY(EditAnywhere)
	FVector NameTagOffset = FVector(0, 0, 1.0f);


private:
	/*UPROPERTY(EditDefaultsOnly)
	float TextOffset;*/


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
