// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TextRenderComponent.h"
#include "NameTagComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLETCHED_API UNameTagComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNameTagComponent();

	//Text component to be attached to the owner
	UTextRenderComponent* NameTagObject;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void CreateNameTagObject(FString& ObjectName, FString& NameTagText, FColor& Color);

	virtual void SetNameTagPosition();

	virtual void SetNameTag(FString& Text, FColor& Color);

private:
	/*UPROPERTY(EditDefaultsOnly)
	float TextOffset;*/

	//NameTag can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere)
	FString NameTagText = "";

	//Color can be set in editor or accessed in child classes
	UPROPERTY(EditAnywhere)
	FColor NameTagColor = FColor::FromHex("BCFFDAFF");

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
