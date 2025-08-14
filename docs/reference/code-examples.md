# Code Examples

Complete implementation examples for the platformer system components.

## Character Class (APlatformCharacter)

### Header Declaration

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlatformCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterTuningData;

UCLASS()
class PLATFORMER_API APlatformCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlatformCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InteractAction;

	// Tuning Data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Tuning)
	UCharacterTuningData* TuningData;

	// Input Functions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprint();
	void StopSprint();
	void Interact();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	bool bIsSprinting = false;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
```

### Constructor Implementation

```cpp
APlatformCharacter::APlatformCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character rotation (don't rotate with controller for platformer)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->JumpMaxCount = 2; // Double jump

	// Create camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	// Create follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}
```

## Interaction System

### IInteractable Interface

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
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionVerb() const;

	// Get the display name of this object
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionName() const;

	// Check if this object can be interacted with by the given instigator
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AActor* Instigator) const;

	// Perform the interaction
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Instigator);
};
```

### UInteractionComponent

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class IInteractable;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLATFORMER_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionRadius = 60.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	AActor* CurrentFocusActor = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetCurrentFocusActor() const { return CurrentFocusActor; }

private:
	void UpdateFocusActor();
	AActor* FindBestInteractable();
};
```

### Interaction Component Implementation

```cpp
#include "InteractionComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Interactable.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Check 10 times per second
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
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
		CurrentFocusActor = NewFocusActor;
		// Trigger UI update event here
	}
}

AActor* UInteractionComponent::FindBestInteractable()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
		return nullptr;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
		return nullptr;

	// Get camera location and forward direction
	FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	FVector ForwardDirection = PC->PlayerCameraManager->GetCameraRotation().Vector();

	// Sphere trace for interactables
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPawn);

	TArray<FHitResult> HitResults;
	FVector TraceEnd = CameraLocation + (ForwardDirection * InteractionRange);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		CameraLocation,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(InteractionRadius),
		QueryParams
	);

	if (!bHit)
		return nullptr;

	// Find best interactable (closest to camera center)
	AActor* BestActor = nullptr;
	float BestDot = -1.0f;

	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor() && Hit.GetActor()->Implements<UInteractable>())
		{
			// Check if we can interact
			if (IInteractable::Execute_CanInteract(Hit.GetActor(), OwnerPawn))
			{
				// Calculate how "centered" this actor is in our view
				FVector ToActor = (Hit.GetActor()->GetActorLocation() - CameraLocation).GetSafeNormal();
				float DotProduct = FVector::DotProduct(ForwardDirection, ToActor);

				if (DotProduct > BestDot)
				{
					BestDot = DotProduct;
					BestActor = Hit.GetActor();
				}
			}
		}
	}

	return BestActor;
}

void UInteractionComponent::TryInteract()
{
	if (CurrentFocusActor && CurrentFocusActor->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(CurrentFocusActor, GetOwner());
	}
}
```

## Stats Component

### UStatsComponent Header

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth, float, HealthPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeath, AActor*, Killer, AActor*, Victim);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLATFORMER_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float HealthRegenRate = 0.0f; // Per second

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float HealthRegenDelay = 3.0f; // Seconds after taking damage

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float CurrentStamina;

private:
	FTimerHandle HealthRegenTimerHandle;
	float LastDamageTime = 0.0f;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeath OnDeath;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float ApplyDamage(float DamageAmount, AActor* DamageCauser = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RestoreHealth(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool IsAlive() const { return CurrentHealth > 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetHealthPercentage() const { return MaxHealth > 0 ? CurrentHealth / MaxHealth : 0.0f; }

private:
	void StartHealthRegen();
	void TickHealthRegen();
};
```

## Data Assets

### Character Tuning Data

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterTuningData.generated.h"

