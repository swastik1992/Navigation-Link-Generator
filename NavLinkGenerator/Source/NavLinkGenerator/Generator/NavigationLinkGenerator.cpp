#include "NavigationLinkGenerator.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavAgentInterface.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavLinkCustomComponent.h"
#include "EngineUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

ANavigationLinkGenerator::ANavigationLinkGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

}

// Called when the game starts or when spawned
void ANavigationLinkGenerator::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = this->GetWorld();
	if (world == nullptr) return;

	UNavigationSystemV1* navSys = Cast<UNavigationSystemV1>(world->GetNavigationSystem());
	if (navSys == nullptr) return;

	for (auto const& navLink : _navLinks)
	{
		if (navLink)
		{
			navSys->UpdateActorInNavOctree(*navLink);
		}
	}
}

void ANavigationLinkGenerator::GenerateNavLinks()
{
	UWorld* world = GetWorld();
	if (world == nullptr) return;

	const UNavigationSystemV1* navSystem = Cast< UNavigationSystemV1>(world->GetNavigationSystem());
	if (navSystem == nullptr) return;

	//Get navmeshrecast, which hold all the data to create nav mesh.
	ARecastNavMesh* navMeshRecast = const_cast<ARecastNavMesh*>(static_cast<const ARecastNavMesh*>(navSystem->GetMainNavData()));
	if (navMeshRecast == nullptr) return;

	//Get all the nav mesh edges.
	FRecastDebugGeometry navMeshGeometry;
	navMeshGeometry.bGatherNavMeshEdges = true;
	navMeshRecast->BeginBatchQuery();
	navMeshRecast->GetDebugGeometry(navMeshGeometry);

	//Destroy and delete all the links in _navLinks.
	for (ANavLinkProxy* aLink : _navLinks)
	{
		if (aLink)
		{
			aLink->Destroy();
		}
	}
	_navLinks.Empty(0);

	_bHasFinishedGeneration = false;

	TArray<FVector>& navMeshEdges = navMeshGeometry.NavMeshEdges;

	//Go through each edge to test the points.
	for (int32 i = 0; i < navMeshEdges.Num(); i += 2)
	{
		const FNavLocation startPoint = FNavLocation(navMeshEdges[i]);
		const FNavLocation endPoint = FNavLocation(navMeshEdges[i + 1]);

		const FVector line = endPoint.Location - startPoint.Location;
		FVector lineDirection = line;
		lineDirection.Normalize();

		FVector linePerp = FVector(line.Y, -line.X, 0);

		//Test start and end point of edge with the line segment's direction.
		CheckPointForLink(startPoint, lineDirection, _navLinks, world);
		CheckPointForLink(endPoint, lineDirection, _navLinks, world);

		const float lineSize = line.Size();

		//checking intermediate points between edges. 
		int32 linkCounter = 1;
		if (lineSize > _minDistanceBtNavLinks)
		{
			const int32 numSegment = lineSize / _minDistanceBtNavLinks;
			for (int32 j = 0; j < numSegment; ++j)
			{
				const FVector point = startPoint.Location + (j * _minDistanceBtNavLinks * lineDirection);
				CheckPointForLink(point, lineDirection, _navLinks, world);
			}
		}

	}

	_bHasFinishedGeneration = true;
}

void ANavigationLinkGenerator::CheckPointForLink(const FVector& inPoint, const FVector& inDirection, TArray<ANavLinkProxy*>& inNavLinks, UWorld* inWorld)
{
	//Find the forward and backward trace direction.
	FVector traceDirection = FVector(inDirection.Y, -inDirection.X, 0);
	traceDirection.Normalize();

	const FVector traceVectorForward = traceDirection * _traceLength;
	const FVector traceVectorBackward = -traceDirection * _traceLength;

	//Check which trace direction is correct.
	if (!FindAndAddNavLinks(inPoint, inDirection, traceVectorForward, inNavLinks, inWorld))
	{
		FindAndAddNavLinks(inPoint, -inDirection, traceVectorBackward, inNavLinks, inWorld);
	}
}

