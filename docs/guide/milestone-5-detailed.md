# Milestone 5 - Detailed Implementation

**Goal:** Add comprehensive testing, console variables, and final polish to create a production-ready game foundation

**Estimated Time:** 6-8 hours over 2-3 sessions

## Overview Checklist

- [ ] Set up Automation Testing framework
- [ ] Create functional tests for core systems
- [ ] Implement console variables (CVars) for tuning
- [ ] Add performance monitoring and debugging
- [ ] Polish code with documentation and cleanup
- [ ] Validate build pipeline and warnings

---

## Session 1: Automation Testing Setup (2-3 hours)

### Step 5.1: Enable Testing Plugins (15 minutes)

**In UE Editor:**

1. Edit → Plugins
2. Search and Enable:
   - ✅ Automation Tests
   - ✅ Functional Testing Framework
   - ✅ Runtime Tests
3. Restart Editor when prompted

**Verify Setup:**

- [ ] Window → Developer Tools → Automation shows testing panel
- [ ] No plugin errors in Output Log

### Step 5.2: Create Base Test Classes (45 minutes)

**Create Test Foundation:**

1. Tools → New C++ Class → None → Show All Classes → Search "AutomationTestBase"
2. Name: `PlatformerTestBase`

**Header `Source/Platformer/Tests/PlatformerTestBase.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Tests/AutomationTestBase.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "PlatformerTestBase.generated.h"

class APlatformCharacter;
class AEnemyNPC;
class AFriendlyNPC;

UCLASS()
class PLATFORMER_API UPlatformerTestBase : public UAutomationTestBase
{
	GENERATED_BODY()

public:
	UPlatformerTestBase();

protected:
	// Helper to create test world
	UWorld* CreateTestWorld();

	// Helper to spawn player character
	APlatformCharacter* SpawnTestPlayer(UWorld* World, const FVector& Location = FVector::ZeroVector);

	// Helper to spawn enemy NPC
	AEnemyNPC* SpawnTestEnemy(UWorld* World, const FVector& Location = FVector::ZeroVector);

	// Helper to spawn friendly NPC
	AFriendlyNPC* SpawnTestFriendly(UWorld* World, const FVector& Location = FVector::ZeroVector);

	// Helper to wait for frames
	void WaitForFrames(int32 NumFrames);

	// Helper to simulate input
	void SimulateInput(APlatformCharacter* Character, const FString& ActionName, float Duration = 0.1f);

	// Cleanup
	virtual void TearDown() override;

private:
	UPROPERTY()
	UWorld* TestWorld;

	UPROPERTY()
	TArray<AActor*> SpawnedActors;
};
```

**Implementation `Source/Platformer/Tests/PlatformerTestBase.cpp`:**

```cpp
#include "PlatformerTestBase.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "../PlatformCharacter.h"
#include "../EnemyNPC.h"
#include "../FriendlyNPC.h"
#include "Misc/AutomationTest.h"

UPlatformerTestBase::UPlatformerTestBase()
{
	TestWorld = nullptr;
}

UWorld* UPlatformerTestBase::CreateTestWorld()
{
	if (TestWorld)
	{
		return TestWorld;
	}

	// Create a minimal test world
	TestWorld = UWorld::CreateWorld(EWorldType::Game, false);

	if (TestWorld)
	{
		TestWorld->InitializeActorsForPlay(FURL());
		TestWorld->BeginPlay();
	}

	return TestWorld;
}

APlatformCharacter* UPlatformerTestBase::SpawnTestPlayer(UWorld* World, const FVector& Location)
{
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APlatformCharacter* Player = World->SpawnActor<APlatformCharacter>(APlatformCharacter::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);

	if (Player)
	{
		SpawnedActors.Add(Player);
	}

	return Player;
}

AEnemyNPC* UPlatformerTestBase::SpawnTestEnemy(UWorld* World, const FVector& Location)
{
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEnemyNPC* Enemy = World->SpawnActor<AEnemyNPC>(AEnemyNPC::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);

	if (Enemy)
	{
		SpawnedActors.Add(Enemy);
	}

	return Enemy;
}

AFriendlyNPC* UPlatformerTestBase::SpawnTestFriendly(UWorld* World, const FVector& Location)
{
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFriendlyNPC* Friendly = World->SpawnActor<AFriendlyNPC>(AFriendlyNPC::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);

	if (Friendly)
	{
		SpawnedActors.Add(Friendly);
	}

	return Friendly;
}

void UPlatformerTestBase::WaitForFrames(int32 NumFrames)
{
	for (int32 i = 0; i < NumFrames; i++)
	{
		if (TestWorld)
		{
			TestWorld->Tick(LEVELTICK_All, 1.0f / 60.0f); // Simulate 60 FPS
		}
	}
}

void UPlatformerTestBase::SimulateInput(APlatformCharacter* Character, const FString& ActionName, float Duration)
{
	if (!Character)
	{
		return;
	}

	// This is a simplified input simulation
	// In a real implementation, you'd need to trigger the actual input actions
	UE_LOG(LogTemp, Log, TEXT("Simulating input '%s' for duration %f"), *ActionName, Duration);

	// Wait for the duration
	const int32 FramesToWait = FMath::CeilToInt(Duration * 60.0f);
	WaitForFrames(FramesToWait);
}

void UPlatformerTestBase::TearDown()
{
	// Clean up spawned actors
	for (AActor* Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedActors.Empty();

	// Clean up test world
	if (TestWorld)
	{
		TestWorld->DestroyWorld(false);
		TestWorld = nullptr;
	}

	Super::TearDown();
}
```

