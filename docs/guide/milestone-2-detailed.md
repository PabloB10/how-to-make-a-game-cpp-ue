# Milestone 2 - Detailed Implementation

**Goal:** Create interaction system with UI prompts for pickups, doors, and other interactive objects

**Estimated Time:** 4-6 hours over 2 sessions

## Overview Checklist

- [ ] Create IInteractable interface
- [ ] Implement UInteractionComponent with sphere tracing
- [ ] Create AInteractiveActor base class
- [ ] Build interaction UI prompt widget
- [ ] Test with pickup and door examples

---

## Session 1: Interface & Component System (2-3 hours)

### Step 2.1: Create IInteractable Interface (30 minutes)

**Create Interface Header:**

1. In UE: Tools → New C++ Class → Interface → Next
2. Name: `Interactable`
3. Click "Create Class"

**Replace generated content in `Source/Platformer/Interactable.h`:**

```cpp
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

	// Get the display name of this object
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionName() const;
	virtual FText GetInteractionName_Implementation() const { return FText::FromString(TEXT("Object")); }

	// Check if this object can be interacted with by the given instigator
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AActor* Instigator) const;
	virtual bool CanInteract_Implementation(AActor* Instigator) const { return true; }

	// Perform the interaction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Instigator);
	virtual void Interact_Implementation(AActor* Instigator) {}

	// Get interaction priority (higher = preferred when multiple targets available)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	int32 GetInteractionPriority() const;
	virtual int32 GetInteractionPriority_Implementation() const { return 0; }
};
```

**Implementation file `Source/Platformer/Interactable.cpp`:**

```cpp
#include "Interactable.h"

// Add required boilerplate - implementation is all in header for interfaces
```

### Step 2.2: Create UInteractionComponent (60 minutes)

**Create Component Header `Source/Platformer/InteractionComponent.h`:**

1. Tools → New C++ Class → Actor Component → Next
2. Name: `InteractionComponent`

**Replace content:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class IInteractable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFocusChanged, AActor*, NewFocus, AActor*, OldFocus);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLATFORMER_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Interaction settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "50", ClampMax = "500"))
	float InteractionRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "20", ClampMax = "100"))
	float InteractionRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// Current state
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	AActor* CurrentFocusActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	float LastInteractionTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionCooldown = 0.5f;

public:
	// Events
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFocusChanged OnFocusChanged;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetCurrentFocusActor() const { return CurrentFocusActor; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool CanInteractNow() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	FText GetCurrentInteractionText() const;

private:
	void UpdateFocusActor();
	AActor* FindBestInteractable();

	// Debug
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (ConsoleVariable = "platformer.ShowInteractionDebug"))
	bool bShowDebugTraces = false;
};
```

**Implementation `Source/Platformer/InteractionComponent.cpp`:**

```cpp
#include "InteractionComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Interactable.h"
#include "DrawDebugHelpers.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Check 10 times per second for performance
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Validate component setup
	if (!ensure(GetOwner()))
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionComponent has no owner!"));
		SetComponentTickEnabled(false);
		return;
	}
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateFocusActor();
}

void UInteractionComponent::UpdateFocusActor()
{
	AActor* NewFocusActor = FindBestInteractable();

	if (NewFocusActor != CurrentFocusActor)
	{
		AActor* OldFocus = CurrentFocusActor;
		CurrentFocusActor = NewFocusActor;

		// Broadcast focus change event
		OnFocusChanged.Broadcast(NewFocusActor, OldFocus);

		UE_LOG(LogTemp, Log, TEXT("Focus changed from %s to %s"),
			OldFocus ? *OldFocus->GetName() : TEXT("None"),
			NewFocusActor ? *NewFocusActor->GetName() : TEXT("None"));
	}
}