bool ANavigationLinkGenerator::FindAndAddNavLinks(const FVector& inPoint, const FVector& inDirection, const FVector& inTraceVector, TArray<ANavLinkProxy*>& inNavLinks, UWorld* inWorld)
{
	FHitResult hitResult;
	FCollisionQueryParams params;

	//Ignore the point if nothing intersect between point to traceVector (perpendicular of line direction). 
	if (!inWorld->LineTraceSingleByChannel(hitResult, inPoint, inPoint + inTraceVector, ECC_WorldStatic, params)) return false;

	//Calculate a new trace vector, unnecessary at the moment, still keeping it.
	FVector traceDirection = inTraceVector;
	traceDirection.Normalize();
	traceDirection *= _traceLength;

	//Check till the max height from the point. increment using stepHeight.
	for (float j = 0; j <= _maxHeight; j += _stepHeight)
	{
		const FVector startPos = inPoint + FVector(0, 0, j);

		//Proceed only if nothing intersect between startPos to trace direction. (line parallel to the perpendicular vector of line segment, different height).
		if (!inWorld->LineTraceTestByChannel(startPos, startPos + traceDirection, ECC_WorldStatic, params))
		{
			//Ignore the point if something intersect between startPos to -ve of trace direction. (line parallel to the counter clock wise perpendicular vector of line segment, different height).
			if (inWorld->LineTraceTestByChannel(startPos, startPos + (-traceDirection), ECC_WorldStatic, params)) return false;

			//Ignore the point if it's under min height.
			if (FMath::Abs(inPoint.Z - startPos.Z) < _minHeight) return false;

			if (inWorld->LineTraceTestByChannel(startPos - FVector(0, 0, _stepHeight), startPos, ECC_WorldStatic, params)) return false;

			float dist = FVector::Dist(inPoint, startPos);

			FVector traceDirectionMargin = traceDirection;
			traceDirectionMargin.Normalize();

			float leftMargin = _leftMargin;
			float rightMargin = _rightMargin;

			const FVector startPosLeft = startPos + (-traceDirectionMargin * leftMargin);
			const FVector endPosLeft = startPosLeft + FVector(0, 0, -dist * 1.5f);

			//Find the left hit point.
			FHitResult hitResultLeft;
			bool bIsLeftHit = inWorld->LineTraceSingleByChannel(hitResultLeft, startPosLeft, endPosLeft, ECC_WorldStatic, params);

			const FVector startPosRight = startPos + (traceDirectionMargin * rightMargin);
			const FVector endPosRight = startPosRight + FVector(0, 0, -dist);

			//Find the right hit point.
			FHitResult hitResultRight;
			bool bIsRightHit = inWorld->LineTraceSingleByChannel(hitResultRight, startPosRight, endPosRight, ECC_WorldStatic, params);

			if (bIsLeftHit && bIsRightHit)
			{
				FVector finalLeftPoint = hitResultLeft.Location;
				FVector finalRightPoint = hitResultRight.Location;

				UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(inWorld);
				if (navSys == nullptr) return false;

				FNavLocation leftNavPoint;
				FNavLocation rightNavPoint;

				//Project both left and right point to nav mesh.
				bool bShouldContinue = false;
				if (navSys->ProjectPointToNavigation(finalLeftPoint, leftNavPoint))
				{
					if (navSys->ProjectPointToNavigation(finalRightPoint, rightNavPoint))
					{
						bShouldContinue = true;
					}
				}

				if (!bShouldContinue) return false;

				if (inWorld->LineTraceTestByChannel(leftNavPoint.Location, leftNavPoint.Location + FVector(0, 0, j), ECC_WorldStatic, params)) return false;
				if (inWorld->LineTraceTestByChannel(rightNavPoint.Location, rightNavPoint.Location + FVector(0, 0, j), ECC_WorldStatic, params)) return false;

				if (FVector::Dist(leftNavPoint.Location, finalLeftPoint) > 100.0f) return false;

				if (FVector::Dist(rightNavPoint.Location, finalRightPoint) > 100.0f) return false;

				ANavLinkProxy *link = nullptr;

				link = GetWorld()->SpawnActor<ANavLinkProxy>(ANavLinkProxy::StaticClass(), FTransform(inPoint));

				//Transform left and right point to link's coordinate system.
				link->PointLinks[0].Left = link->GetTransform().InverseTransformPosition(rightNavPoint.Location);
				link->PointLinks[0].Right = link->GetTransform().InverseTransformPosition(leftNavPoint.Location);

				ENavLinkDirection::Type originalDirection = ENavLinkDirection::BothWays;

				//Need to set up the smart links location as well, since it's manual
				link->GetSmartLinkComp()->SetLinkData(link->PointLinks[0].Left, link->PointLinks[0].Right, originalDirection);

				link->SetActorHiddenInGame(false);

				link->bSmartLinkIsRelevant = true;
				link->GetSmartLinkComp()->SetNavigationRelevancy(link->bSmartLinkIsRelevant);

				navSys->UpdateActorInNavOctree(*link);

				//Attach the link to generator's actor.
				link->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

				//Add the link to _navLink array.
				_navLinks.Add(link);

				break;

			}
		}
	}

	return true;

}


#if WITH_EDITOR
void ANavigationLinkGenerator::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ANavigationLinkGenerator, _bRefreshGenerator))
	{
		_bRefreshGenerator = false;
		GenerateNavLinks();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ANavigationLinkGenerator, _bClearGenerator))
	{
		_bClearGenerator = false;

		//remove and destroy all the links of clear flag is true.
		for (ANavLinkProxy* aLink : _navLinks)
		{
			if (aLink)
			{
				aLink->Destroy();
			}
		}
		_navLinks.Empty(0);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif