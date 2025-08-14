# Testing Guide (Comprehensive)

## Testing Philosophy

- **Unit Tests**: Fast, isolated tests for individual components and functions
- **Integration Tests**: Verify component interactions and system behaviors
- **Functional Tests**: End-to-end gameplay validation in PIE
- **Performance Tests**: Validate frame rate and memory usage
- **Edge Case Tests**: Handle error conditions and invalid inputs

---

## Test Organization

```
Source/Platformer/Tests/
├── Unit/
│   ├── CharacterMovementTests.cpp
│   ├── InteractionComponentTests.cpp
│   └── StatsComponentTests.cpp
├── Integration/
│   ├── InputSystemTests.cpp
│   ├── AIBehaviorTests.cpp
│   └── InteractionSystemTests.cpp
├── Functional/
│   ├── GameplayTests.cpp
│   └── PerformanceTests.cpp
└── TestBase/
    ├── PlatformerTestBase.h
    └── PlatformerTestBase.cpp
```

---

## C++ Automation Spec Implementation

### Base Test Class

**`Source/Platformer/Tests/TestBase/PlatformerTestBase.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

class PLATFORMER_API FPlatformerTestBase
{
public:
    // Helper function to create test world
    static UWorld* CreateTestWorld();

    // Helper function to spawn test actors
    template<typename T>
    static T* SpawnTestActor(UWorld* World, const FVector& Location = FVector::ZeroVector);

    // Helper function to validate component properties
    static bool ValidateComponent(UActorComponent* Component, const FString& ExpectedState);

    // Helper function for frame-based testing
    static void TickWorld(UWorld* World, float DeltaTime = 0.016f, int32 FrameCount = 1);

protected:
    static UWorld* TestWorld;
};
```

### Character Movement Tests

**`Source/Platformer/Tests/Unit/CharacterMovementTests.cpp`:**

```cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "PlatformCharacter.h"
#include "TestBase/PlatformerTestBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterMovementSpeedTest,
    "Platformer.Unit.Character.MovementSpeed",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCharacterMovementSpeedTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWorld* World = FPlatformerTestBase::CreateTestWorld();
    APlatformCharacter* Character = FPlatformerTestBase::SpawnTestActor<APlatformCharacter>(World);

    if (!TestNotNull("Character spawned", Character))
        return false;

    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (!TestNotNull("Movement component exists", MovementComp))
        return false;

    // Act & Assert - Test sprint speed change
    float InitialSpeed = MovementComp->MaxWalkSpeed;
    Character->StartSprint();
    float SprintSpeed = MovementComp->MaxWalkSpeed;
    Character->StopSprint();
    float FinalSpeed = MovementComp->MaxWalkSpeed;

    TestTrue("Sprint increases speed", SprintSpeed > InitialSpeed);
    TestEqual("Speed returns to normal", FinalSpeed, InitialSpeed);
    TestTrue("Sprint speed within reasonable bounds", SprintSpeed >= 800.0f && SprintSpeed <= 1200.0f);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterJumpTest,
    "Platformer.Unit.Character.Jump",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCharacterJumpTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWorld* World = FPlatformerTestBase::CreateTestWorld();
    APlatformCharacter* Character = FPlatformerTestBase::SpawnTestActor<APlatformCharacter>(World);

    if (!TestNotNull("Character spawned", Character))
        return false;

    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (!TestNotNull("Movement component exists", MovementComp))
        return false;

    // Act & Assert - Test double jump capability
    TestEqual("Jump count starts at 2", Character->GetJumpMaxCount(), 2);

    // Simulate first jump
    Character->Jump();
    FPlatformerTestBase::TickWorld(World, 0.016f, 10); // Tick for 10 frames

    bool bIsInAir = !MovementComp->IsMovingOnGround();
    TestTrue("Character is airborne after jump", bIsInAir);

    return true;
}
```

### Interaction System Tests

**`Source/Platformer/Tests/Integration/InteractionSystemTests.cpp`:**

```cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "InteractionComponent.h"
#include "Interactable.h"
#include "TestBase/PlatformerTestBase.h"

// Mock interactable actor for testing
UCLASS()
class ATestInteractableActor : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    bool bCanInteract = true;
    bool bWasInteracted = false;
    FText InteractionVerb = FText::FromString(TEXT("Test"));

    virtual FText GetInteractionVerb_Implementation() const override { return InteractionVerb; }
    virtual bool CanInteract_Implementation(AActor* Instigator) const override { return bCanInteract; }
    virtual void Interact_Implementation(AActor* Instigator) override { bWasInteracted = true; }
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInteractionComponentTest,
    "Platformer.Integration.Interaction.Component",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInteractionComponentTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWorld* World = FPlatformerTestBase::CreateTestWorld();
    APawn* TestPawn = FPlatformerTestBase::SpawnTestActor<APawn>(World);
    ATestInteractableActor* InteractableActor = FPlatformerTestBase::SpawnTestActor<ATestInteractableActor>(World, FVector(100, 0, 0));

    UInteractionComponent* InteractionComp = NewObject<UInteractionComponent>(TestPawn);
    TestPawn->AddInstanceComponent(InteractionComp);
    InteractionComp->RegisterComponent();

    if (!TestNotNull("Interaction component created", InteractionComp))
        return false;

    // Act - Move interactable into range and update focus
    InteractableActor->SetActorLocation(FVector(50, 0, 0)); // Within range
    FPlatformerTestBase::TickWorld(World, 0.016f, 5); // Give time for focus update

    // Assert
    AActor* FocusActor = InteractionComp->GetCurrentFocusActor();
    TestEqual("Focus actor is the interactable", FocusActor, InteractableActor);

    // Test interaction
    InteractionComp->TryInteract();
    TestTrue("Interaction was triggered", InteractableActor->bWasInteracted);

    return true;
}
```