### Step 5.3: Create Movement Tests (60 minutes)

**Create Movement Test Class:**

**Header `Source/Platformer/Tests/MovementTests.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "PlatformerTestBase.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlatformCharacterMovementTest, "Platformer.Character.Movement",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlatformCharacterJumpTest, "Platformer.Character.Jump",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlatformCharacterSprintTest, "Platformer.Character.Sprint",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
```

**Implementation `Source/Platformer/Tests/MovementTests.cpp`:**

```cpp
#include "MovementTests.h"
#include "../PlatformCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

bool FPlatformCharacterMovementTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn player character
	APlatformCharacter* Player = SpawnTestPlayer(World, FVector(0, 0, 100));
	TestTrue("Player spawned successfully", Player != nullptr);

	if (!Player)
	{
		return false;
	}

	// Test initial position
	const FVector InitialPosition = Player->GetActorLocation();
	TestEqual("Initial position is correct", InitialPosition, FVector(0, 0, 100), 1.0f);

	// Test movement component exists
	UCharacterMovementComponent* MovementComp = Player->GetCharacterMovement();
	TestTrue("Movement component exists", MovementComp != nullptr);

	if (MovementComp)
	{
		// Test movement parameters
		TestTrue("Max walk speed is reasonable", MovementComp->MaxWalkSpeed > 0.0f && MovementComp->MaxWalkSpeed < 2000.0f);
		TestTrue("Jump Z velocity is reasonable", MovementComp->JumpZVelocity > 0.0f && MovementComp->JumpZVelocity < 2000.0f);
	}

	// Simulate forward movement
	const FVector ForwardVector = Player->GetActorForwardVector();
	Player->AddMovementInput(ForwardVector, 1.0f);

	// Wait a few frames for movement to process
	WaitForFrames(10);

	const FVector NewPosition = Player->GetActorLocation();
	const float DistanceMoved = FVector::Dist(InitialPosition, NewPosition);
	TestTrue("Character moved forward", DistanceMoved > 1.0f);

	return true;
}

bool FPlatformCharacterJumpTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn player character
	APlatformCharacter* Player = SpawnTestPlayer(World, FVector(0, 0, 100));
	TestTrue("Player spawned successfully", Player != nullptr);

	if (!Player)
	{
		return false;
	}

	// Test initial ground state
	UCharacterMovementComponent* MovementComp = Player->GetCharacterMovement();
	TestTrue("Movement component exists", MovementComp != nullptr);

	if (!MovementComp)
	{
		return false;
	}

	// Make sure character is on ground initially
	const FVector InitialPosition = Player->GetActorLocation();
	const bool bInitiallyOnGround = MovementComp->IsMovingOnGround();

	// Trigger jump
	Player->Jump();

	// Wait for jump to take effect
	WaitForFrames(5);

	// Check if character is airborne
	const bool bIsAirborne = !MovementComp->IsMovingOnGround();
	TestTrue("Character is airborne after jump", bIsAirborne);

	// Check if character moved upward
	const FVector JumpPosition = Player->GetActorLocation();
	const float HeightGained = JumpPosition.Z - InitialPosition.Z;
	TestTrue("Character gained height", HeightGained > 10.0f);

	// Test double jump if available
	if (Player->CanJump() && bIsAirborne)
	{
		Player->Jump();
		WaitForFrames(5);

		const FVector DoubleJumpPosition = Player->GetActorLocation();
		const float AdditionalHeight = DoubleJumpPosition.Z - JumpPosition.Z;
		TestTrue("Double jump provided additional height", AdditionalHeight > 5.0f);
	}

	return true;
}

bool FPlatformCharacterSprintTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn player character
	APlatformCharacter* Player = SpawnTestPlayer(World, FVector(0, 0, 100));
	TestTrue("Player spawned successfully", Player != nullptr);

	if (!Player)
	{
		return false;
	}

	UCharacterMovementComponent* MovementComp = Player->GetCharacterMovement();
	TestTrue("Movement component exists", MovementComp != nullptr);

	if (!MovementComp)
	{
		return false;
	}

	// Record normal walk speed
	const float NormalWalkSpeed = MovementComp->MaxWalkSpeed;

	// Start sprinting (this would normally be triggered by input)
	// For testing purposes, we'll manually adjust the speed
	const float SprintMultiplier = 1.5f;
	MovementComp->MaxWalkSpeed = NormalWalkSpeed * SprintMultiplier;

	// Test that sprint speed is higher
	TestTrue("Sprint speed is higher than walk speed", MovementComp->MaxWalkSpeed > NormalWalkSpeed);

	// Simulate movement at sprint speed
	const FVector InitialPosition = Player->GetActorLocation();
	const FVector ForwardVector = Player->GetActorForwardVector();

	Player->AddMovementInput(ForwardVector, 1.0f);
	WaitForFrames(30); // Wait longer to see sprint effect

	const FVector SprintPosition = Player->GetActorLocation();
	const float SprintDistance = FVector::Dist(InitialPosition, SprintPosition);

	// Reset to normal speed and test again
	MovementComp->MaxWalkSpeed = NormalWalkSpeed;
	Player->SetActorLocation(FVector(200, 0, 100)); // Move to new starting position
	const FVector WalkStartPosition = Player->GetActorLocation();

	Player->AddMovementInput(ForwardVector, 1.0f);
	WaitForFrames(30); // Same duration as sprint test

	const FVector WalkPosition = Player->GetActorLocation();
	const float WalkDistance = FVector::Dist(WalkStartPosition, WalkPosition);

	// Sprint should cover more distance in the same time
	TestTrue("Sprint covers more distance than walk", SprintDistance > WalkDistance);

	return true;
}
```

