# Debugging Guide

## Blueprint vs C++ Debugging

### C++ Debugging in Visual Studio

**Basic Setup:**

1. Set breakpoints in VS by clicking left margin
2. Launch UE project from VS (F5) instead of Epic Launcher
3. Use "Debug Game" configuration for debugging, "Development" for performance

**Common Debug Workflow:**

```cpp
// Add debug logging
UE_LOG(LogTemp, Warning, TEXT("Player position: %s"), *GetActorLocation().ToString());

// Conditional breakpoints
if (PlayerHealth <= 0)
{
    // Breakpoint here to debug death logic
    UE_LOG(LogTemp, Error, TEXT("Player died!"));
}
```

**Useful Debug Commands (in UE console):**

- `stat fps` - Show framerate
- `stat memory` - Memory usage
- `showdebug ai` - AI debugging info
- `showdebug input` - Input debugging

### Blueprint Debugging

**Visual Debugging:**

- Add breakpoints to Blueprint nodes
- Use "Print String" nodes for quick debugging
- Watch window shows variable values
- Step through execution node by node

**C++ → Blueprint Debugging:**

- Expose C++ variables with `UPROPERTY(BlueprintReadOnly)`
- Use `UFUNCTION(BlueprintCallable)` to call C++ from BP
- Debug Blueprint that extends your C++ class

## Runtime Debugging Tools

### UE5 Built-in Tools

**Gameplay Debugger (Apostrophe key):**

```cpp
// In your project, enable gameplay debugger modules
// Add to Build.cs:
PrivateDependencyModuleNames.AddRange(new string[] {
    "GameplayDebugger"
});
```

**Viewport Debug Drawing:**

```cpp
// Draw debug spheres, lines, etc.
DrawDebugSphere(GetWorld(), GetActorLocation(), 100.0f, 12, FColor::Red, false, 1.0f);
DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, 1.0f);
```

**Console Commands for Debugging:**

- `DumpConsoleCommands` - List all available commands
- `showflag.collision` - Visualize collision
- `stat unit` - Performance breakdown
- `r.screenpercentage 50` - Reduce resolution for performance testing

### AI Debugging

**Behavior Tree Debugging:**

```cpp
// In AIController
void AMyAIController::BeginPlay()
{
    Super::BeginPlay();

    // Enable BT debugging
    if (BehaviorTreeComponent)
    {
        BehaviorTreeComponent->SetLoggingEnabled(true);
    }
}
```

**Blackboard Debugging:**

```cpp
// Log blackboard values
UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
if (BlackboardComp)
{
    FVector PlayerLocation = BlackboardComp->GetValueAsVector(TEXT("PlayerLocation"));
    UE_LOG(LogTemp, Warning, TEXT("AI sees player at: %s"), *PlayerLocation.ToString());
}
```

### Network Debugging

**For multiplayer projects:**

```cpp
// Log with network role context
UE_LOG(LogTemp, Warning, TEXT("[%s] Player health: %f"),
    GetLocalRole() == ROLE_Authority ? TEXT("SERVER") : TEXT("CLIENT"),
    Health);
```

## Performance Debugging

### Identifying Performance Issues

**Stat Commands:**

- `stat fps` - Frame rate
- `stat unit` - Frame time breakdown
- `stat memory` - Memory usage
- `stat streaming` - Asset streaming
- `stat ai` - AI performance

**Profiling Tools:**

- **Unreal Insights**: Comprehensive profiling (Session Frontend)
- **stat scenerendering**: Rendering performance
- **stat rhi**: Graphics driver performance

### Common Performance Problems

**Tick Functions:**

```cpp
// DON'T: Expensive operations in Tick
void UMyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // Bad: Complex calculations every frame
    for (int32 i = 0; i < 1000; ++i)
    {
        // Expensive work
    }
}

// DO: Use timers for periodic updates
void UMyComponent::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(
        UpdateTimerHandle,
        this,
        &UMyComponent::PeriodicUpdate,
        0.1f,  // Every 100ms instead of every frame
        true
    );
}
```

**Memory Allocations:**

```cpp
// DON'T: Allocate every frame
void UMyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    TArray<AActor*> FoundActors; // New allocation every frame
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
}

// DO: Cache or use object pooling
class UMyComponent : public UActorComponent
{
private:
    TArray<AActor*> CachedActors; // Reuse this array

public:
    void RefreshActorCache()
    {
        CachedActors.Empty(); // Clear without deallocating
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), CachedActors);
    }
};
```

## Error Diagnosis

### Compilation Errors

**"Unresolved external symbol":**

