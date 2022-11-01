// Fill out your copyright notice in the Description page of Project Settings.

/*  This component calculates the actual size in meters of this object using mesh bounds and object scale
	This component displays the size of the object as text above it
	*/

#include "NameTagComponent.h"
#include "TextWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetTextLibrary.h"

	// Sets default values for this component's properties
UNameTagComponent::UNameTagComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	NameTagWidget = CreateDefaultSubobject<UWidgetComponent>("NameTagWidget");
	
	//Name Tag defaults
	NameTagColor = FColor::FromHex("BCFFDAFF");
	//Space default
	NameTagSpace = EWidgetSpace::Screen;
}


// Called when the game starts
void UNameTagComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CreateNameTagWidget(NameTagText, NameTagColor);

	//TODO: could be useful later
	//Calculate the center of the mesh adjusted by object scale
	//FVector MeshCenter = ObjectScale * (FVector((MaxBounds.X + MinBounds.X), (MaxBounds.Y + MinBounds.Y), (MaxBounds.Z + MinBounds.Z)) / 2);
}

// Called every frame
void UNameTagComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Creation of the NameTagWidget 
void UNameTagComponent::CreateNameTagWidget(FText& Text, FColor& Color)
{
	//Check if this component is attached to something
	if (GetOwner() != nullptr)
	{
		if(NameTagWidget != nullptr)
		{
			SetupNameTagWidget();
			SetNameTagText(Text, Color);			
			SetNameTagPosition(NameTagOffset);	
		}
	}	
}

/* Setup and settings for the NameTagWidget
 */
void UNameTagComponent::SetupNameTagWidget()
{
	//Widget Setup
	if(TextWidgetObject != nullptr)
	{
		TextWidgetClass = TextWidgetObject->GetClass();
	}
	NameTagWidget->SetWidgetClass(TextWidgetClass);
	NameTagWidget->SetWidgetSpace(NameTagSpace);
	NameTagWidget->SetDrawSize(FVector2D(700.0f, 30.0f));
	NameTagWidget->SetVisibility(true);

	//Register and attach widget to where this NameTagComponent is attached to
	NameTagWidget->RegisterComponent();
	NameTagWidget->AttachToComponent(GetOwner()->GetRootComponent(),
	                            FAttachmentTransformRules::FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
		                            EAttachmentRule::SnapToTarget,
		                            EAttachmentRule::KeepWorld,
		                            true));
	NameTagWidget->CreationMethod = EComponentCreationMethod::Instance;	
}

/* This function calculates the topside of the mesh this component is attached to
 * and then sets the Name Tag Widget to this position taking the editor defined
 * offset in consideration.
 */
void UNameTagComponent::SetNameTagPosition(FVector &PositionOffset)
{
	//Get the owner static mesh where this component is attached to
	TArray<UStaticMeshComponent*> MeshComponents;
	GetOwner()->GetComponents<UStaticMeshComponent>(MeshComponents);

	if (MeshComponents.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s does not have a static mesh attached!"), *GetOwner()->GetName());		
		return;
	}
	if (MeshComponents.Num() != 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Multiple StaticMeshes not supported, using RootComponent StaticMesh"));
	}
	
	//Out vectors for the bounds of the static mesh
	FVector MinBounds;
	FVector MaxBounds;	
	
	MeshComponents[0]->GetLocalBounds(MinBounds, MaxBounds);
	
	//Where the Name Tag should be positioned based on the static mesh
	FVector NameTagPosition = FVector(0, 0, MaxBounds.Z);

	//Make sure offset isn't affected by actor scale
	PositionOffset = PositionOffset / GetOwner()->GetActorScale();

	//Change the rotation of the NameTag in world space
	NameTagWidget->AddLocalRotation(NameTagRotation);
	
	//Reposition Text based on object bounds and the offset
	NameTagWidget->AddLocalOffset(NameTagPosition + PositionOffset);
}

void UNameTagComponent::SetNameTagText(FText& Text, FColor& Color)
{
	NameTagWidget->InitWidget();
	if (NameTagWidget->GetUserWidgetObject() == nullptr)
	{
		return;
	}
	//Cast to the parent class of the Widget BP
	NameTagTextWidget = Cast<UTextWidget>(NameTagWidget->GetUserWidgetObject());
	NameTagTextWidget->SetTextLabel(Text);

	if (bOverrideDefaultColor)
	{
		//Set NameTag Text Color
		NameTagTextWidget->SetColor(Color);
	}
	
}

void UNameTagComponent::ReconfigureTextWidget(UUserWidget& Widget, FText Text, EWidgetSpace WidgetSpace,
                                              FVector WidgetOffset, FRotator WidgetRotation, bool bOverrideTextColor, FColor Color)
{
	TextWidgetObject = &Widget;
	NameTagText = Text;
	NameTagOffset = WidgetOffset;
	NameTagRotation = WidgetRotation;
	NameTagSpace = WidgetSpace;
	
	if(bOverrideTextColor)
	{
		NameTagColor = Color;
		bOverrideDefaultColor = true;
	}
}