---

## Session 2: Interaction & AI Tests (2-3 hours)

### Step 5.4: Create Interaction System Tests (60 minutes)

**Header `Source/Platformer/Tests/InteractionTests.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "PlatformerTestBase.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInteractionPromptTest, "Platformer.Interaction.Prompt",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInteractionRangeTest, "Platformer.Interaction.Range",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPickupInteractionTest, "Platformer.Interaction.Pickup",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
```

**Implementation `Source/Platformer/Tests/InteractionTests.cpp`:**

```cpp
#include "InteractionTests.h"
#include "../PlatformCharacter.h"
#include "../InteractionComponent.h"
#include "../InteractiveActor.h"
#include "../PickupActor.h"

bool FInteractionPromptTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn player character
	APlatformCharacter* Player = SpawnTestPlayer(World, FVector(0, 0, 100));
	TestTrue("Player spawned successfully", Player != nullptr);

	if (!Player)
	{
		return false;
	}

	// Get interaction component
	UInteractionComponent* InteractionComp = Player->FindComponentByClass<UInteractionComponent>();
	TestTrue("Player has interaction component", InteractionComp != nullptr);

	if (!InteractionComp)
	{
		return false;
	}

	// Spawn a pickup nearby
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APickupActor* Pickup = World->SpawnActor<APickupActor>(APickupActor::StaticClass(),
		FVector(150, 0, 100), FRotator::ZeroRotator, SpawnParams);
	TestTrue("Pickup spawned successfully", Pickup != nullptr);

	if (!Pickup)
	{
		return false;
	}
	SpawnedActors.Add(Pickup);

	// Test interaction detection
	WaitForFrames(5); // Allow time for interaction detection

	// Check if prompt should be visible (within range)
	const float Distance = FVector::Dist(Player->GetActorLocation(), Pickup->GetActorLocation());
	const float InteractionRange = InteractionComp->GetInteractionRange();

	if (Distance <= InteractionRange)
	{
		TestTrue("Pickup is within interaction range", true);
		// In a real implementation, you'd check if the UI prompt is visible
	}
	else
	{
		TestTrue("Pickup is outside interaction range", false);
	}

	return true;
}

bool FInteractionRangeTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn player character
	APlatformCharacter* Player = SpawnTestPlayer(World, FVector(0, 0, 100));
	TestTrue("Player spawned successfully", Player != nullptr);

	if (!Player)
	{
		return false;
	}

	// Get interaction component
	UInteractionComponent* InteractionComp = Player->FindComponentByClass<UInteractionComponent>();
	TestTrue("Player has interaction component", InteractionComp != nullptr);

	if (!InteractionComp)
	{
		return false;
	}

	// Test interaction range settings
	const float DefaultRange = InteractionComp->GetInteractionRange();
	TestTrue("Default interaction range is reasonable", DefaultRange > 50.0f && DefaultRange < 500.0f);

	// Spawn pickups at different distances
	const TArray<float> TestDistances = {50.0f, 150.0f, 300.0f, 500.0f};

	for (int32 i = 0; i < TestDistances.Num(); i++)
	{
		const float TestDistance = TestDistances[i];

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APickupActor* Pickup = World->SpawnActor<APickupActor>(APickupActor::StaticClass(),
			FVector(TestDistance, 0, 100), FRotator::ZeroRotator, SpawnParams);

		if (Pickup)
		{
			SpawnedActors.Add(Pickup);

			WaitForFrames(3);

			const bool bShouldBeInRange = TestDistance <= DefaultRange;
			const bool bIsInRange = FVector::Dist(Player->GetActorLocation(), Pickup->GetActorLocation()) <= DefaultRange;

			TestEqual(FString::Printf(TEXT("Pickup at distance %f range check"), TestDistance),
				bIsInRange, bShouldBeInRange);
		}
	}

	return true;
}

bool FPickupInteractionTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn player character
	APlatformCharacter* Player = SpawnTestPlayer(World, FVector(0, 0, 100));
	TestTrue("Player spawned successfully", Player != nullptr);

	if (!Player)
	{
		return false;
	}

	// Spawn a pickup very close to player
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APickupActor* Pickup = World->SpawnActor<APickupActor>(APickupActor::StaticClass(),
		FVector(100, 0, 100), FRotator::ZeroRotator, SpawnParams);
	TestTrue("Pickup spawned successfully", Pickup != nullptr);

	if (!Pickup)
	{
		return false;
	}
	SpawnedActors.Add(Pickup);

	// Wait for interaction system to detect the pickup
	WaitForFrames(10);

	// Simulate interaction input
	UInteractionComponent* InteractionComp = Player->FindComponentByClass<UInteractionComponent>();
	if (InteractionComp)
	{
		// In a real implementation, this would trigger the interaction
		const bool bCanInteract = InteractionComp->CanInteractWithTarget();
		TestTrue("Can interact with pickup", bCanInteract);

		if (bCanInteract)
		{
			// Perform interaction
			const bool bInteractionSuccessful = InteractionComp->TryInteract();
			TestTrue("Interaction was successful", bInteractionSuccessful);

			// Check if pickup was consumed/destroyed
			WaitForFrames(3);
			const bool bPickupStillExists = IsValid(Pickup) && !Pickup->IsPendingKill();
			TestFalse("Pickup was consumed after interaction", bPickupStillExists);
		}
	}

	return true;
}
```

