// Fill out your copyright notice in the Description page of Project Settings.

/*  This component calculates the actual size in meters of this object using mesh bounds and object scale
	This component displays the size of the object as text above it    
	*/

#include "ObjectSizeComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetTextLibrary.h"

// Sets default values for this component's properties
UObjectSizeComponent::UObjectSizeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;	
}


// Called when the game starts
void UObjectSizeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (GetOwner() != nullptr)
	{
		SizeText = NewObject<UTextRenderComponent>(this, UTextRenderComponent::StaticClass(), TEXT("TextComponent"));

		if (SizeText)
		{
			//Register and attach TextRender to the root of the object this component attached
			SizeText->RegisterComponent();
			SizeText->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SizeText->CreationMethod = EComponentCreationMethod::Instance;


			TArray<UStaticMeshComponent*> MeshComponents;
			GetOwner()->GetComponents<UStaticMeshComponent>(MeshComponents);
			if(MeshComponents.Num() != 1)
			{
				UE_LOG(LogTemp, Warning, TEXT("Multiple StaticMeshes not supported, using RootComponent StaticMesh"));
			}


			
			//Calculate The size of the object
			FVector ObjectScale = GetOwner()->GetActorScale();
			FVector MeshSize = 2 * MeshComponents[0]->GetStaticMesh()->GetBounds().BoxExtent;
			FVector ObjectDimensions = MeshSize * ObjectScale;

			//Reposition Text based on object dimensions
			FVector ObjectTop = FVector(0, 0, MeshSize.Z);
			SizeText->AddLocalOffset(ObjectTop);

			FString DimensionText = "";	

			FString XSizeText = "X:" + FString::SanitizeFloat(ObjectDimensions.X) + "cm ";
			DimensionText = DimensionText + (bShowXSize ? XSizeText : "");

			FString YSizeText = "Y:" + FString::SanitizeFloat(ObjectDimensions.Y) + "cm ";
			DimensionText = DimensionText + (bShowYSize ? YSizeText : "");

			FString ZSizeText = "Z:" + FString::SanitizeFloat(ObjectDimensions.Z) + "cm ";
			DimensionText = DimensionText + (bShowZSize ? ZSizeText : "");

			SizeText->SetTextRenderColor(FColor::Blue);
			SizeText->SetText(FText::FromString(*DimensionText));
			//UE_LOG(LogTemp, Warning, TEXT("Scale: %s"), *DimensionText);
		}

	}
	
}


// Called every frame
void UObjectSizeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

