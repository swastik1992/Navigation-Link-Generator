#pragma once
/*
//basic generator.
//Fixes.
//Modifier Volume.
//Async generator.
//Github Readme.
*/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "NavigationLinkUtils.generated.h"

UENUM(BlueprintType)
enum class ENavLinkModifierVolumeTypeA : uint8
{
	Default,
	NoLinks,
};


UENUM(BlueprintType)
enum class ENavLinkModifierVolumeTypeB : uint8
{
	BothDirection,
	LeftToRightLinks,
	RightToLeftLinks,
};

UENUM(BlueprintType)
enum class ENavLinkModifierVolumeTypeC : uint8
{
	DefaultMargin,
	LeftMargin,
	RightMargin,
	GlobalMargin
};