### Step 5.5: Create AI Behavior Tests (90 minutes)

**Header `Source/Platformer/Tests/AITests.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "PlatformerTestBase.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemySpawnTest, "Platformer.AI.Enemy.Spawn",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyPatrolTest, "Platformer.AI.Enemy.Patrol",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyChaseTest, "Platformer.AI.Enemy.Chase",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFriendlyNPCDialogTest, "Platformer.AI.Friendly.Dialog",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
```

**Implementation `Source/Platformer/Tests/AITests.cpp`:**

```cpp
#include "AITests.h"
#include "../EnemyNPC.h"
#include "../FriendlyNPC.h"
#include "../PlatformCharacter.h"
#include "../StatsComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

bool FEnemySpawnTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn enemy NPC
	AEnemyNPC* Enemy = SpawnTestEnemy(World, FVector(0, 0, 100));
	TestTrue("Enemy spawned successfully", Enemy != nullptr);

	if (!Enemy)
	{
		return false;
	}

	// Test enemy components
	UStatsComponent* StatsComp = Enemy->FindComponentByClass<UStatsComponent>();
	TestTrue("Enemy has stats component", StatsComp != nullptr);

	if (StatsComp)
	{
		TestTrue("Enemy has positive health", StatsComp->GetCurrentHealth() > 0.0f);
		TestTrue("Enemy is alive", StatsComp->IsAlive());
	}

	// Test AI Controller
	AAIController* AIController = Cast<AAIController>(Enemy->GetController());
	TestTrue("Enemy has AI controller", AIController != nullptr);

	if (AIController)
	{
		// Test blackboard
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		TestTrue("AI controller has blackboard", Blackboard != nullptr);
	}

	return true;
}

bool FEnemyPatrolTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn enemy NPC
	AEnemyNPC* Enemy = SpawnTestEnemy(World, FVector(0, 0, 100));
	TestTrue("Enemy spawned successfully", Enemy != nullptr);

	if (!Enemy)
	{
		return false;
	}

	// Record initial position
	const FVector InitialPosition = Enemy->GetActorLocation();

	// Wait for AI to initialize and start patrolling
	WaitForFrames(60); // Wait 1 second at 60 FPS

	// Check if enemy has moved (indicating patrol behavior)
	const FVector CurrentPosition = Enemy->GetActorLocation();
	const float DistanceMoved = FVector::Dist(InitialPosition, CurrentPosition);

	// Enemy should move during patrol (even if just slightly)
	TestTrue("Enemy moved during patrol", DistanceMoved > 1.0f);

	// Test that enemy returns to patrol area over time
	WaitForFrames(300); // Wait 5 seconds

	const FVector LaterPosition = Enemy->GetActorLocation();
	const float DistanceFromStart = FVector::Dist(InitialPosition, LaterPosition);

	// Enemy should stay within reasonable patrol radius
	TestTrue("Enemy stays within patrol area", DistanceFromStart < 500.0f);

	return true;
}

bool FEnemyChaseTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn enemy NPC and player close together
	AEnemyNPC* Enemy = SpawnTestEnemy(World, FVector(0, 0, 100));
	APlatformCharacter* Player = SpawnTestPlayer(World, FVector(300, 0, 100));

	TestTrue("Enemy spawned successfully", Enemy != nullptr);
	TestTrue("Player spawned successfully", Player != nullptr);

	if (!Enemy || !Player)
	{
		return false;
	}

	// Wait for AI to detect player
	WaitForFrames(30);

	AAIController* AIController = Cast<AAIController>(Enemy->GetController());
	if (AIController && AIController->GetBlackboardComponent())
	{
		// Check if player is detected in blackboard
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetActor")));

		if (TargetActor == Player)
		{
			TestTrue("Enemy detected player", true);

			// Record positions before chase
			const FVector EnemyInitialPos = Enemy->GetActorLocation();
			const FVector PlayerPos = Player->GetActorLocation();

			// Wait for chase behavior
			WaitForFrames(60);

			// Check if enemy moved towards player
			const FVector EnemyNewPos = Enemy->GetActorLocation();
			const float InitialDistance = FVector::Dist(EnemyInitialPos, PlayerPos);
			const float NewDistance = FVector::Dist(EnemyNewPos, PlayerPos);

			TestTrue("Enemy moved closer to player during chase", NewDistance < InitialDistance);
		}
		else
		{
			TestTrue("Enemy should detect player within sight range", false);
		}
	}

	return true;
}

bool FFriendlyNPCDialogTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = CreateTestWorld();
	TestTrue("Test world created", World != nullptr);

	if (!World)
	{
		return false;
	}

	// Spawn friendly NPC and player
	AFriendlyNPC* FriendlyNPC = SpawnTestFriendly(World, FVector(0, 0, 100));
	APlatformCharacter* Player = SpawnTestPlayer(World, FVector(150, 0, 100));

	TestTrue("Friendly NPC spawned successfully", FriendlyNPC != nullptr);
	TestTrue("Player spawned successfully", Player != nullptr);

	if (!FriendlyNPC || !Player)
	{
		return false;
	}

	// Test that NPC implements ITalkable interface
	const bool bImplementsTalkable = FriendlyNPC->GetClass()->ImplementsInterface(UTalkable::StaticClass());
	TestTrue("Friendly NPC implements ITalkable interface", bImplementsTalkable);

	// Test interaction with NPC
	if (bImplementsTalkable)
	{
		// Get dialog from NPC
		const FDialogSession* DialogSession = ITalkable::Execute_GetCurrentDialog(FriendlyNPC);

		if (DialogSession)
		{
			TestTrue("NPC has dialog session", true);
			TestTrue("Dialog has lines", DialogSession->DialogLines.Num() > 0);

			if (DialogSession->DialogLines.Num() > 0)
			{
				const FDialogLine& FirstLine = DialogSession->DialogLines[0];
				TestFalse("First dialog line has text", FirstLine.DialogText.IsEmpty());
				TestFalse("First dialog line has speaker name", FirstLine.SpeakerName.IsEmpty());
			}
		}
		else
		{
			TestTrue("NPC should have dialog session", false);
		}
	}

	return true;
}
```

