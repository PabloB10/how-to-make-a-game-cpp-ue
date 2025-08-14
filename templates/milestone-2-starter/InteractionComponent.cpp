#include "InteractionComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Check for interactions 10 times per second
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateInteraction();
}

void UInteractionComponent::UpdateInteraction()
{
	AActor* BestInteractable = FindBestInteractable();
	
	if (BestInteractable != CurrentInteractable)
	{
		SetCurrentInteractable(BestInteractable);
	}
}

AActor* UInteractionComponent::FindBestInteractable()
{
	if (!GetOwner())
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FVector StartLocation = GetOwner()->GetActorLocation();
	const FVector EndLocation = StartLocation + (GetOwner()->GetActorForwardVector() * InteractionRange);

	// Perform sphere trace
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = false;

	TArray<FHitResult> HitResults;
	const bool bHit = World->SweepMultiByChannel(
		HitResults,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		InteractionChannel,
		FCollisionShape::MakeSphere(InteractionRange),
		QueryParams
	);

	// Debug drawing
	if (bShowDebugTrace)
	{
		DrawDebugSphere(World, StartLocation, InteractionRange, 12, 
			bHit ? FColor::Green : FColor::Red, false, 0.1f);
	}

	if (!bHit)
	{
		return nullptr;
	}

	// Find the best interactable based on priority and distance
	AActor* BestActor = nullptr;
	float ClosestDistance = FLT_MAX;
	int32 HighestPriority = -1;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || !HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			continue;
		}

		// Check if we can interact with this actor
		if (!IInteractable::Execute_CanInteract(HitActor, GetOwner()))
		{
			continue;
		}

		// Get priority and distance
		const int32 Priority = IInteractable::Execute_GetInteractionPriority(HitActor);
		const float Distance = FVector::Dist(StartLocation, HitActor->GetActorLocation());

		// Select based on priority first, then distance
		bool bIsBetter = false;
		if (Priority > HighestPriority)
		{
			bIsBetter = true;
		}
		else if (Priority == HighestPriority && Distance < ClosestDistance)
		{
			bIsBetter = true;
		}

		if (bIsBetter)
		{
			BestActor = HitActor;
			ClosestDistance = Distance;
			HighestPriority = Priority;
		}
	}

	return BestActor;
}

void UInteractionComponent::SetCurrentInteractable(AActor* NewInteractable)
{
	const bool bCanInteract = NewInteractable && IInteractable::Execute_CanInteract(NewInteractable, GetOwner());
	
	CurrentInteractable = NewInteractable;
	OnInteractionChanged.Broadcast(CurrentInteractable, bCanInteract);
}

bool UInteractionComponent::TryInteract()
{
	if (!CanInteractWithTarget())
	{
		return false;
	}

	OnInteractionStarted.Broadcast(CurrentInteractable, GetOwner());
	
	const bool bSuccess = IInteractable::Execute_Interact(CurrentInteractable, GetOwner());
	
	if (bSuccess)
	{
		OnInteractionCompleted.Broadcast(CurrentInteractable, GetOwner());
	}

	return bSuccess;
}

bool UInteractionComponent::CanInteractWithTarget() const
{
	if (!CurrentInteractable)
	{
		return false;
	}

	return IInteractable::Execute_CanInteract(CurrentInteractable, GetOwner());
}

void UInteractionComponent::SetInteractionRange(float NewRange)
{
	InteractionRange = FMath::Clamp(NewRange, 50.0f, 500.0f);
}