### Performance Tests

**`Source/Platformer/Tests/Functional/PerformanceTests.cpp`:**

```cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "TestBase/PlatformerTestBase.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/DateTime.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFrameRateTest,
    "Platformer.Performance.FrameRate",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FFrameRateTest::RunTest(const FString& Parameters)
{
    // Arrange
    const int32 TestFrameCount = 100;
    const float TargetFrameTime = 1.0f / 60.0f; // 60 FPS target

    UWorld* World = FPlatformerTestBase::CreateTestWorld();

    // Spawn multiple characters to stress test
    TArray<APlatformCharacter*> Characters;
    for (int32 i = 0; i < 10; ++i)
    {
        APlatformCharacter* Character = FPlatformerTestBase::SpawnTestActor<APlatformCharacter>(World,
            FVector(i * 100, 0, 0));
        if (Character)
        {
            Characters.Add(Character);
        }
    }

    // Act - Measure frame times
    TArray<float> FrameTimes;
    double StartTime = FPlatformTime::Seconds();

    for (int32 Frame = 0; Frame < TestFrameCount; ++Frame)
    {
        double FrameStartTime = FPlatformTime::Seconds();
        FPlatformerTestBase::TickWorld(World, TargetFrameTime);
        double FrameEndTime = FPlatformTime::Seconds();

        FrameTimes.Add(FrameEndTime - FrameStartTime);
    }

    double TotalTime = FPlatformTime::Seconds() - StartTime;

    // Assert - Calculate statistics
    float AverageFrameTime = 0.0f;
    float MaxFrameTime = 0.0f;

    for (float FrameTime : FrameTimes)
    {
        AverageFrameTime += FrameTime;
        MaxFrameTime = FMath::Max(MaxFrameTime, FrameTime);
    }

    AverageFrameTime /= FrameTimes.Num();

    TestTrue("Average frame time within target", AverageFrameTime <= TargetFrameTime * 1.1f); // 10% tolerance
    TestTrue("Max frame time reasonable", MaxFrameTime <= TargetFrameTime * 2.0f); // No frame should take more than 2x target

    AddInfo(FString::Printf(TEXT("Average Frame Time: %.3fms"), AverageFrameTime * 1000.0f));
    AddInfo(FString::Printf(TEXT("Max Frame Time: %.3fms"), MaxFrameTime * 1000.0f));

    return true;
}
```

---

## Blueprint Functional Tests

### Test Map Setup

1. **Create Test Level**: `Content/Tests/TestLevel_Interaction`
2. **Add Test Actors**: Place interactive objects at known positions
3. **Create Functional Test Blueprint**: `BP_InteractionFunctionalTest`

### Example Functional Test Blueprint

```cpp
// In BP_InteractionFunctionalTest
Begin Play:
1. Spawn Player Character at (0, 0, 100)
2. Spawn Interactive Door at (200, 0, 0)
3. Wait 1 second
4. Move Player to (150, 0, 100) - within interaction range
5. Assert "Interaction prompt visible"
6. Simulate Interact input
7. Assert "Door opened"
8. Finish Test with Success
```

---

## Running Tests

### Console Commands

```cpp
// Run all tests
Automation RunTests Platformer

// Run specific test category
Automation RunTests Platformer.Unit
Automation RunTests Platformer.Integration
Automation RunTests Platformer.Performance

// Run single test
Automation RunTests Platformer.Unit.Character.MovementSpeed
```

### CI Integration

**`.github/workflows/test.yml`:**

```yaml
name: Automated Tests
on: [push, pull_request]

jobs:
  test:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v3
      - name: Run UE Tests
        run: |
          # Run headless editor with automation
          UnrealEditor.exe YourProject.uproject -ExecCmds="Automation RunTests Platformer; Quit" -unattended -nopause -nullrhi
```

---

## Test Coverage Goals

| System                 | Target Coverage | Key Tests                                          |
| ---------------------- | --------------- | -------------------------------------------------- |
| **Character Movement** | 90%+            | Speed changes, jumping, input response             |
| **Interaction System** | 85%+            | Focus detection, range validation, interface calls |
| **AI Behavior**        | 80%+            | State transitions, perception, pathfinding         |
| **Stats System**       | 95%+            | Damage calculation, death handling, regeneration   |
| **Input System**       | 85%+            | Action binding, context switching, validation      |

---

## Performance Benchmarks

| Metric           | Target     | Test Method                          |
| ---------------- | ---------- | ------------------------------------ |
| **Frame Rate**   | 60+ FPS    | Stress test with multiple characters |
| **Memory Usage** | <512MB     | Monitor over extended play session   |
| **Loading Time** | <3 seconds | Level transition timing              |
| **AI Response**  | <100ms     | Perception to action delay           |

Remember: Tests should run fast, be deterministic, and provide clear failure messages.