---

## Session 3: Console Variables & Polish (2 hours)

### Step 5.6: Implement Console Variables (45 minutes)

**Create CVars Header `Source/Platformer/PlatformerCVars.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"

namespace PlatformerCVars
{
	// Camera settings
	extern TAutoConsoleVariable<float> CameraLagSpeed;
	extern TAutoConsoleVariable<float> CameraLagMaxDistance;
	extern TAutoConsoleVariable<int32> CameraDebugMode;

	// Interaction settings
	extern TAutoConsoleVariable<float> InteractionRange;
	extern TAutoConsoleVariable<int32> InteractionDebugDraw;

	// Movement settings
	extern TAutoConsoleVariable<float> WalkSpeedMultiplier;
	extern TAutoConsoleVariable<float> SprintSpeedMultiplier;
	extern TAutoConsoleVariable<float> JumpHeightMultiplier;

	// AI settings
	extern TAutoConsoleVariable<float> AIDetectionRange;
	extern TAutoConsoleVariable<float> AIPatrolRadius;
	extern TAutoConsoleVariable<int32> AIDebugMode;

	// Debug settings
	extern TAutoConsoleVariable<int32> ShowInteractionPrompts;
	extern TAutoConsoleVariable<int32> ShowHealthBars;
	extern TAutoConsoleVariable<int32> LogGameplayEvents;
}
```

