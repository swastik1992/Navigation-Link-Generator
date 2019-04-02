// Fill out your copyright notice in the Description page of Project Settings.


#include "NavLinkModifier.h"

// Sets default values
ANavLinkModifier::ANavLinkModifier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	boxComponent = CreateDefaultSubobject<UBoxComponent>("box");
	RootComponent = boxComponent;
	boxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	boxComponent->SetWorldScale3D(FVector::OneVector * 10);

	_directionArrow = CreateDefaultSubobject<UArrowComponent>("arrow");
	_directionArrow->AttachToComponent(boxComponent, FAttachmentTransformRules::KeepRelativeTransform);
	_directionArrow->ArrowSize *= 0.5f;
	_directionArrow->SetArrowColor(FLinearColor::Blue);
}

// Called when the game starts or when spawned
void ANavLinkModifier::BeginPlay()
{
	Super::BeginPlay();
}
