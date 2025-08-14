# Error Handling & Validation Best Practices

## Philosophy

- **Fail Fast**: Detect errors early in development
- **Graceful Degradation**: Handle runtime errors without crashing
- **Clear Messaging**: Provide actionable error information
- **Defensive Programming**: Validate inputs and state consistently
- **Performance Conscious**: Balance safety with performance

---

## UE5 Error Handling Mechanisms

### 1. Compile-Time Validation

```cpp
// Use static_assert for compile-time validation
static_assert(sizeof(FVector) == 12, "FVector size assumption invalid");

// Use UPROPERTY meta tags for editor validation
UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1000.0"))
float Speed = 600.0f;

UPROPERTY(EditAnywhere, meta = (RequiredAssetCheck = "true"))
UInputAction* JumpAction = nullptr;
```

### 2. Runtime Validation with ensure()

```cpp
// Use ensure() for recoverable errors (development builds only)
void APlatformCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Recoverable - can continue without this asset
    if (!ensure(DefaultMappingContext))
    {
        UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext not set on %s"), *GetName());
        return; // Early return, but don't crash
    }

    // Multiple validations with specific messages
    ensureMsgf(JumpAction, TEXT("JumpAction not configured on %s - jumping disabled"), *GetName());
    ensureMsgf(MoveAction, TEXT("MoveAction not configured on %s - movement disabled"), *GetName());
}
```

### 3. Fatal Errors with check()

```cpp
// Use check() only for truly unrecoverable errors
void UInteractionComponent::TryInteract()
{
    // This should never happen if properly initialized
    check(GetOwner()); // Will crash if false - use sparingly!

    // Better approach - validate and handle gracefully
    if (!GetOwner())
    {
        UE_LOG(LogInteraction, Fatal, TEXT("InteractionComponent has no owner!"));
        return;
    }
}
```

### 4. Logging Levels

```cpp
// Define custom log categories
DECLARE_LOG_CATEGORY_EXTERN(LogPlatformer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogAI, Log, All);

// Use appropriate log levels
UE_LOG(LogPlatformer, Fatal, TEXT("Critical error - game cannot continue"));
UE_LOG(LogPlatformer, Error, TEXT("Error occurred but game can continue"));
UE_LOG(LogPlatformer, Warning, TEXT("Unexpected situation but not an error"));
UE_LOG(LogPlatformer, Log, TEXT("Normal operation info"));
UE_LOG(LogPlatformer, Verbose, TEXT("Detailed debug information"));
UE_LOG(LogPlatformer, VeryVerbose, TEXT("Extremely detailed debug info"));
```

---

## Common Error Patterns & Solutions

### 1. Null Pointer Access

**❌ Dangerous:**

```cpp
void UMyComponent::UpdateTarget()
{
    // Dangerous - could crash
    TargetActor->SetActorLocation(NewLocation);
    GetOwner()->GetActorLocation(); // Could be null
}
```

**✅ Safe:**

```cpp
void UMyComponent::UpdateTarget()
{
    // Validate before use
    if (!IsValid(TargetActor))
    {
        UE_LOG(LogPlatformer, Warning, TEXT("TargetActor is invalid"));
        return;
    }

    if (!ensure(GetOwner()))
    {
        UE_LOG(LogPlatformer, Error, TEXT("Component has no owner"));
        return;
    }

    TargetActor->SetActorLocation(NewLocation);
    FVector OwnerLocation = GetOwner()->GetActorLocation();
}
```

### 2. Array Bounds Checking

**❌ Dangerous:**

```cpp
void ProcessItems(const TArray<FItem>& Items)
{
    // Could crash if array is empty
    FItem FirstItem = Items[0];

    // Could crash if index is invalid
    FItem SpecificItem = Items[DesiredIndex];
}
```

**✅ Safe:**

```cpp
void ProcessItems(const TArray<FItem>& Items)
{
    // Check array size before access
    if (Items.Num() == 0)
    {
        UE_LOG(LogPlatformer, Warning, TEXT("Items array is empty"));
        return;
    }

    FItem FirstItem = Items[0];

    // Use IsValidIndex for bounds checking
    if (Items.IsValidIndex(DesiredIndex))
    {
        FItem SpecificItem = Items[DesiredIndex];
    }
    else
    {
        UE_LOG(LogPlatformer, Error, TEXT("Invalid index %d for array of size %d"),
            DesiredIndex, Items.Num());
    }
}
```

### 3. Asset Reference Validation

**❌ Risky:**

```cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    // Assumes asset is always set
    GetAIController()->RunBehaviorTree(BehaviorTreeAsset);
}
```