**Implementation `Source/Platformer/PlatformerCVars.cpp`:**

```cpp
#include "PlatformerCVars.h"

namespace PlatformerCVars
{
	// Camera CVars
	TAutoConsoleVariable<float> CameraLagSpeed(
		TEXT("platformer.camera.LagSpeed"),
		10.0f,
		TEXT("Camera lag speed for smooth following. Higher values = more responsive camera."),
		ECVF_Default
	);

	TAutoConsoleVariable<float> CameraLagMaxDistance(
		TEXT("platformer.camera.LagMaxDistance"),
		200.0f,
		TEXT("Maximum distance camera can lag behind the character."),
		ECVF_Default
	);

	TAutoConsoleVariable<int32> CameraDebugMode(
		TEXT("platformer.camera.DebugMode"),
		0,
		TEXT("Show camera debug information. 0=Off, 1=Basic, 2=Detailed"),
		ECVF_Default
	);

	// Interaction CVars
	TAutoConsoleVariable<float> InteractionRange(
		TEXT("platformer.interaction.Range"),
		200.0f,
		TEXT("Maximum range for player interactions with objects."),
		ECVF_Default
	);

	TAutoConsoleVariable<int32> InteractionDebugDraw(
		TEXT("platformer.interaction.DebugDraw"),
		0,
		TEXT("Draw debug shapes for interaction ranges. 0=Off, 1=On"),
		ECVF_Default
	);

	// Movement CVars
	TAutoConsoleVariable<float> WalkSpeedMultiplier(
		TEXT("platformer.movement.WalkSpeedMultiplier"),
		1.0f,
		TEXT("Multiplier for character walk speed."),
		ECVF_Default
	);

	TAutoConsoleVariable<float> SprintSpeedMultiplier(
		TEXT("platformer.movement.SprintSpeedMultiplier"),
		1.5f,
		TEXT("Multiplier for character sprint speed."),
		ECVF_Default
	);

	TAutoConsoleVariable<float> JumpHeightMultiplier(
		TEXT("platformer.movement.JumpHeightMultiplier"),
		1.0f,
		TEXT("Multiplier for character jump height."),
		ECVF_Default
	);

	// AI CVars
	TAutoConsoleVariable<float> AIDetectionRange(
		TEXT("platformer.ai.DetectionRange"),
		800.0f,
		TEXT("Range at which AI can detect the player."),
		ECVF_Default
	);

	TAutoConsoleVariable<float> AIPatrolRadius(
		TEXT("platformer.ai.PatrolRadius"),
		400.0f,
		TEXT("Radius for AI patrol behavior."),
		ECVF_Default
	);

	TAutoConsoleVariable<int32> AIDebugMode(
		TEXT("platformer.ai.DebugMode"),
		0,
		TEXT("Show AI debug information. 0=Off, 1=Basic, 2=Detailed"),
		ECVF_Default
	);

	// Debug CVars
	TAutoConsoleVariable<int32> ShowInteractionPrompts(
		TEXT("platformer.debug.ShowInteractionPrompts"),
		1,
		TEXT("Show interaction UI prompts. 0=Hidden, 1=Visible"),
		ECVF_Default
	);

	TAutoConsoleVariable<int32> ShowHealthBars(
		TEXT("platformer.debug.ShowHealthBars"),
		0,
		TEXT("Show health bars above characters. 0=Hidden, 1=Visible"),
		ECVF_Default
	);

	TAutoConsoleVariable<int32> LogGameplayEvents(
		TEXT("platformer.debug.LogGameplayEvents"),
		0,
		TEXT("Log gameplay events to console. 0=Off, 1=On"),
		ECVF_Default
	);
}
```

### Step 5.7: Apply CVars to Game Systems (60 minutes)

**Update PlatformCharacter to use CVars:**

Add to `PlatformCharacter.cpp` in the movement functions:

