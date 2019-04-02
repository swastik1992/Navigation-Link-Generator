// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationLinkUtils.h"
#include "NavLinkModifier.generated.h"

UCLASS()
class NAVLINKGENERATOR_API ANavLinkModifier : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANavLinkModifier();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	/** The box shape component for modifier volume. */
	UPROPERTY()
	UBoxComponent* boxComponent;

	/** The modifier type to not have any links on the region. */
	UPROPERTY(EditAnywhere)
	ENavLinkModifierVolumeTypeA linkExistModifier = ENavLinkModifierVolumeTypeA::Default;

	/** The modifier type to define the left-right/ right-left usage of the link. (Only affects if linkExistModifier is set to default.)  */
	UPROPERTY(EditAnywhere)
	ENavLinkModifierVolumeTypeB directionModifier = ENavLinkModifierVolumeTypeB::BothDirection;

	/** The modifier type to increase margin on the links.(Only affects if linkExistModifier is set to default.)  */
	UPROPERTY(EditAnywhere)
	ENavLinkModifierVolumeTypeC marginModifier = ENavLinkModifierVolumeTypeC::DefaultMargin;


private:

	/** Show the direction of the links created. */
	UArrowComponent* _directionArrow;

};