**✅ Robust:**

```cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    // Validate AI controller exists
    AAIController* AIController = GetAIController();
    if (!AIController)
    {
        UE_LOG(LogAI, Warning, TEXT("%s has no AI Controller"), *GetName());
        return;
    }

    // Validate behavior tree asset
    if (!BehaviorTreeAsset)
    {
        UE_LOG(LogAI, Error, TEXT("BehaviorTreeAsset not set on %s"), *GetName());
        return;
    }

    // Only proceed if all validations pass
    AIController->RunBehaviorTree(BehaviorTreeAsset);
    UE_LOG(LogAI, Log, TEXT("Started behavior tree for %s"), *GetName());
}
```

### 4. Input Validation

**❌ Fragile:**

```cpp
void UInteractionComponent::SetInteractionRange(float NewRange)
{
    InteractionRange = NewRange; // Could be negative or extremely large
}
```

**✅ Validated:**

```cpp
void UInteractionComponent::SetInteractionRange(float NewRange)
{
    // Validate input parameters
    if (NewRange < 0.0f)
    {
        UE_LOG(LogInteraction, Warning, TEXT("Negative interaction range not allowed: %.2f"), NewRange);
        return;
    }

    if (NewRange > 10000.0f) // Reasonable maximum
    {
        UE_LOG(LogInteraction, Warning, TEXT("Interaction range too large: %.2f, clamping to 10000"), NewRange);
        NewRange = 10000.0f;
    }

    InteractionRange = NewRange;
    UE_LOG(LogInteraction, VeryVerbose, TEXT("Interaction range set to %.2f"), InteractionRange);
}
```

---

## Error Handling by System

### Character Movement Errors

```cpp
void APlatformCharacter::StartSprint()
{
    // Validate movement component
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (!ensure(MovementComp))
    {
        UE_LOG(LogPlatformer, Error, TEXT("No CharacterMovement component on %s"), *GetName());
        return;
    }

    // Validate we're not already sprinting
    if (bIsSprinting)
    {
        UE_LOG(LogPlatformer, VeryVerbose, TEXT("Already sprinting on %s"), *GetName());
        return;
    }

    // Validate we can sprint (on ground, not disabled, etc.)
    if (!CanSprint())
    {
        UE_LOG(LogPlatformer, Log, TEXT("Cannot sprint: conditions not met for %s"), *GetName());
        return;
    }

    bIsSprinting = true;
    MovementComp->MaxWalkSpeed = SprintSpeed;
    UE_LOG(LogPlatformer, Log, TEXT("Sprint started on %s"), *GetName());
}

bool APlatformCharacter::CanSprint() const
{
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (!MovementComp)
        return false;

    // Can't sprint if not on ground
    if (!MovementComp->IsMovingOnGround())
        return false;

    // Can't sprint if no input
    if (GetInputAxisValue("Move").IsNearlyZero())
        return false;

    return true;
}
```

### Interaction System Errors

```cpp
void UInteractionComponent::TryInteract()
{
    // Validate component state
    if (!IsComponentTickEnabled())
    {
        UE_LOG(LogInteraction, Warning, TEXT("Interaction component disabled on %s"),
            GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"));
        return;
    }

    // Validate cooldown
    if (!CanInteractNow())
    {
        UE_LOG(LogInteraction, VeryVerbose, TEXT("Interaction on cooldown"));
        return;
    }

    // Validate focus target
    if (!CurrentFocusActor)
    {
        UE_LOG(LogInteraction, Log, TEXT("No interaction target available"));
        return;
    }

    // Validate interface implementation
    if (!CurrentFocusActor->Implements<UInteractable>())
    {
        UE_LOG(LogInteraction, Error, TEXT("Focus actor %s doesn't implement IInteractable"),
            *CurrentFocusActor->GetName());
        CurrentFocusActor = nullptr; // Clear invalid focus
        return;
    }

    // Final validation before interaction
    if (!IInteractable::Execute_CanInteract(CurrentFocusActor, GetOwner()))
    {
        UE_LOG(LogInteraction, Log, TEXT("Cannot interact with %s"), *CurrentFocusActor->GetName());
        return;
    }

    // All validations passed - perform interaction
    LastInteractionTime = GetWorld()->GetTimeSeconds();
    IInteractable::Execute_Interact(CurrentFocusActor, GetOwner());

    UE_LOG(LogInteraction, Log, TEXT("Successfully interacted with %s"), *CurrentFocusActor->GetName());
}
```

### AI System Errors

