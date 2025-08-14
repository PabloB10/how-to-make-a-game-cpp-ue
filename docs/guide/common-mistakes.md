# Common Mistakes

## Memory Management Pitfalls

### Mistake: Holding Raw Pointers to UObjects

**❌ Wrong:**

```cpp
class UMyComponent : public UActorComponent
{
private:
    AActor* CachedActor; // Raw pointer - dangerous!

public:
    void SetCachedActor(AActor* Actor) { CachedActor = Actor; }

    void UseCachedActor()
    {
        // CRASH: Actor might have been garbage collected
        CachedActor->DoSomething();
    }
};
```

**✅ Right:**

```cpp
class UMyComponent : public UActorComponent
{
private:
    // Use UPROPERTY to prevent GC, or TWeakObjectPtr for non-owning references
    UPROPERTY()
    AActor* OwnedActor; // GC will not delete this

    TWeakObjectPtr<AActor> WeakCachedActor; // Safe weak reference

public:
    void SetCachedActor(AActor* Actor)
    {
        WeakCachedActor = Actor;
    }

    void UseCachedActor()
    {
        if (WeakCachedActor.IsValid())
        {
            WeakCachedActor->DoSomething();
        }
    }
};
```

### Mistake: Not Using UPROPERTY for UObject References

**❌ Wrong:**

```cpp
class APlatformCharacter : public ACharacter
{
private:
    UInputMappingContext* InputContext; // Will be garbage collected!
};
```

**✅ Right:**

```cpp
class APlatformCharacter : public ACharacter
{
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputMappingContext* InputContext; // Protected from GC
};
```

## Component Lifecycle Issues

### Mistake: Accessing Components Before They're Ready

**❌ Wrong:**

```cpp
APlatformCharacter::APlatformCharacter()
{
    // Constructor is too early for some operations
    GetCharacterMovement()->SetMovementMode(MOVE_Flying); // Might not work reliably

    // Accessing player controller here will fail
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        // GetController() returns nullptr in constructor
    }
}
```

**✅ Right:**

```cpp
APlatformCharacter::APlatformCharacter()
{
    // Only create components and set basic properties in constructor
    GetCharacterMovement()->MaxWalkSpeed = 600.0f; // This is fine
}

void APlatformCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Access controllers and world context in BeginPlay
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        // Now GetController() works properly
    }
}
```

### Mistake: Calling Parent Functions at Wrong Time

**❌ Wrong:**

```cpp
void AMyActor::BeginPlay()
{
    // Setup that depends on parent initialization
    MyCustomSetup();

    // Calling Super::BeginPlay() last can break things
    Super::BeginPlay();
}
```

**✅ Right:**

```cpp
void AMyActor::BeginPlay()
{
    // Always call parent first
    Super::BeginPlay();

    // Then do your custom setup
    MyCustomSetup();
}
```

## Input Handling Edge Cases

### Mistake: Not Handling Input Action Nulls

**❌ Wrong:**

```cpp
void APlatformCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

    // Crash if MoveAction is null (not set in Blueprint)
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlatformCharacter::Move);
}
```

**✅ Right:**

```cpp
void APlatformCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Check each action before binding
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlatformCharacter::Move);
        }

        if (JumpAction)
        {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
        }
    }
}
```

### Mistake: Forgetting to Add Input Mapping Context

**❌ Wrong:**

```cpp
void APlatformCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Input actions are bound but context is never added
    // Result: Input doesn't work
}
```

**✅ Right:**

```cpp
void APlatformCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}
```

## AI Behavior Tree Debugging

### Mistake: Blackboard Key Type Mismatches

**❌ Wrong:**

```cpp
// In Behavior Tree, key "TargetLocation" is set as Object type
// But trying to use it as Vector
void UMyBTTask::ExecuteTask()
{
    // This will return invalid data - key type mismatch
    FVector TargetLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("TargetLocation"));
}
```

**✅ Right:**

