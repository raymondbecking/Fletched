// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/Actor.h"
#include "ObjectSizeComponent.h"

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
			SizeText->RegisterComponent();
			SizeText->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SizeText->CreationMethod = EComponentCreationMethod::Instance;

			//FVector ObjectTop = FVector(0, 0, 10);//GetOwner()->StaticMesh
			//SizeText->AddLocalOffset(ObjectTop);

			SizeText->SetTextRenderColor(FColor::Green);
			SizeText->SetText(FText::FromString("Test"));
			UE_LOG(LogTemp, Warning, TEXT("Success: %s"), *GetOwner()->GetRootComponent()->GetName());
		}

	}
	
}


// Called every frame
void UObjectSizeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