```cpp
// Common cause: Missing module dependency
// Add to YourProject.Build.cs:
PublicDependencyModuleNames.AddRange(new string[] {
    "EnhancedInput",  // If using Enhanced Input
    "AIModule",       // If using AI classes
    "UMG"             // If using UI widgets
});
```

**"Cannot convert from X to Y":**

```cpp
// Common cause: Missing cast or wrong type
// Wrong:
APlayerController* PC = GetController();

// Right:
APlayerController* PC = Cast<APlayerController>(GetController());
if (PC)
{
    // Safe to use PC
}
```

### Runtime Errors

**Null Pointer Access:**

```cpp
// Always check for null before dereferencing
if (MyComponent)
{
    MyComponent->DoSomething();
}

// Or use ensure for debugging
if (ensure(MyComponent))
{
    MyComponent->DoSomething();
}

// For critical code, use check (crashes if null)
check(MyComponent);
MyComponent->DoSomething();
```

**Access Violation:**

```cpp
// Common cause: Accessing deleted UObject
// Use weak pointers for non-owning references
UPROPERTY()
TWeakObjectPtr<AActor> CachedActor;

// Check validity before use
if (CachedActor.IsValid())
{
    CachedActor->DoSomething();
}
```

## Logging Best Practices

### Structured Logging

```cpp
// Define log categories
DECLARE_LOG_CATEGORY_EXTERN(LogPlatformer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogAI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);

// In .cpp file:
DEFINE_LOG_CATEGORY(LogPlatformer);
DEFINE_LOG_CATEGORY(LogAI);
DEFINE_LOG_CATEGORY(LogInteraction);

// Usage with different verbosity levels
UE_LOG(LogPlatformer, Verbose, TEXT("Detailed debug info"));    // Only in debug builds
UE_LOG(LogPlatformer, Log, TEXT("General information"));        // Normal logging
UE_LOG(LogPlatformer, Warning, TEXT("Something unusual"));      // Warnings
UE_LOG(LogPlatformer, Error, TEXT("Something failed"));         // Errors
```

### Conditional Logging

```cpp
// Only log in debug builds
#if UE_BUILD_DEBUG
    UE_LOG(LogPlatformer, Warning, TEXT("Debug info: %d"), SomeValue);
#endif

// Use ensure for development warnings
if (!ensure(IsValid(SomeActor)))
{
    UE_LOG(LogPlatformer, Error, TEXT("SomeActor is invalid!"));
    return;
}
```

### Visual Debugging

```cpp
// On-screen debug messages
if (GEngine)
{
    GEngine->AddOnScreenDebugMessage(
        -1,                    // Key (use -1 for new message each time)
        5.0f,                  // Display time
        FColor::Red,           // Color
        FString::Printf(TEXT("Health: %.1f"), CurrentHealth)
    );
}

// Debug draw helpers
void UMyComponent::DebugDraw()
{
    if (!GetWorld()) return;

    // Draw interaction range
    DrawDebugSphere(
        GetWorld(),
        GetOwner()->GetActorLocation(),
        InteractionRange,
        32,
        FColor::Blue,
        false,
        0.1f
    );

    // Draw trace lines
    if (LastTraceHit.bBlockingHit)
    {
        DrawDebugLine(
            GetWorld(),
            TraceStart,
            LastTraceHit.Location,
            FColor::Green,
            false,
            0.1f,
            0,
            2.0f
        );
    }
}
```

## Testing and Validation

### Unit Testing Approach

```cpp
// Simple validation functions
bool UMyComponent::ValidateConfiguration() const
{
    if (!ensure(InteractionRange > 0))
    {
        UE_LOG(LogPlatformer, Error, TEXT("InteractionRange must be > 0"));
        return false;
    }

    if (!ensure(IsValid(RequiredComponent)))
    {
        UE_LOG(LogPlatformer, Error, TEXT("RequiredComponent is not set"));
        return false;
    }

    return true;
}

// Call validation in BeginPlay
void UMyComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!ValidateConfiguration())
    {
        SetComponentTickEnabled(false); // Disable if invalid
    }
}
```

### Debug Console Commands

```cpp
// Create custom console commands for debugging
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
static TAutoConsoleVariable<bool> CVarShowInteractionDebug(
    TEXT("platformer.ShowInteractionDebug"),
    false,
    TEXT("Show interaction system debug information"),
    ECVF_Default
);

// Use in your code
if (CVarShowInteractionDebug.GetValueOnGameThread())
{
    // Draw debug information
    DebugDraw();
}
#endif
```

This debugging guide provides practical tools and patterns for identifying and fixing issues during development. Remember: when debugging, start with logging, use breakpoints strategically, and always validate your assumptions with actual data.