```cpp
// Ensure Blackboard key types match your usage
void UMyBTTask::ExecuteTask()
{
    // If key is Object type, get it as object first
    if (AActor* TargetActor = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor"))))
    {
        FVector TargetLocation = TargetActor->GetActorLocation();
    }

    // Or if key is Vector type, use GetValueAsVector
    FVector DirectLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("TargetLocation"));
}
```

### Mistake: Not Checking AI Controller Validity

**❌ Wrong:**

```cpp
void AEnemyNPC::BeginPlay()
{
    Super::BeginPlay();

    // Assuming AIController exists
    GetAIController()->RunBehaviorTree(BehaviorTree); // Crash if no AIController set
}
```

**✅ Right:**

```cpp
void AEnemyNPC::BeginPlay()
{
    Super::BeginPlay();

    if (AAIController* AIController = GetAIController())
    {
        if (BehaviorTree)
        {
            AIController->RunBehaviorTree(BehaviorTree);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AEnemyNPC has no AIController! Check Pawn settings."));
    }
}
```

## Interface Implementation Issues

### Mistake: Not Implementing Interface Functions Properly

**❌ Wrong:**

```cpp
// Interface declared with BlueprintImplementableEvent
UCLASS()
class AMyActor : public AActor, public IInteractable
{
    GENERATED_BODY()

    // Missing implementation - will cause linker errors if called from C++
};
```

**✅ Right:**

```cpp
// For C++ implementation, use BlueprintNativeEvent or regular virtual functions
UINTERFACE(BlueprintType)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class IInteractable
{
    GENERATED_BODY()

public:
    // Can be implemented in C++ or Blueprint
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(AActor* Instigator);
    virtual void Interact_Implementation(AActor* Instigator) {}

    // Or pure virtual for C++ only
    virtual FText GetInteractionText() const = 0;
};

// Implementation
void AMyActor::Interact_Implementation(AActor* Instigator)
{
    // C++ implementation here
    UE_LOG(LogTemp, Log, TEXT("Interacted with %s"), *GetName());
}
```

### Mistake: Incorrect Interface Checking

**❌ Wrong:**

```cpp
void UInteractionComponent::TryInteract(AActor* Target)
{
    // Wrong way to check interface
    if (IInteractable* Interactable = Cast<IInteractable>(Target))
    {
        // This cast will always fail - interfaces aren't UObjects
    }
}
```

**✅ Right:**

```cpp
void UInteractionComponent::TryInteract(AActor* Target)
{
    // Correct way to check interface implementation
    if (Target && Target->Implements<UInteractable>())
    {
        // Execute interface function
        IInteractable::Execute_Interact(Target, GetOwner());

        // Or call directly if using virtual functions
        if (IInteractable* Interactable = Cast<IInteractable>(Target))
        {
            Interactable->GetInteractionText();
        }
    }
}
```

## Blueprint Integration Problems

### Mistake: Forgetting UFUNCTION Specifiers

**❌ Wrong:**

```cpp
class UStatsComponent : public UActorComponent
{
public:
    // Not accessible from Blueprint
    void TakeDamage(float Damage);

    // Not visible in Blueprint editor
    float Health = 100.0f;
};
```

**✅ Right:**

```cpp
class UStatsComponent : public UActorComponent
{
public:
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void TakeDamage(float Damage);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Health = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float MaxHealth = 100.0f;
};
```

### Mistake: Incorrect UPROPERTY Usage

**❌ Wrong:**

```cpp
class AMyActor : public AActor
{
    // Can't edit in Blueprint because of const
    UPROPERTY(EditAnywhere)
    const float Speed = 600.0f;

    // Can't see value changes in Blueprint
    UPROPERTY(EditAnywhere)
    bool bIsRunning;

    // Not protected from garbage collection
    AActor* CachedReference;
};
```

**✅ Right:**

```cpp
class AMyActor : public AActor
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Speed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsRunning = false;

    UPROPERTY(BlueprintReadOnly, Category = "References")
    AActor* CachedReference = nullptr;
};
```

## Performance Anti-Patterns

### Mistake: Expensive Operations in Tick

**❌ Wrong:**