UCLASS(BlueprintType)
class PLATFORMER_API UCharacterTuningData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Movement settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "100", ClampMax = "1000"))
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "200", ClampMax = "1500"))
	float SprintSpeed = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "300", ClampMax = "1200"))
	float JumpZVelocity = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float AirControl = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "1", ClampMax = "3"))
	int32 JumpMaxCount = 2;

	// Camera settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (ClampMin = "200", ClampMax = "800"))
	float CameraDistance = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (ClampMin = "1", ClampMax = "25"))
	float CameraLagSpeed = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (ClampMin = "1", ClampMax = "25"))
	float CameraRotationLagSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (ClampMin = "-45", ClampMax = "45"))
	float CameraAngle = -15.0f;
};
```

## Enhanced Input Setup

### Input Action Creation (Blueprint)

```cpp
// This would be created in Blueprint, but here's how to reference them in C++

// In your character's header:
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
UInputAction* MoveAction;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
UInputAction* LookAction;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
UInputAction* JumpAction;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
UInputAction* SprintAction;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
UInputAction* InteractAction;

// In SetupPlayerInputComponent:
if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
{
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlatformCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlatformCharacter::Look);

	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlatformCharacter::StartSprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlatformCharacter::StopSprint);

	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlatformCharacter::Interact);
}
```

## Common Patterns

### Safe Casting Pattern

```cpp
// Always use safe casting for UObjects
if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
{
	if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
	{
		// Safe to use PC here
	}
}
```

### Component Access Pattern

```cpp
// Safe component access
if (UStatsComponent* StatsComp = GetOwner()->FindComponentByClass<UStatsComponent>())
{
	StatsComp->ApplyDamage(DamageAmount);
}
```

### Interface Implementation Check

```cpp
// Check if actor implements interface
if (TargetActor && TargetActor->Implements<UInteractable>())
{
	// Safe to call interface functions
	IInteractable::Execute_Interact(TargetActor, this);
}
```

### Timer Usage Pattern

```cpp
// Setting up a timer
FTimerHandle TimerHandle;
GetWorld()->GetTimerManager().SetTimer(
	TimerHandle,
	this,
	&UMyComponent::MyTimerFunction,
	1.0f,  // Delay
	true   // Looping
);

// Clearing a timer
GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
```

These examples provide the foundation for implementing all the milestone features. Each example includes proper UE5 patterns, memory safety, and Blueprint integration points.

This page provides minimal C++ code snippets for key concepts.

## Enhanced Input Setup

Input Action Asset setup (in editor):

- Create Input Action `IA_Move` (Value Type: Axis2D)
- Create Input Action `IA_Jump` (Value Type: Digital Bool)
- Create Input Mapping Context `IMC_Player`

Character header snippet:

```cpp
UCLASS()
class PLATFORMER_API APlatformCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlatformCharacter();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class UCameraComponent* FollowCamera;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* JumpAction;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void Move(const FInputActionValue& Value);
    void StartJump();
    void StopJump();
};
```

## Interface Example

IInteractable interface:

```cpp
UINTERFACE(MinimalAPI, BlueprintType)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class PLATFORMER_API IInteractable
{
    GENERATED_BODY()

public:
    virtual FText GetInteractionVerb() const = 0;
    virtual bool CanInteract(AActor* Instigator) const = 0;
    virtual void Interact(AActor* Instigator) = 0;
};
```

## Data Asset Example

Character tuning data asset:

```cpp
UCLASS()
class PLATFORMER_API UCharacterTuningData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float WalkSpeed = 600.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float SprintSpeed = 900.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float AirControl = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    int32 JumpMaxCount = 2;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
```

## Component Example

Stats component header:

```cpp
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLATFORMER_API UStatsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStatsComponent();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeath, AActor*, Victim, AActor*, Killer);

    UPROPERTY(BlueprintAssignable)
    FOnDeath OnDeath;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float HealthMax = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float Health = 100.0f;

public:
    UFUNCTION(BlueprintCallable, Category = "Stats")
    float ApplyDamage(float DamageAmount, AActor* DamageCauser);

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetHealthPercent() const { return Health / HealthMax; }
};
```
