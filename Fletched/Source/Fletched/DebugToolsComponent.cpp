// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugToolsComponent.h"
#include "NameTagComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values for this component's properties
UDebugToolsComponent::UDebugToolsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DebugNameTagComponent = CreateDefaultSubobject<UNameTagComponent>("Name Tag Component");

	//Defaults
	DebugNameTagSpace = EWidgetSpace::Screen;
	DebugNameTagColor = FColor::White;
}


// Called when the game starts
void UDebugToolsComponent::BeginPlay()
{
	Super::BeginPlay();

	if(bEnableDimensionNameTag)
	{
		if(GetOwner() != nullptr)
		{
			DebugNameTagText = CalculateObjectDimensions(GetOwner());
		}
	}
	
	if(bEnableNameTag)
	{
		if(DebugTextWidgetObject != nullptr)
		{
			DebugNameTagComponent->ReconfigureTextWidget(*DebugTextWidgetObject, DebugNameTagText, DebugNameTagSpace,
			                                             DebugNameTagOffset, bOverrideWidgetColor, DebugNameTagColor);
		}
	}
	// ...
	
}

FText UDebugToolsComponent::CalculateObjectDimensions(AActor* ActorToCalculate) const
{
	TArray<UStaticMeshComponent*> MeshComponents;
	ActorToCalculate->GetComponents<UStaticMeshComponent>(MeshComponents);
	if(MeshComponents.Num() != 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Multiple StaticMeshes not supported, using RootComponent StaticMesh"));
	}
			
	//Calculate The size of the object
	const FVector ObjectScale = ActorToCalculate->GetActorScale();
	const FVector MeshSize = 2 * MeshComponents[0]->GetStaticMesh()->GetBounds().BoxExtent;
	const FVector ObjectDimensions = MeshSize * ObjectScale;

	FString DimensionText = "";	
	//String concat to show in-editor chosen dimensions
	FString XSizeText = "X:" + FString::SanitizeFloat(ObjectDimensions.X) + "cm ";
	DimensionText = DimensionText + (bShowXDimension ? XSizeText : "");
	FString YSizeText = "Y:" + FString::SanitizeFloat(ObjectDimensions.Y) + "cm ";
	DimensionText = DimensionText + (bShowYDimension ? YSizeText : "");
	FString ZSizeText = "Z:" + FString::SanitizeFloat(ObjectDimensions.Z) + "cm ";
	DimensionText = DimensionText + (bShowZDimension ? ZSizeText : "");
	
	return FText::FromString(DimensionText);
}


// Called every frame
void UDebugToolsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

