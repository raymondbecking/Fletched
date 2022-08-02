// Copyright Epic Games, Inc. All Rights Reserved.

#include "FletchedGameMode.h"
#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFletchedGameMode::AFletchedGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Characters/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