```cpp
void UMyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // Expensive operations every frame
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    for (AActor* Actor : AllActors)
    {
        // Complex calculations for every actor every frame
        float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Actor->GetActorLocation());
        // ... more expensive work
    }
}
```

**✅ Right:**

```cpp
void UMyComponent::BeginPlay()
{
    Super::BeginPlay();

    // Use timer for periodic updates instead of tick
    GetWorld()->GetTimerManager().SetTimer(
        UpdateTimerHandle,
        this,
        &UMyComponent::PeriodicUpdate,
        0.2f, // Every 200ms instead of every frame (16ms)
        true
    );
}

void UMyComponent::PeriodicUpdate()
{
    // Same logic but much less frequent
    // Cache results for use between updates
}
```

### Mistake: Creating Objects Every Frame

**❌ Wrong:**

```cpp
void UMyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // New array allocation every frame
    TArray<FHitResult> HitResults;

    // New string creation every frame
    FString DebugMessage = FString::Printf(TEXT("Health: %f"), CurrentHealth);
}
```

**✅ Right:**

```cpp
class UMyComponent : public UActorComponent
{
private:
    // Reuse containers
    TArray<FHitResult> CachedHitResults;

public:
    void PeriodicUpdate()
    {
        // Clear but don't deallocate
        CachedHitResults.Reset();

        // Use cached array
        // ... fill CachedHitResults
    }
};
```

## Testing and Validation Mistakes

### Mistake: Not Validating Asset References

**❌ Wrong:**

```cpp
void APlatformCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Assuming assets are always set
    GetAIController()->RunBehaviorTree(BehaviorTree); // Crash if BehaviorTree is null
}
```

**✅ Right:**

```cpp
void APlatformCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Validate all required assets
    if (!ensure(BehaviorTree))
    {
        UE_LOG(LogTemp, Error, TEXT("BehaviorTree not set on %s"), *GetName());
        return;
    }

    if (AAIController* AIController = GetAIController())
    {
        AIController->RunBehaviorTree(BehaviorTree);
    }
}
```

### Mistake: Not Testing Edge Cases

**❌ Wrong:**

```cpp
void UInteractionComponent::TryInteract()
{
    // Assuming CurrentFocusActor is always valid
    IInteractable::Execute_Interact(CurrentFocusActor, GetOwner());
}
```

**✅ Right:**

```cpp
void UInteractionComponent::TryInteract()
{
    // Check all conditions
    if (!CurrentFocusActor)
    {
        UE_LOG(LogInteraction, Warning, TEXT("No focus actor to interact with"));
        return;
    }

    if (!CurrentFocusActor->Implements<UInteractable>())
    {
        UE_LOG(LogInteraction, Warning, TEXT("Focus actor %s doesn't implement IInteractable"), *CurrentFocusActor->GetName());
        return;
    }

    if (!IInteractable::Execute_CanInteract(CurrentFocusActor, GetOwner()))
    {
        UE_LOG(LogInteraction, Log, TEXT("Cannot interact with %s right now"), *CurrentFocusActor->GetName());
        return;
    }

    IInteractable::Execute_Interact(CurrentFocusActor, GetOwner());
}
```

## Prevention Strategies

### Use Static Analysis Tools

- Enable all warnings in your compiler settings
- Use PVS-Studio or similar static analysis tools
- Review code regularly for these patterns

### Defensive Programming

```cpp
// Always validate inputs
void UMyComponent::SetTarget(AActor* NewTarget)
{
    if (!ensure(IsValid(NewTarget)))
    {
        return;
    }

    Target = NewTarget;
}

// Use const correctness
void UMyComponent::ProcessActor(const AActor* Actor) const
{
    // Function can't modify Actor or component state
}
```

### Regular Testing

- Test with different input combinations
- Test with missing/null assets
- Test with destroyed objects
- Test in packaged builds, not just PIE

Remember: Most of these mistakes are caught by following UE5's established patterns and using the provided safety mechanisms like ensure(), IsValid(), and proper UPROPERTY usage.
