// Copyright Epic Games, Inc. All Rights Reserved.

#include "FletchedGameMode.h"
#include "FletchedCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFletchedGameMode::AFletchedGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
