// Fill out your copyright notice in the Description page of Project Settings.

/*  This component calculates the actual size in meters of this object using mesh bounds and object scale
	This component displays the size of the object as text above it
	*/

#include "NameTagComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetTextLibrary.h"

	// Sets default values for this component's properties
UNameTagComponent::UNameTagComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//Name Tag defaults
	NameTagColor = FColor::FromHex("BCFFDAFF");
}


// Called when the game starts
void UNameTagComponent::BeginPlay()
{
	Super::BeginPlay();

	//Check if this component is attached to something
	if (GetOwner() != nullptr)
	{
		FString ObjectName = "TextComponent";
		CreateNameTagObject(ObjectName, NameTagText, NameTagColor, NameTagOffset);
			
		//TODO: could be useful later
		//Calculate the center of the mesh adjusted by object scale
		//FVector MeshCenter = ObjectScale * (FVector((MaxBounds.X + MinBounds.X), (MaxBounds.Y + MinBounds.Y), (MaxBounds.Z + MinBounds.Z)) / 2);	

	}

}

// Called every frame
void UNameTagComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UNameTagComponent::CreateNameTagObject(FString& ObjectName, FString &Text, FColor &Color, FVector& PositionOffset)
{
	NameTagObject = NewObject<UTextRenderComponent>(this, UTextRenderComponent::StaticClass(), *ObjectName);

	if (NameTagObject)
	{
		//Register and attach TextRender to the root of the object this component attached
		NameTagObject->RegisterComponent();
		NameTagObject->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
		NameTagObject->CreationMethod = EComponentCreationMethod::Instance;

		
		SetNameTagPosition(PositionOffset);
		SetNameTag(Text, Color);
	}
}

void UNameTagComponent::SetNameTagPosition(FVector &PositionOffset)
{
	//Get static mesh
	TArray<UStaticMeshComponent*> MeshComponents;
	GetOwner()->GetComponents<UStaticMeshComponent>(MeshComponents);
	if (MeshComponents.Num() != 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Multiple StaticMeshes not supported, using RootComponent StaticMesh"));
	}

	//Get the Bounds of the mesh
	FVector MinBounds;
	FVector MaxBounds;
	MeshComponents[0]->GetLocalBounds(MinBounds, MaxBounds);


	//Where the Name Tag should be positioned based on the mesh
	FVector NameTagPosition = FVector(0, 0, MaxBounds.Z);

	//Make sure offset isn't affected by actor scale
	PositionOffset = PositionOffset / GetOwner()->GetActorScale();

	//Reposition Text based on object bounds and offset
	NameTagObject->AddLocalOffset(NameTagPosition + PositionOffset);

	//Set text alignment to center
	NameTagObject->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
}

void UNameTagComponent::SetNameTag(FString &Text, FColor &Color)
{
	//Set NameTag Text Color
	NameTagObject->SetTextRenderColor(Color);

	//Set NameTag Text
	NameTagObject->SetText(FText::FromString(Text));

}