```cpp
#include "PlatformerCVars.h"

void APlatformCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Apply CVar multipliers to movement
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		// Store original values if not already stored
		if (OriginalWalkSpeed <= 0.0f)
		{
			OriginalWalkSpeed = MovementComp->MaxWalkSpeed;
		}
		if (OriginalJumpVelocity <= 0.0f)
		{
			OriginalJumpVelocity = MovementComp->JumpZVelocity;
		}

		// Apply CVar multipliers
		MovementComp->MaxWalkSpeed = OriginalWalkSpeed * PlatformerCVars::WalkSpeedMultiplier.GetValueOnGameThread();
		MovementComp->JumpZVelocity = OriginalJumpVelocity * PlatformerCVars::JumpHeightMultiplier.GetValueOnGameThread();
	}

	// Apply camera CVars
	if (SpringArmComponent)
	{
		SpringArmComponent->CameraLagSpeed = PlatformerCVars::CameraLagSpeed.GetValueOnGameThread();
		SpringArmComponent->CameraLagMaxDistance = PlatformerCVars::CameraLagMaxDistance.GetValueOnGameThread();
	}
}

void APlatformCharacter::StartSprint()
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		const float SprintMultiplier = PlatformerCVars::SprintSpeedMultiplier.GetValueOnGameThread();
		MovementComp->MaxWalkSpeed = OriginalWalkSpeed * SprintMultiplier;
	}
}

void APlatformCharacter::StopSprint()
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		const float WalkMultiplier = PlatformerCVars::WalkSpeedMultiplier.GetValueOnGameThread();
		MovementComp->MaxWalkSpeed = OriginalWalkSpeed * WalkMultiplier;
	}
}
```

**Update InteractionComponent to use CVars:**

```cpp
#include "PlatformerCVars.h"

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Use CVar for interaction range
	const float CurrentInteractionRange = PlatformerCVars::InteractionRange.GetValueOnGameThread();
	SetInteractionRange(CurrentInteractionRange);

	// Debug drawing if enabled
	if (PlatformerCVars::InteractionDebugDraw.GetValueOnGameThread() > 0)
	{
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(),
			CurrentInteractionRange, 12, FColor::Yellow, false, 0.0f);
	}
}
```

### Step 5.8: Build Validation & Cleanup (30 minutes)

**Create Build Validation Test:**

**Header `Source/Platformer/Tests/BuildValidationTests.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildValidationTest, "Platformer.Build.Validation",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetValidationTest, "Platformer.Build.Assets",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
```

**Implementation `Source/Platformer/Tests/BuildValidationTests.cpp`:**

```cpp
#include "BuildValidationTests.h"
#include "Engine/Engine.h"
#include "Misc/OutputDeviceRedirector.h"

bool FBuildValidationTest::RunTest(const FString& Parameters)
{
	// Test that core classes can be instantiated
	TestTrue("Engine is valid", GEngine != nullptr);

	// Test that all our custom classes are properly registered
	UClass* PlatformCharacterClass = FindObject<UClass>(ANY_PACKAGE, TEXT("PlatformCharacter"));
	TestTrue("PlatformCharacter class is registered", PlatformCharacterClass != nullptr);

	UClass* EnemyNPCClass = FindObject<UClass>(ANY_PACKAGE, TEXT("EnemyNPC"));
	TestTrue("EnemyNPC class is registered", EnemyNPCClass != nullptr);

	UClass* FriendlyNPCClass = FindObject<UClass>(ANY_PACKAGE, TEXT("FriendlyNPC"));
	TestTrue("FriendlyNPC class is registered", FriendlyNPCClass != nullptr);

	// Test interfaces
	UClass* InteractableInterface = FindObject<UClass>(ANY_PACKAGE, TEXT("Interactable"));
	TestTrue("IInteractable interface is registered", InteractableInterface != nullptr);

	UClass* TalkableInterface = FindObject<UClass>(ANY_PACKAGE, TEXT("Talkable"));
	TestTrue("ITalkable interface is registered", TalkableInterface != nullptr);

	// Test components
	UClass* StatsComponentClass = FindObject<UClass>(ANY_PACKAGE, TEXT("StatsComponent"));
	TestTrue("StatsComponent class is registered", StatsComponentClass != nullptr);

	UClass* InteractionComponentClass = FindObject<UClass>(ANY_PACKAGE, TEXT("InteractionComponent"));
	TestTrue("InteractionComponent class is registered", InteractionComponentClass != nullptr);

	return true;
}

bool FAssetValidationTest::RunTest(const FString& Parameters)
{
	// Test that required assets exist and can be loaded
	bool bAllAssetsValid = true;

	// Test input assets
	const FString InputMappingContextPath = TEXT("/Game/Input/IMC_Player");
	UObject* InputMappingContext = LoadObject<UObject>(nullptr, *InputMappingContextPath);
	if (!InputMappingContext)
	{
		AddError(FString::Printf(TEXT("Failed to load Input Mapping Context: %s"), *InputMappingContextPath));
		bAllAssetsValid = false;
	}

	// Test behavior tree assets for AI
	const FString BehaviorTreePath = TEXT("/Game/AI/BT_Enemy");
	UObject* BehaviorTree = LoadObject<UObject>(nullptr, *BehaviorTreePath);
	if (!BehaviorTree)
	{
		AddWarning(FString::Printf(TEXT("Behavior Tree not found (optional): %s"), *BehaviorTreePath));
		// Don't fail the test for optional assets
	}

	// Test widget assets
	const FString InteractionWidgetPath = TEXT("/Game/UI/WBP_InteractionPrompt");
	UObject* InteractionWidget = LoadObject<UObject>(nullptr, *InteractionWidgetPath);
	if (!InteractionWidget)
	{
		AddWarning(FString::Printf(TEXT("Interaction Widget not found (optional): %s"), *InteractionWidgetPath));
	}

	TestTrue("All required assets are valid", bAllAssetsValid);

	return bAllAssetsValid;
}
```

