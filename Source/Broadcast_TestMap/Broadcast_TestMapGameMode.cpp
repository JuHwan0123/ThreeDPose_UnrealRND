// Copyright Epic Games, Inc. All Rights Reserved.

#include "Broadcast_TestMapGameMode.h"
#include "Broadcast_TestMapCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABroadcast_TestMapGameMode::ABroadcast_TestMapGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