AActor* UInteractionComponent::FindBestInteractable()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
		return nullptr;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC || !PC->PlayerCameraManager)
		return nullptr;

	// Get camera location and forward direction
	FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	FVector ForwardDirection = PC->PlayerCameraManager->GetCameraRotation().Vector();
	FVector TraceEnd = CameraLocation + (ForwardDirection * InteractionRange);

	// Sphere trace for interactables
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPawn);
	QueryParams.bTraceComplex = false;

	TArray<FHitResult> HitResults;
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		CameraLocation,
		TraceEnd,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(InteractionRadius),
		QueryParams
	);

	// Debug visualization
	if (bShowDebugTraces)
	{
		DrawDebugSphere(GetWorld(), CameraLocation + (ForwardDirection * InteractionRange * 0.5f),
			InteractionRadius, 16, bHit ? FColor::Green : FColor::Red, false, 0.1f);
		DrawDebugLine(GetWorld(), CameraLocation, TraceEnd, FColor::Yellow, false, 0.1f, 0, 2.0f);
	}

	if (!bHit)
		return nullptr;

	// Find best interactable (highest priority, then closest to camera center)
	AActor* BestActor = nullptr;
	int32 BestPriority = -1;
	float BestDot = -1.0f;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || !HitActor->Implements<UInteractable>())
			continue;

		// Check if we can interact
		if (!IInteractable::Execute_CanInteract(HitActor, OwnerPawn))
			continue;

		// Get priority
		int32 Priority = IInteractable::Execute_GetInteractionPriority(HitActor);

		// Calculate how "centered" this actor is in our view
		FVector ToActor = (HitActor->GetActorLocation() - CameraLocation).GetSafeNormal();
		float DotProduct = FVector::DotProduct(ForwardDirection, ToActor);

		// Select best candidate (priority first, then centering)
		if (Priority > BestPriority || (Priority == BestPriority && DotProduct > BestDot))
		{
			BestPriority = Priority;
			BestDot = DotProduct;
			BestActor = HitActor;
		}
	}

	return BestActor;
}

void UInteractionComponent::TryInteract()
{
	if (!CanInteractNow())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot interact right now"));
		return;
	}

	if (CurrentFocusActor && CurrentFocusActor->Implements<UInteractable>())
	{
		LastInteractionTime = GetWorld()->GetTimeSeconds();
		IInteractable::Execute_Interact(CurrentFocusActor, GetOwner());

		UE_LOG(LogTemp, Log, TEXT("Interacted with %s"), *CurrentFocusActor->GetName());
	}
}

bool UInteractionComponent::CanInteractNow() const
{
	if (!CurrentFocusActor)
		return false;

	// Check cooldown
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastInteractionTime < InteractionCooldown)
		return false;

	// Check if target can still be interacted with
	if (CurrentFocusActor->Implements<UInteractable>())
	{
		return IInteractable::Execute_CanInteract(CurrentFocusActor, GetOwner());
	}

	return false;
}

FText UInteractionComponent::GetCurrentInteractionText() const
{
	if (!CurrentFocusActor || !CurrentFocusActor->Implements<UInteractable>())
		return FText::GetEmpty();

	FText Verb = IInteractable::Execute_GetInteractionVerb(CurrentFocusActor);
	FText Name = IInteractable::Execute_GetInteractionName(CurrentFocusActor);

	return FText::Format(FText::FromString(TEXT("{0} {1}")), Verb, Name);
}
```

### Step 2.3: Add Interaction to Character (30 minutes)

**Update `PlatformCharacter.h` to include interaction:**

```cpp
// Add to includes section
#include "InteractionComponent.h"

// Add to private members section
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
class UInteractionComponent* InteractionComponent;

// Add to input actions
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
UInputAction* InteractAction;

// Add input function
void Interact();

// Add getter
FORCEINLINE UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
```

**Update `PlatformCharacter.cpp`:**

```cpp
// In constructor, add:
InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

// In SetupPlayerInputComponent, add:
if (InteractAction)
{
    EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlatformCharacter::Interact);
}

// Add implementation:
void APlatformCharacter::Interact()
{
    if (InteractionComponent)
    {
        InteractionComponent->TryInteract();
    }
}
```

### Step 2.4: Build and Test Basic System (30 minutes)

**Compile and Test:**

1. Build → Build Solution in Visual Studio
2. Should compile successfully
3. In UE Editor, create Blueprint based on `BP_PlatformCharacter`
4. Set `InteractAction` to `IA_Interact` (create this Input Action asset)
5. Test in PIE - no visible interaction yet, but no errors

---

## Session 2: Interactive Actors & UI (2-3 hours)

### Step 2.5: Create AInteractiveActor Base Class (45 minutes)

**Create new C++ class:**

1. Tools → New C++ Class → Actor → Next
2. Name: `InteractiveActor`

**Header `Source/Platformer/InteractiveActor.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "InteractiveActor.generated.h"

UCLASS(Blueprintable)
class PLATFORMER_API AInteractiveActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AInteractiveActor();

