#pragma once

#include "Navigation/NavLinkProxy.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "NavigationLinkUtils.h"
#include "NavigationLinkGenerator.generated.h"

UCLASS()
class NAVLINKGENERATOR_API ANavigationLinkGenerator : public AActor
{
	GENERATED_BODY()

public:
	
	ANavigationLinkGenerator();

	virtual void BeginPlay() override;

	/**
	* Function to generate all the links on navmesh's edges.
	*/
	void GenerateNavLinks();

	/**
	* Helper function for GenerateNavLinks, which checks if a point is valid based on the line direction.
	* @param inPoint: The segment point.
	* @param inDirection: Direction of the point towards the next point in the nav mesh edge.
	* @param inNavLinks: Array to hold the final nav links.
	* @param inWorld: Pointer to the world generator is in.
	*/
	void CheckPointForLink(const FVector& inPoint, const FVector& inDirection, TArray<ANavLinkProxy*>& inNavLinks, UWorld* inWorld);

	/**
	* Helper function for CheckPointForLink, which checks if a point is valid based on trace direction and adds it to the inNavLinks array.
	* @param inPoint: The segment point.
	* @param inDirection: Direction of the point towards the next point in the nav mesh edge.
	* @param inTraceVector: Trace direction for each point.
	* @param inNavLinks: Array to hold the final nav links.
	* @param inWorld: Pointer to the world generator is in.
	*/
	bool FindAndAddNavLinks(const FVector& inPoint, const FVector& inDirection, const FVector& inTraceVector, TArray<ANavLinkProxy*>& inNavLinks, UWorld* inWorld);


#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/** Array for holding all the nav links.*/
	UPROPERTY()
	TArray<ANavLinkProxy*> _navLinks;

	/** Flag to check if the nav links generation is finished. */
	UPROPERTY()
	bool _bHasFinishedGeneration = false;

	/** Distance between nav links. */
	UPROPERTY(Category = Default, EditAnywhere)
	float _minDistanceBtNavLinks = 160;

	/** Maximum height check for links.*/
	UPROPERTY(Category = Default, EditAnywhere)
	float _maxHeight = 1200;

	/** Step height check for links.*/
	UPROPERTY(Category = Default, EditAnywhere)
	float _stepHeight = 40;

	/** Trace length distance.*/
	UPROPERTY(Category = Default, EditAnywhere)
	float _traceLength = 400;

	/** Left margin distance for right point of nav link.*/
	UPROPERTY(Category = Default, EditAnywhere)
	float _leftMargin = 400;

	/** Right margin distance for right point of nav link.*/
	UPROPERTY(Category = Default, EditAnywhere)
	float _rightMargin = 400;

	/** Trace length distance.*/
	UPROPERTY(Category = Default, EditAnywhere)
	float _minHeight = 400;

	/** Flag to recalculate the links. */
	UPROPERTY(Category = Default, EditAnywhere)
	bool _bRefreshGenerator = false;

	/** Flag to delete all the links. */
	UPROPERTY(Category = Default, EditAnywhere)
	bool _bClearGenerator = false;

};