```cpp
void AEnemyNPC::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    // Validate perception component
    UAIPerceptionComponent* PerceptionComp = GetPerceptionComponent();
    if (!ensure(PerceptionComp))
    {
        UE_LOG(LogAI, Error, TEXT("No perception component on %s"), *GetName());
        return;
    }

    // Validate blackboard
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
    if (!ensure(BlackboardComp))
    {
        UE_LOG(LogAI, Error, TEXT("No blackboard component on %s"), *GetName());
        return;
    }

    for (AActor* Actor : UpdatedActors)
    {
        if (!IsValid(Actor))
        {
            UE_LOG(LogAI, Warning, TEXT("Invalid actor in perception update"));
            continue;
        }

        // Validate actor is a valid target
        if (!IsValidTarget(Actor))
        {
            continue;
        }

        // Get perception info safely
        FActorPerceptionBlueprintInfo PerceptionInfo;
        if (PerceptionComp->GetActorsPerception(Actor, PerceptionInfo))
        {
            // Process valid perception data
            ProcessTargetPerception(Actor, PerceptionInfo);
        }
    }
}

bool AEnemyNPC::IsValidTarget(AActor* Actor) const
{
    if (!IsValid(Actor))
        return false;

    // Must be a pawn
    APawn* TargetPawn = Cast<APawn>(Actor);
    if (!TargetPawn)
        return false;

    // Must be alive (if has stats component)
    if (UStatsComponent* StatsComp = TargetPawn->FindComponentByClass<UStatsComponent>())
    {
        if (StatsComp->IsDead())
            return false;
    }

    // Must be enemy faction
    return IsEnemy(TargetPawn);
}
```

---

## Performance Considerations

### Minimize Error Checking in Hot Paths

```cpp
// In frequently called functions (like Tick), use lighter validation
void UMyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Light validation - only check critical things
    if (UNLIKELY(!GetOwner()))
    {
        // Use UNLIKELY for rare error conditions to help branch prediction
        return;
    }

    // Use checkSlow for debug-only validation
    checkSlow(DeltaTime >= 0.0f);

    // Main logic here...
}
```

### Batch Validation

```cpp
bool UMyComponent::ValidateState() const
{
    bool bIsValid = true;

    if (!GetOwner())
    {
        UE_LOG(LogPlatformer, Error, TEXT("No owner"));
        bIsValid = false;
    }

    if (!TargetActor)
    {
        UE_LOG(LogPlatformer, Error, TEXT("No target"));
        bIsValid = false;
    }

    if (SomeImportantValue < 0.0f)
    {
        UE_LOG(LogPlatformer, Error, TEXT("Invalid value: %.2f"), SomeImportantValue);
        bIsValid = false;
    }

    return bIsValid;
}

void UMyComponent::DoImportantOperation()
{
    // Validate everything at once
    if (!ValidateState())
    {
        UE_LOG(LogPlatformer, Error, TEXT("Component state invalid, aborting operation"));
        return;
    }

    // Proceed with operation...
}
```

---

## Testing Error Conditions

### Unit Test Error Handling

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterErrorHandlingTest,
    "Platformer.Unit.Character.ErrorHandling",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCharacterErrorHandlingTest::RunTest(const FString& Parameters)
{
    UWorld* World = FPlatformerTestBase::CreateTestWorld();
    APlatformCharacter* Character = FPlatformerTestBase::SpawnTestActor<APlatformCharacter>(World);

    // Test null input handling
    Character->Move(FInputActionValue()); // Empty value
    TestTrue("Character handles null input gracefully", true); // Should not crash

    // Test invalid controller scenario
    Character->SetActorEnableCollision(false);
    Character->PossessedBy(nullptr); // Remove controller
    Character->Move(FInputActionValue(FVector2D(1.0f, 0.0f))); // Should handle gracefully

    return true;
}
```

---

## Error Recovery Strategies

### Graceful Degradation

```cpp
void AMyGameMode::HandleCriticalError(const FString& ErrorMessage)
{
    UE_LOG(LogPlatformer, Error, TEXT("Critical error: %s"), *ErrorMessage);

    // Try to recover gracefully
    if (GetWorld())
    {
        // Disable non-essential systems
        DisableNonEssentialSystems();

        // Notify player
        ShowErrorNotification(TEXT("A recoverable error occurred. Some features may be disabled."));

        // Log for analytics
        LogErrorToAnalytics(ErrorMessage);
    }
    else
    {
        // Last resort - controlled shutdown
        RequestEngineExit(TEXT("Unrecoverable error"));
    }
}
```

Remember: Good error handling makes the difference between a robust, professional game and a fragile prototype.