protected:
	virtual void BeginPlay() override;

	// Interaction properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText DisplayName = FText::FromString(TEXT("Interactive Object"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionVerb = FText::FromString(TEXT("Use"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	int32 InteractionPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanInteract = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bOneTimeUse = false;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bHasBeenUsed = false;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

public:
	// IInteractable interface implementation
	virtual FText GetInteractionVerb_Implementation() const override;
	virtual FText GetInteractionName_Implementation() const override;
	virtual bool CanInteract_Implementation(AActor* Instigator) const override;
	virtual void Interact_Implementation(AActor* Instigator) override;
	virtual int32 GetInteractionPriority_Implementation() const override;

protected:
	// Blueprint events
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteracted(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void HandleInteraction(AActor* Instigator);
	virtual void HandleInteraction_Implementation(AActor* Instigator);
};
```

**Implementation `Source/Platformer/InteractiveActor.cpp`:**

```cpp
#include "InteractiveActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"

AInteractiveActor::AInteractiveActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(50.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();

	// Validate setup
	if (!ensure(CollisionComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("InteractiveActor %s has no collision component!"), *GetName());
	}
}

FText AInteractiveActor::GetInteractionVerb_Implementation() const
{
	return InteractionVerb;
}

FText AInteractiveActor::GetInteractionName_Implementation() const
{
	return DisplayName;
}

bool AInteractiveActor::CanInteract_Implementation(AActor* Instigator) const
{
	if (!bCanInteract)
		return false;

	if (bOneTimeUse && bHasBeenUsed)
		return false;

	return true;
}

void AInteractiveActor::Interact_Implementation(AActor* Instigator)
{
	if (!CanInteract_Implementation(Instigator))
		return;

	// Mark as used if one-time
	if (bOneTimeUse)
	{
		bHasBeenUsed = true;
	}

	// Handle the interaction
	HandleInteraction(Instigator);

	// Call Blueprint event
	OnInteracted(Instigator);

	UE_LOG(LogTemp, Log, TEXT("%s interacted with %s"),
		Instigator ? *Instigator->GetName() : TEXT("Unknown"),
		*GetName());
}

int32 AInteractiveActor::GetInteractionPriority_Implementation() const
{
	return InteractionPriority;
}

void AInteractiveActor::HandleInteraction_Implementation(AActor* Instigator)
{
	// Default implementation - show message
	if (GEngine)
	{
		FString Message = FString::Printf(TEXT("Interacted with %s"), *DisplayName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, Message);
	}
}
```

### Step 2.6: Create Interaction UI Widget (60 minutes)

**Create Widget Blueprint:**

1. Content Browser → Add → User Interface → Widget Blueprint
2. Name: `WBP_InteractionPrompt`
3. Open the widget

**Widget Structure in Blueprint:**

1. Add Canvas Panel as root
2. Add Border component with:
   - Background color: Semi-transparent black (0,0,0,0.7)
   - Padding: 8,8,8,8
3. Add Text Block inside Border:
   - Name: `InteractionText`
   - Font size: 18
   - Color: White
   - Text: "Interact Object"

**Create C++ Widget Component for easier integration:**

Create `Source/Platformer/InteractionPromptWidget.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionPromptWidget.generated.h"

UCLASS()
class PLATFORMER_API UInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractionText;

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionText(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ShowPrompt(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HidePrompt();
};
```

**Implementation `Source/Platformer/InteractionPromptWidget.cpp`:**

```cpp
#include "InteractionPromptWidget.h"
#include "Components/TextBlock.h"
#include "Animation/UMGSequencePlayer.h"

void UInteractionPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initially hidden
	SetVisibility(ESlateVisibility::Hidden);
}

void UInteractionPromptWidget::SetInteractionText(const FText& Text)
{
	if (InteractionText)
	{
		InteractionText->SetText(Text);
	}
}

void UInteractionPromptWidget::ShowPrompt(const FText& Text)
{
	SetInteractionText(Text);
	SetVisibility(ESlateVisibility::Visible);

	// TODO: Add fade-in animation if desired
}

void UInteractionPromptWidget::HidePrompt()
{
	SetVisibility(ESlateVisibility::Hidden);

	// TODO: Add fade-out animation if desired
}
```

### Step 2.7: Integrate UI with Interaction System (45 minutes)

**Update Character to manage UI:**

Add to `PlatformCharacter.h`:

```cpp
// Add include
#include "InteractionPromptWidget.h"

