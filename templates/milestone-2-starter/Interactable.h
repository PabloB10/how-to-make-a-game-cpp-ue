#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class PLATFORMER_API IInteractable
{
	GENERATED_BODY()

public:
	// Get the verb to display in UI (e.g., "Open", "Pickup", "Talk")
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionVerb() const;
	virtual FText GetInteractionVerb_Implementation() const { return FText::FromString(TEXT("Interact")); }

	// Get the name of the object for UI display
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionName() const;
	virtual FText GetInteractionName_Implementation() const { return FText::FromString(TEXT("Object")); }

	// Check if interaction is currently available
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AActor* Interactor) const;
	virtual bool CanInteract_Implementation(AActor* Interactor) const { return true; }

	// Perform the interaction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool Interact(AActor* Interactor);
	virtual bool Interact_Implementation(AActor* Interactor) { return true; }

	// Get interaction priority (higher = more important when multiple objects in range)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	int32 GetInteractionPriority() const;
	virtual int32 GetInteractionPriority_Implementation() const { return 0; }
};