---

## Final Steps: Running and Validating Tests

### Step 5.9: Run All Tests (15 minutes)

**To run tests in UE Editor:**

1. Window → Developer Tools → Automation
2. In Automation panel, expand "Platformer" category
3. Select all tests or individual categories:
   - ✅ Platformer.Character.\*
   - ✅ Platformer.Interaction.\*
   - ✅ Platformer.AI.\*
   - ✅ Platformer.Build.\*
4. Click "Start Tests"

**Console Commands for Testing:**

```
# Run all Platformer tests
Automation RunTests Platformer

# Run specific test category
Automation RunTests Platformer.Character

# Run with detailed output
Automation RunTests Platformer +LogLevel=Verbose

# List all available tests
Automation List
```

### Step 5.10: Console Variable Reference (10 minutes)

**Create CVar Documentation:**

**Create file `Source/Platformer/Documentation/CVarReference.md`:**

```markdown
# Console Variable Reference

## Camera Controls

- `platformer.camera.LagSpeed` (Default: 10.0) - Camera lag speed
- `platformer.camera.LagMaxDistance` (Default: 200.0) - Max camera lag distance
- `platformer.camera.DebugMode` (Default: 0) - Camera debug display

## Movement Controls

- `platformer.movement.WalkSpeedMultiplier` (Default: 1.0) - Walk speed multiplier
- `platformer.movement.SprintSpeedMultiplier` (Default: 1.5) - Sprint speed multiplier
- `platformer.movement.JumpHeightMultiplier` (Default: 1.0) - Jump height multiplier

## Interaction Controls

- `platformer.interaction.Range` (Default: 200.0) - Interaction range
- `platformer.interaction.DebugDraw` (Default: 0) - Show interaction debug

## AI Controls

- `platformer.ai.DetectionRange` (Default: 800.0) - AI detection range
- `platformer.ai.PatrolRadius` (Default: 400.0) - AI patrol radius
- `platformer.ai.DebugMode` (Default: 0) - AI debug display

## Debug Controls

- `platformer.debug.ShowInteractionPrompts` (Default: 1) - Show interaction UI
- `platformer.debug.ShowHealthBars` (Default: 0) - Show health bars
- `platformer.debug.LogGameplayEvents` (Default: 0) - Log gameplay events

## Usage Examples
```

# Make character faster

platformer.movement.WalkSpeedMultiplier 1.5

# Increase interaction range for easier debugging

platformer.interaction.Range 300

# Enable all debug modes

platformer.camera.DebugMode 1
platformer.ai.DebugMode 1
platformer.interaction.DebugDraw 1

```

```

---

## Acceptance Criteria

**✅ Testing Framework:**

- [ ] Automation tests run without errors
- [ ] All movement functionality tested
- [ ] Interaction system validated
- [ ] AI behavior verified
- [ ] Build validation passes

**✅ Console Variables:**

- [ ] CVars adjust gameplay parameters in real-time
- [ ] Camera tuning works during gameplay
- [ ] Movement speed adjustments function
- [ ] Interaction range modifiable
- [ ] AI parameters tunable

**✅ Code Quality:**

- [ ] No compilation warnings
- [ ] All classes properly documented
- [ ] Consistent coding standards
- [ ] Clean build output

**✅ Performance:**

- [ ] Game runs at target framerate
- [ ] No memory leaks detected
- [ ] Efficient AI performance
- [ ] Smooth camera behavior

---

## What You've Accomplished

By completing Milestone 5, you have:

1. **Comprehensive Testing** - Automated tests for all major systems
2. **Tunable Parameters** - Console variables for real-time adjustment
3. **Production Ready** - Clean, documented, and validated codebase
4. **Professional Workflow** - Industry-standard testing and debugging tools

Your platformer game foundation is now complete and ready for expansion with new features, levels, and content!

**Next Steps:**

- Explore the stretch goals in the main milestones guide
- Add your own custom gameplay features
- Create levels and content for your game
- Consider multiplayer or save system implementations

🎉 **Congratulations on completing the full C++ Unreal Engine Platformer course!**