// Add to protected members
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
TSubclassOf<UInteractionPromptWidget> InteractionPromptWidgetClass;

UPROPERTY()
UInteractionPromptWidget* InteractionPromptWidget;

// Add functions
UFUNCTION()
void OnInteractionFocusChanged(AActor* NewFocus, AActor* OldFocus);
```

**Update `PlatformCharacter.cpp`:**

```cpp
// In BeginPlay, add:
// Bind to interaction component events
if (InteractionComponent)
{
    InteractionComponent->OnFocusChanged.AddDynamic(this, &APlatformCharacter::OnInteractionFocusChanged);
}

// Create interaction UI
if (InteractionPromptWidgetClass)
{
    InteractionPromptWidget = CreateWidget<UInteractionPromptWidget>(GetWorld(), InteractionPromptWidgetClass);
    if (InteractionPromptWidget)
    {
        InteractionPromptWidget->AddToViewport();
    }
}

// Add implementation:
void APlatformCharacter::OnInteractionFocusChanged(AActor* NewFocus, AActor* OldFocus)
{
    if (!InteractionPromptWidget)
        return;

    if (NewFocus && NewFocus->Implements<UInteractable>())
    {
        FText InteractionText = InteractionComponent->GetCurrentInteractionText();
        InteractionPromptWidget->ShowPrompt(InteractionText);
    }
    else
    {
        InteractionPromptWidget->HidePrompt();
    }
}
```

### Step 2.8: Create Example Interactive Objects (30 minutes)

**Create Pickup Example:**

Create Blueprint class based on `AInteractiveActor`:

1. Name: `BP_Pickup`
2. Set properties:
   - Display Name: "Health Potion"
   - Interaction Verb: "Pickup"
   - One Time Use: true
3. Set mesh to a simple cube or sphere
4. Implement `OnInteracted` event to:
   - Play pickup sound (optional)
   - Add particle effect (optional)
   - Destroy actor after delay

**Create Door Example:**

Create Blueprint class based on `AInteractiveActor`:

1. Name: `BP_Door`
2. Set properties:
   - Display Name: "Door"
   - Interaction Verb: "Open"
   - One Time Use: false
3. Set mesh to door shape
4. Implement `OnInteracted` event to:
   - Toggle door open/closed state
   - Play door animation
   - Change interaction verb between "Open"/"Close"

### Step 2.9: Final Testing & Validation (30 minutes)

**Test Setup:**

1. Place `BP_Pickup` and `BP_Door` in test level
2. Ensure `BP_PlatformCharacter` has:
   - `InteractionPromptWidgetClass` set to `WBP_InteractionPrompt`
   - `InteractAction` set to appropriate Input Action
3. Configure Input Mapping Context with Interact action (E key)

**Acceptance Criteria Verification:**

- [ ] Prompt appears when looking at interactive objects ✅
- [ ] Prompt disappears when looking away ✅
- [ ] Prompt shows correct verb and object name ✅
- [ ] E key triggers interaction ✅
- [ ] Pickup disappears after interaction ✅
- [ ] Door can be opened/closed repeatedly ✅
- [ ] Multiple objects prioritize correctly ✅
- [ ] No errors in Output Log ✅

**Common Issues & Solutions:**

**Prompt doesn't appear:**

- Check `WBP_InteractionPrompt` is assigned in character Blueprint
- Verify collision setup on interactive objects
- Check trace channel settings match collision responses

**Interaction doesn't work:**

- Verify `IA_Interact` is created and bound in Input Mapping Context
- Check `InteractAction` is set in character Blueprint
- Ensure interactive objects implement interface correctly

**Performance issues:**

- Reduce interaction component tick frequency if needed
- Limit number of interactive objects in range
- Use object pooling for frequently spawned pickups

## Next Steps

Once Milestone 2 is complete:

1. Commit your code to Git
2. Test with different object types and configurations
3. Begin [Milestone 3: Stats + Damage + Enemy AI](/guide/milestone-3-detailed)

**Git Commit Message Example:**

```
Milestone 2: Implement interaction system with UI prompts

- Create IInteractable interface for consistent interaction contracts
- Add UInteractionComponent with sphere tracing and focus management
- Build AInteractiveActor base class for pickups and doors
- Integrate interaction UI prompt widget with fade in/out
- Add pickup and door examples with proper state management
```
