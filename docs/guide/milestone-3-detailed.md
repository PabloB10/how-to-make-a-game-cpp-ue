# Milestone 3 - Detailed Implementation

**Goal:** Implement player stats (health/stamina), damage system, and basic enemy AI with patrol and combat

**Estimated Time:** 8-10 hours over 3-4 sessions

## Overview Checklist

- [ ] Create UStatsComponent for health/stamina management
- [ ] Implement IDamageable interface for damage handling
- [ ] Build AEnemyNPC with AI controller and behavior tree
- [ ] Set up AI perception for sight-based detection
- [ ] Create patrol and combat behaviors
- [ ] Test enemy AI lifecycle and player interaction

---

## Session 1: Stats & Damage System (2-3 hours)

### Step 3.1: Create UStatsComponent (60 minutes)

**Create Component Class:**

1. Tools → New C++ Class → Actor Component → Next
2. Name: `StatsComponent`

**Header `Source/Platformer/StatsComponent.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth, float, HealthPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina, float, StaminaPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeath, AActor*, Killer, AActor*, Victim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRevived, AActor*, Victim);

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDamageType> DamageType = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Instigator = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* DamageCauser = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HitLocation = FVector::ZeroVector;

	FDamageInfo()
	{
		Amount = 0.0f;
		DamageType = nullptr;
		Instigator = nullptr;
		DamageCauser = nullptr;
		HitLocation = FVector::ZeroVector;
	}

	FDamageInfo(float InAmount, AActor* InInstigator = nullptr, AActor* InDamageCauser = nullptr)
		: Amount(InAmount), Instigator(InInstigator), DamageCauser(InDamageCauser)
	{
		DamageType = nullptr;
		HitLocation = FVector::ZeroVector;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLATFORMER_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Health settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "1", ClampMax = "1000"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "0", ClampMax = "50"))
	float HealthRegenRate = 0.0f; // Per second

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "0", ClampMax = "10"))
	float HealthRegenDelay = 3.0f; // Seconds after taking damage

	// Stamina settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "1", ClampMax = "1000"))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0", ClampMax = "100"))
	float StaminaRegenRate = 20.0f; // Per second

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0", ClampMax = "5"))
	float StaminaRegenDelay = 1.0f; // Seconds after using stamina

	// Current values
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float CurrentStamina;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	bool bIsDead = false;

private:
	float LastDamageTime = 0.0f;
	float LastStaminaUseTime = 0.0f;
	bool bHealthRegenEnabled = true;
	bool bStaminaRegenEnabled = true;

public:
	// Events
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStaminaChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRevived OnRevived;

	// Health functions
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float ApplyDamage(const FDamageInfo& DamageInfo);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RestoreHealth(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetMaxHealth(float NewMaxHealth, bool bHealToMax = false);

	// Stamina functions
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool ConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RestoreStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetStamina(float NewStamina);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetMaxStamina(float NewMaxStamina, bool bRestoreToMax = false);

	// Death/Revival
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Kill(AActor* Killer = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Revive(float HealthPercentage = 1.0f);

	// Status checks
	UFUNCTION(BlueprintPure, Category = "Stats")
	bool IsAlive() const { return !bIsDead && CurrentHealth > 0.0f; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHealthPercentage() const { return MaxHealth > 0 ? CurrentHealth / MaxHealth : 0.0f; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetStaminaPercentage() const { return MaxStamina > 0 ? CurrentStamina / MaxStamina : 0.0f; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	bool HasEnoughStamina(float RequiredAmount) const { return CurrentStamina >= RequiredAmount; }

	// Getters
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxStamina() const { return MaxStamina; }

private:
	void HandleHealthRegen(float DeltaTime);
	void HandleStaminaRegen(float DeltaTime);
	void NotifyHealthChanged();
	void NotifyStaminaChanged();
};
```

**Implementation `Source/Platformer/StatsComponent.cpp`:**

```cpp
#include "StatsComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Update 10 times per second
}

void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize current values to max
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	bIsDead = false;

	// Notify initial values
	NotifyHealthChanged();
	NotifyStaminaChanged();

	UE_LOG(LogTemp, Log, TEXT("StatsComponent initialized for %s - Health: %.1f/%.1f, Stamina: %.1f/%.1f"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"),
		CurrentHealth, MaxHealth, CurrentStamina, MaxStamina);
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsDead)
	{
		HandleHealthRegen(DeltaTime);
		HandleStaminaRegen(DeltaTime);
	}
}

void UStatsComponent::HandleHealthRegen(float DeltaTime)
{
	if (!bHealthRegenEnabled || HealthRegenRate <= 0.0f || CurrentHealth >= MaxHealth)
		return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastDamageTime < HealthRegenDelay)
		return;

	float RegenAmount = HealthRegenRate * DeltaTime;
	SetHealth(CurrentHealth + RegenAmount);
}

void UStatsComponent::HandleStaminaRegen(float DeltaTime)
{
	if (!bStaminaRegenEnabled || StaminaRegenRate <= 0.0f || CurrentStamina >= MaxStamina)
		return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastStaminaUseTime < StaminaRegenDelay)
		return;

	float RegenAmount = StaminaRegenRate * DeltaTime;
	SetStamina(CurrentStamina + RegenAmount);
}

float UStatsComponent::ApplyDamage(const FDamageInfo& DamageInfo)
{
	if (bIsDead || DamageInfo.Amount <= 0.0f)
		return 0.0f;

	float DamageToApply = FMath::Min(DamageInfo.Amount, CurrentHealth);
	float NewHealth = CurrentHealth - DamageToApply;

	SetHealth(NewHealth);
	LastDamageTime = GetWorld()->GetTimeSeconds();

	// Check for death
	if (CurrentHealth <= 0.0f && !bIsDead)
	{
		Kill(DamageInfo.Instigator);
	}

	UE_LOG(LogTemp, Log, TEXT("%s took %.1f damage from %s (Health: %.1f/%.1f)"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"),
		DamageToApply,
		DamageInfo.Instigator ? *DamageInfo.Instigator->GetName() : TEXT("Unknown"),
		CurrentHealth, MaxHealth);

	return DamageToApply;
}

void UStatsComponent::RestoreHealth(float HealAmount)
{
	if (bIsDead || HealAmount <= 0.0f)
		return;

	SetHealth(CurrentHealth + HealAmount);

	UE_LOG(LogTemp, Log, TEXT("%s healed for %.1f (Health: %.1f/%.1f)"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"),
		HealAmount, CurrentHealth, MaxHealth);
}

void UStatsComponent::SetHealth(float NewHealth)
{
	float PrevHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);

	if (CurrentHealth != PrevHealth)
	{
		NotifyHealthChanged();
	}
}

void UStatsComponent::SetMaxHealth(float NewMaxHealth, bool bHealToMax)
{
	MaxHealth = FMath::Max(NewMaxHealth, 1.0f);

	if (bHealToMax)
	{
		SetHealth(MaxHealth);
	}
	else
	{
		// Clamp current health to new max
		SetHealth(CurrentHealth);
	}
}

bool UStatsComponent::ConsumeStamina(float Amount)
{
	if (Amount <= 0.0f || CurrentStamina < Amount)
		return false;

	SetStamina(CurrentStamina - Amount);
	LastStaminaUseTime = GetWorld()->GetTimeSeconds();

	return true;
}

void UStatsComponent::RestoreStamina(float Amount)
{
	if (Amount <= 0.0f)
		return;

	SetStamina(CurrentStamina + Amount);
}

void UStatsComponent::SetStamina(float NewStamina)
{
	float PrevStamina = CurrentStamina;
	CurrentStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);

	if (CurrentStamina != PrevStamina)
	{
		NotifyStaminaChanged();
	}
}

void UStatsComponent::SetMaxStamina(float NewMaxStamina, bool bRestoreToMax)
{
	MaxStamina = FMath::Max(NewMaxStamina, 1.0f);

	if (bRestoreToMax)
	{
		SetStamina(MaxStamina);
	}
	else
	{
		SetStamina(CurrentStamina);
	}
}

void UStatsComponent::Kill(AActor* Killer)
{
	if (bIsDead)
		return;

	bIsDead = true;
	CurrentHealth = 0.0f;

	NotifyHealthChanged();
	OnDeath.Broadcast(Killer, GetOwner());

	UE_LOG(LogTemp, Warning, TEXT("%s died (killed by %s)"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"),
		Killer ? *Killer->GetName() : TEXT("Unknown"));
}

void UStatsComponent::Revive(float HealthPercentage)
{
	if (!bIsDead)
		return;

	bIsDead = false;
	float ReviveHealth = MaxHealth * FMath::Clamp(HealthPercentage, 0.1f, 1.0f);
	SetHealth(ReviveHealth);

	OnRevived.Broadcast(GetOwner());

	UE_LOG(LogTemp, Log, TEXT("%s revived with %.1f health"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"), CurrentHealth);
}

void UStatsComponent::NotifyHealthChanged()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetHealthPercentage());
}

void UStatsComponent::NotifyStaminaChanged()
{
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina, GetStaminaPercentage());
}
```

### Step 3.2: Create IDamageable Interface (30 minutes)

**Create Interface:**

1. Tools → New C++ Class → Interface → Next
2. Name: `Damageable`

**Header `Source/Platformer/Damageable.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatsComponent.h"
#include "Damageable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

class PLATFORMER_API IDamageable
{
	GENERATED_BODY()

public:
	// Apply damage to this object
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	float TakeDamage(const FDamageInfo& DamageInfo);
	virtual float TakeDamage_Implementation(const FDamageInfo& DamageInfo) { return 0.0f; }

	// Check if this object can take damage
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	bool CanTakeDamage() const;
	virtual bool CanTakeDamage_Implementation() const { return true; }

	// Get the current health percentage (0.0 to 1.0)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	float GetHealthPercentage() const;
	virtual float GetHealthPercentage_Implementation() const { return 1.0f; }

	// Check if this object is alive
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	bool IsAlive() const;
	virtual bool IsAlive_Implementation() const { return true; }
};
```

**Implementation `Source/Platformer/Damageable.cpp`:**

```cpp
#include "Damageable.h"

// Interface implementation is in header for this simple case
```

### Step 3.3: Integrate Stats with Character (30 minutes)

**Update `PlatformCharacter.h`:**

```cpp
// Add includes
#include "StatsComponent.h"
#include "Damageable.h"

// Make character implement IDamageable
class PLATFORMER_API APlatformCharacter : public ACharacter, public IDamageable
{
    // ... existing content ...

private:
    // Add stats component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    UStatsComponent* StatsComponent;

public:
    // IDamageable interface
    virtual float TakeDamage_Implementation(const FDamageInfo& DamageInfo) override;
    virtual bool CanTakeDamage_Implementation() const override;
    virtual float GetHealthPercentage_Implementation() const override;
    virtual bool IsAlive_Implementation() const override;

    // Stats event handlers
    UFUNCTION()
    void OnHealthChanged(float CurrentHealth, float MaxHealth, float HealthPercentage);

    UFUNCTION()
    void OnPlayerDeath(AActor* Killer, AActor* Victim);

    // Getter
    FORCEINLINE UStatsComponent* GetStatsComponent() const { return StatsComponent; }
};
```

**Update `PlatformCharacter.cpp`:**

```cpp
// In constructor:
StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));

// In BeginPlay:
if (StatsComponent)
{
    StatsComponent->OnHealthChanged.AddDynamic(this, &APlatformCharacter::OnHealthChanged);
    StatsComponent->OnDeath.AddDynamic(this, &APlatformCharacter::OnPlayerDeath);
}

// Add implementations:
float APlatformCharacter::TakeDamage_Implementation(const FDamageInfo& DamageInfo)
{
    if (StatsComponent)
    {
        return StatsComponent->ApplyDamage(DamageInfo);
    }
    return 0.0f;
}

bool APlatformCharacter::CanTakeDamage_Implementation() const
{
    return StatsComponent && StatsComponent->IsAlive();
}

float APlatformCharacter::GetHealthPercentage_Implementation() const
{
    return StatsComponent ? StatsComponent->GetHealthPercentage() : 0.0f;
}

bool APlatformCharacter::IsAlive_Implementation() const
{
    return StatsComponent ? StatsComponent->IsAlive() : false;
}

void APlatformCharacter::OnHealthChanged(float CurrentHealth, float MaxHealth, float HealthPercentage)
{
    UE_LOG(LogTemp, Log, TEXT("Player health changed: %.1f/%.1f (%.1f%%)"),
        CurrentHealth, MaxHealth, HealthPercentage * 100.0f);

    // TODO: Update health UI here
}

void APlatformCharacter::OnPlayerDeath(AActor* Killer, AActor* Victim)
{
    UE_LOG(LogTemp, Warning, TEXT("Player died! Killed by: %s"),
        Killer ? *Killer->GetName() : TEXT("Unknown"));

    // TODO: Handle player death (respawn, game over, etc.)

    // Simple respawn after 3 seconds
    GetWorld()->GetTimerManager().SetTimer(
        FTimerHandle(),
        [this]()
        {
            if (StatsComponent)
            {
                StatsComponent->Revive(1.0f);
                UE_LOG(LogTemp, Log, TEXT("Player respawned"));
            }
        },
        3.0f,
        false
    );
}
```

### Step 3.4: Build and Test Stats System (30 minutes)

**Compile and Test:**

1. Build → Build Solution
2. Should compile successfully
3. Test in PIE - character should have stats component

**Test with console commands:**
Create a simple test command to damage the player:

```cpp
// Add to PlatformCharacter.cpp for testing
#if UE_BUILD_DEBUG
static TAutoConsoleVariable<float> CVarTestDamage(
    TEXT("platformer.TestDamage"),
    10.0f,
    TEXT("Test damage amount to apply to player"),
    ECVF_Default
);
#endif
```

---

## Session 2: Enemy AI Foundation (2-3 hours)

### Step 3.5: Create Base Enemy Class (60 minutes)

**Create Enemy Character:**

1. Tools → New C++ Class → Character → Next
2. Name: `EnemyNPC`

**Header `Source/Platformer/EnemyNPC.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Damageable.h"
#include "EnemyNPC.generated.h"

class UStatsComponent;
class UAIPerceptionComponent;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Idle,
    Patrolling,
    Investigating,
    Chasing,
    Attacking,
    Dead
};

UCLASS()
class PLATFORMER_API AEnemyNPC : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	AEnemyNPC();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStatsComponent* StatsComponent;

	// AI Settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBlackboardAsset* BlackboardAsset;

	// Combat Settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "50", ClampMax = "500"))
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "5", ClampMax = "100"))
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.5", ClampMax = "5.0"))
	float AttackCooldown = 1.5f;

	// Patrol Settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
	TArray<AActor*> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol", meta = (ClampMin = "1", ClampMax = "10"))
	float PatrolWaitTime = 2.0f;

	// Current State
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EEnemyState CurrentState = EEnemyState::Idle;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	AActor* CurrentTarget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	float LastAttackTime = 0.0f;

public:
	// IDamageable interface
	virtual float TakeDamage_Implementation(const FDamageInfo& DamageInfo) override;
	virtual bool CanTakeDamage_Implementation() const override;
	virtual float GetHealthPercentage_Implementation() const override;
	virtual bool IsAlive_Implementation() const override;

	// AI Functions
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetCurrentTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetCurrentState(EEnemyState NewState);

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool CanAttackTarget() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void PerformAttack();

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetNextPatrolPoint();

	// Getters
	UFUNCTION(BlueprintPure, Category = "AI")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	UFUNCTION(BlueprintPure, Category = "AI")
	EEnemyState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetAttackRange() const { return AttackRange; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsInAttackRange() const;

protected:
	// Event handlers
	UFUNCTION()
	void OnEnemyDeath(AActor* Killer, AActor* Victim);

	UFUNCTION()
	void OnEnemyHealthChanged(float CurrentHealth, float MaxHealth, float HealthPercentage);

private:
	int32 CurrentPatrolIndex = 0;
};
```

**Implementation `Source/Platformer/EnemyNPC.cpp`:**

```cpp
#include "EnemyNPC.h"
#include "StatsComponent.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Damageable.h"

AEnemyNPC::AEnemyNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create stats component
	StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);

	// Configure collision
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// Set AI controller class
	AIControllerClass = AAIController::StaticClass();
}

void AEnemyNPC::BeginPlay()
{
	Super::BeginPlay();

	// Bind stats events
	if (StatsComponent)
	{
		StatsComponent->OnDeath.AddDynamic(this, &AEnemyNPC::OnEnemyDeath);
		StatsComponent->OnHealthChanged.AddDynamic(this, &AEnemyNPC::OnEnemyHealthChanged);
	}

	UE_LOG(LogTemp, Log, TEXT("EnemyNPC %s spawned"), *GetName());
}

void AEnemyNPC::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Start AI behavior tree
	if (AAIController* AIController = Cast<AAIController>(NewController))
	{
		if (BehaviorTree)
		{
			AIController->RunBehaviorTree(BehaviorTree);
			UE_LOG(LogTemp, Log, TEXT("EnemyNPC %s started behavior tree"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("EnemyNPC %s has no behavior tree assigned!"), *GetName());
		}
	}
}

float AEnemyNPC::TakeDamage_Implementation(const FDamageInfo& DamageInfo)
{
	if (!StatsComponent)
		return 0.0f;

	float DamageApplied = StatsComponent->ApplyDamage(DamageInfo);

	// Set attacker as target if we're not dead
	if (DamageApplied > 0.0f && StatsComponent->IsAlive() && DamageInfo.Instigator)
	{
		SetCurrentTarget(DamageInfo.Instigator);
		SetCurrentState(EEnemyState::Chasing);
	}

	return DamageApplied;
}

bool AEnemyNPC::CanTakeDamage_Implementation() const
{
	return StatsComponent && StatsComponent->IsAlive();
}

float AEnemyNPC::GetHealthPercentage_Implementation() const
{
	return StatsComponent ? StatsComponent->GetHealthPercentage() : 0.0f;
}

bool AEnemyNPC::IsAlive_Implementation() const
{
	return StatsComponent ? StatsComponent->IsAlive() : false;
}

void AEnemyNPC::SetCurrentTarget(AActor* NewTarget)
{
	CurrentTarget = NewTarget;

	// Update blackboard
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsObject(TEXT("TargetActor"), NewTarget);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("EnemyNPC %s target set to %s"),
		*GetName(), NewTarget ? *NewTarget->GetName() : TEXT("None"));
}

void AEnemyNPC::SetCurrentState(EEnemyState NewState)
{
	if (CurrentState != NewState)
	{
		EEnemyState PrevState = CurrentState;
		CurrentState = NewState;

		// Update blackboard
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
			{
				BlackboardComp->SetValueAsEnum(TEXT("CurrentState"), static_cast<uint8>(NewState));
			}
		}

		UE_LOG(LogTemp, Log, TEXT("EnemyNPC %s state changed from %d to %d"),
			*GetName(), static_cast<int32>(PrevState), static_cast<int32>(NewState));
	}
}

bool AEnemyNPC::CanAttackTarget() const
{
	if (!CurrentTarget || !IsAlive())
		return false;

	// Check cooldown
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackCooldown)
		return false;

	// Check range
	return IsInAttackRange();
}

void AEnemyNPC::PerformAttack()
{
	if (!CanAttackTarget())
		return;

	LastAttackTime = GetWorld()->GetTimeSeconds();

	// Apply damage to target if it implements IDamageable
	if (CurrentTarget && CurrentTarget->Implements<UDamageable>())
	{
		FDamageInfo DamageInfo(AttackDamage, this, this);
		float DamageDealt = IDamageable::Execute_TakeDamage(CurrentTarget, DamageInfo);

		UE_LOG(LogTemp, Log, TEXT("EnemyNPC %s attacked %s for %.1f damage"),
			*GetName(), *CurrentTarget->GetName(), DamageDealt);
	}

	// TODO: Play attack animation/effects
}

AActor* AEnemyNPC::GetNextPatrolPoint()
{
	if (PatrolPoints.Num() == 0)
		return nullptr;

	CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
	return PatrolPoints[CurrentPatrolIndex];
}

bool AEnemyNPC::IsInAttackRange() const
{
	if (!CurrentTarget)
		return false;

	float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
	return Distance <= AttackRange;
}

void AEnemyNPC::OnEnemyDeath(AActor* Killer, AActor* Victim)
{
	SetCurrentState(EEnemyState::Dead);
	SetCurrentTarget(nullptr);

	UE_LOG(LogTemp, Warning, TEXT("EnemyNPC %s died"), *GetName());

	// Disable collision and movement
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	// TODO: Play death animation/effects

	// Destroy after delay
	GetWorld()->GetTimerManager().SetTimer(
		FTimerHandle(),
		[this]()
		{
			Destroy();
		},
		5.0f,
		false
	);
}

void AEnemyNPC::OnEnemyHealthChanged(float CurrentHealth, float MaxHealth, float HealthPercentage)
{
	// TODO: Update health bar UI if needed
	if (HealthPercentage < 0.3f)
	{
		// Low health behavior could be implemented here
	}
}
```

### Step 3.6: Create AI Controller (45 minutes)

**Create AI Controller:**

1. Tools → New C++ Class → AI Controller → Next
2. Name: `EnemyAIController`

**Header `Source/Platformer/EnemyAIController.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class PLATFORMER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	// AI Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

	// Perception settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
	float SightRadius = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
	float LoseSightRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
	float PeripheralVisionAngleDegrees = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
	float MaxAge = 5.0f;

public:
	// Getters
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComponent; }
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorTreeComponent; }

private:
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UAISenseConfig_Sight* SightConfig;
};
```

**Implementation `Source/Platformer/EnemyAIController.cpp`:**

```cpp
#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnemyNPC.h"
#include "PlatformCharacter.h"
#include "Damageable.h"

AEnemyAIController::AEnemyAIController()
{
	// Create components
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	// Create perception component
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// Configure sight sense
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(MaxAge);
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;

	// Add sight to perception component
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Bind perception events
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AEnemyNPC* Enemy = Cast<AEnemyNPC>(InPawn))
	{
		// Initialize blackboard
		if (Enemy->BlackboardAsset && BlackboardComponent)
		{
			UseBlackboard(Enemy->BlackboardAsset);

			// Set initial values
			BlackboardComponent->SetValueAsEnum(TEXT("CurrentState"), static_cast<uint8>(EEnemyState::Idle));
			BlackboardComponent->SetValueAsObject(TEXT("SelfActor"), Enemy);
		}

		// Start behavior tree
		if (Enemy->BehaviorTree && BehaviorTreeComponent)
		{
			RunBehaviorTree(Enemy->BehaviorTree);
		}
	}
}

void AEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	AEnemyNPC* ControlledEnemy = Cast<AEnemyNPC>(GetPawn());
	if (!ControlledEnemy || !ControlledEnemy->IsAlive())
		return;

	// Look for player characters
	for (AActor* Actor : UpdatedActors)
	{
		if (APlatformCharacter* Player = Cast<APlatformCharacter>(Actor))
		{
			// Check if we can see the player
			FActorPerceptionBlueprintInfo Info;
			AIPerceptionComponent->GetActorsPerception(Player, Info);

			if (Info.LastSensedStimuli.Num() > 0)
			{
				const FAIStimulus& Stimulus = Info.LastSensedStimuli[0];

				if (Stimulus.WasSuccessfullySensed())
				{
					// Player spotted!
					ControlledEnemy->SetCurrentTarget(Player);
					ControlledEnemy->SetCurrentState(EEnemyState::Chasing);

					// Update blackboard
					if (BlackboardComponent)
					{
						BlackboardComponent->SetValueAsVector(TEXT("TargetLocation"), Player->GetActorLocation());
						BlackboardComponent->SetValueAsVector(TEXT("LastKnownTargetLocation"), Player->GetActorLocation());
					}

					UE_LOG(LogTemp, Log, TEXT("EnemyAI %s spotted player %s"),
						*ControlledEnemy->GetName(), *Player->GetName());
				}
				else
				{
					// Lost sight of player
					ControlledEnemy->SetCurrentTarget(nullptr);
					ControlledEnemy->SetCurrentState(EEnemyState::Investigating);

					UE_LOG(LogTemp, Log, TEXT("EnemyAI %s lost sight of player"), *ControlledEnemy->GetName());
				}
			}
		}
	}
}
```

### Step 3.7: Update Enemy to Use Custom AI Controller (15 minutes)

**Update `EnemyNPC.cpp` constructor:**

```cpp
// Replace the line:
// AIControllerClass = AAIController::StaticClass();

// With:
AIControllerClass = AEnemyAIController::StaticClass();
```

**Update Build.cs to include AI modules:**

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
    "AIModule", "UMG", "GameplayTasks", "NavigationSystem",
    "GameplayTasks", "NavigationSystem"
});
```

### Step 3.8: Build and Test Enemy Foundation (30 minutes)

**Compile and Test:**

1. Build → Build Solution
2. Should compile successfully
3. Create Blueprint based on `AEnemyNPC`: `BP_Enemy`
4. Test spawning enemy in level - should have stats and AI controller

---

## Session 3: Behavior Tree & Patrol System (2-3 hours)

---

## Session 3: Behavior Tree & Patrol System (2-3 hours)

### Step 3.9: Create Blackboard Asset (15 minutes)

**Create Blackboard:**

1. Content Browser → Add → Artificial Intelligence → Blackboard
2. Name: `BB_Enemy`
3. Open and add these keys:
   - `CurrentState` (Enum - create new enum `EEnemyState`)
   - `SelfActor` (Object - Actor class)
   - `TargetActor` (Object - Actor class)
   - `TargetLocation` (Vector)
   - `LastKnownTargetLocation` (Vector)
   - `PatrolPoint` (Object - Actor class)
   - `IsInAttackRange` (Bool)
   - `CanAttack` (Bool)

### Step 3.10: Create Behavior Tree (60 minutes)

**Create Behavior Tree:**

1. Content Browser → Add → Artificial Intelligence → Behavior Tree
2. Name: `BT_Enemy`
3. Set Blackboard Asset to `BB_Enemy`

**Behavior Tree Structure:**

```
Root
└── Selector
    ├── Sequence [Death Check]
    │   ├── Blackboard Decorator [CurrentState == Dead]
    │   └── Wait [Duration: 1.0]
    ├── Sequence [Combat]
    │   ├── Blackboard Decorator [CurrentState == Attacking]
    │   ├── Wait [Duration: 0.5]
    │   └── Task: Attack Target
    ├── Sequence [Chase]
    │   ├── Blackboard Decorator [CurrentState == Chasing]
    │   ├── Move To [TargetLocation]
    │   └── Task: Check Attack Range
    ├── Sequence [Investigate]
    │   ├── Blackboard Decorator [CurrentState == Investigating]
    │   ├── Move To [LastKnownTargetLocation]
    │   ├── Wait [Duration: 2.0]
    │   └── Task: Return to Patrol
    └── Sequence [Patrol]
        ├── Task: Get Next Patrol Point
        ├── Move To [PatrolPoint]
        ├── Wait [Duration: 2.0]
        └── Task: Continue Patrol
```

### Step 3.11: Create Custom Behavior Tree Tasks (90 minutes)

**Create Attack Task:**

Create `Source/Platformer/BTTask_AttackTarget.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackTarget.generated.h"

UCLASS()
class PLATFORMER_API UBTTask_AttackTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AttackTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
```

**Implementation `Source/Platformer/BTTask_AttackTarget.cpp`:**

```cpp
#include "BTTask_AttackTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "EnemyNPC.h"

UBTTask_AttackTarget::UBTTask_AttackTarget()
{
	NodeName = "Attack Target";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	AEnemyNPC* Enemy = Cast<AEnemyNPC>(AIController->GetPawn());
	if (!Enemy)
		return EBTNodeResult::Failed;

	if (Enemy->CanAttackTarget())
	{
		Enemy->PerformAttack();

		// Update blackboard
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsBool(TEXT("CanAttack"), false);
		}

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
```

**Create Check Attack Range Task:**

Create `Source/Platformer/BTTask_CheckAttackRange.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CheckAttackRange.generated.h"

UCLASS()
class PLATFORMER_API UBTTask_CheckAttackRange : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_CheckAttackRange();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
```

**Implementation `Source/Platformer/BTTask_CheckAttackRange.cpp`:**

```cpp
#include "BTTask_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "EnemyNPC.h"

UBTTask_CheckAttackRange::UBTTask_CheckAttackRange()
{
	NodeName = "Check Attack Range";
}

EBTNodeResult::Type UBTTask_CheckAttackRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	AEnemyNPC* Enemy = Cast<AEnemyNPC>(AIController->GetPawn());
	if (!Enemy)
		return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
		return EBTNodeResult::Failed;

	bool bInRange = Enemy->IsInAttackRange();
	bool bCanAttack = Enemy->CanAttackTarget();

	BlackboardComp->SetValueAsBool(TEXT("IsInAttackRange"), bInRange);
	BlackboardComp->SetValueAsBool(TEXT("CanAttack"), bCanAttack);

	if (bInRange)
	{
		Enemy->SetCurrentState(EEnemyState::Attacking);
	}

	return EBTNodeResult::Succeeded;
}
```

**Create Get Next Patrol Point Task:**

Create `Source/Platformer/BTTask_GetNextPatrolPoint.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetNextPatrolPoint.generated.h"

UCLASS()
class PLATFORMER_API UBTTask_GetNextPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GetNextPatrolPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
```

**Implementation `Source/Platformer/BTTask_GetNextPatrolPoint.cpp`:**

```cpp
#include "BTTask_GetNextPatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "EnemyNPC.h"

UBTTask_GetNextPatrolPoint::UBTTask_GetNextPatrolPoint()
{
	NodeName = "Get Next Patrol Point";
}

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	AEnemyNPC* Enemy = Cast<AEnemyNPC>(AIController->GetPawn());
	if (!Enemy)
		return EBTNodeResult::Failed;

	AActor* NextPatrolPoint = Enemy->GetNextPatrolPoint();
	if (!NextPatrolPoint)
	{
		// No patrol points - stay idle
		Enemy->SetCurrentState(EEnemyState::Idle);
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TEXT("PatrolPoint"), NextPatrolPoint);
		BlackboardComp->SetValueAsVector(TEXT("TargetLocation"), NextPatrolPoint->GetActorLocation());
	}

	Enemy->SetCurrentState(EEnemyState::Patrolling);
	return EBTNodeResult::Succeeded;
}
```

### Step 3.12: Setup Behavior Tree in Blueprint (30 minutes)

**Configure BT_Enemy:**

1. Open `BT_Enemy`
2. Build the tree structure as described above
3. Configure decorators with correct blackboard keys
4. Set task parameters (wait durations, move distances, etc.)

**Behavior Tree Node Configuration:**

- **Move To tasks**: Set Blackboard Key to `TargetLocation`
- **Wait tasks**: Set appropriate durations (0.5s for attack, 2s for patrol/investigate)
- **Blackboard decorators**: Use correct key names and comparison values

---

## Session 4: Combat Integration & Final Testing (2-3 hours)

### Step 3.13: Create Patrol Points (15 minutes)

**Setup Patrol System:**

1. Create Blueprint class based on Actor: `BP_PatrolPoint`
2. Add a simple mesh (cube or sphere) for visualization
3. Add a Billboard component for editor visibility
4. Place multiple patrol points in your test level

### Step 3.14: Configure Enemy Blueprint (30 minutes)

**Setup BP_Enemy:**

1. Open `BP_Enemy` (based on `AEnemyNPC`)
2. Set component values:
   - **Stats Component**: Health = 60, no regen
   - **Behavior Tree**: `BT_Enemy`
   - **Blackboard Asset**: `BB_Enemy`
   - **Attack Range**: 150
   - **Attack Damage**: 25
   - **Attack Cooldown**: 1.5
3. Assign patrol points in the `Patrol Points` array
4. Add a simple mesh (capsule or character mesh)
5. Configure collision properly

### Step 3.15: Test Combat System (45 minutes)

**Create Test Console Commands:**

Add to `PlatformCharacter.cpp`:

```cpp
#if UE_BUILD_DEBUG
// Test commands for development
static FAutoConsoleCommand TestDamagePlayerCommand(
	TEXT("platformer.TestDamagePlayer"),
	TEXT("Apply test damage to player"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (APlatformCharacter* Player = Cast<APlatformCharacter>(World->GetFirstPlayerController()->GetPawn()))
			{
				float Damage = Args.Num() > 0 ? FCString::Atof(*Args[0]) : 20.0f;
				FDamageInfo DamageInfo(Damage);
				Player->TakeDamage_Implementation(DamageInfo);
			}
		}
	})
);

static FAutoConsoleCommand TestSpawnEnemyCommand(
	TEXT("platformer.TestSpawnEnemy"),
	TEXT("Spawn test enemy near player"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (APlatformCharacter* Player = Cast<APlatformCharacter>(World->GetFirstPlayerController()->GetPawn()))
			{
				FVector SpawnLocation = Player->GetActorLocation() + FVector(300, 0, 0);
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				// You'll need to load the blueprint class here
				// For now, just log that the command was called
				UE_LOG(LogTemp, Warning, TEXT("Test spawn enemy command called"));
			}
		}
	})
);
#endif
```

**Test Scenarios:**

1. **Basic AI**: Enemy should patrol between points when no player in sight
2. **Player Detection**: Enemy should chase when player comes within sight range
3. **Combat**: Enemy should attack when in range, respecting cooldown
4. **Damage Response**: Enemy should target player who damages them
5. **Death**: Enemy should die when health reaches 0, clean up properly

### Step 3.16: Create Basic Health UI (60 minutes)

**Create Health Bar Widget:**

Create `Source/Platformer/HealthBarWidget.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class PLATFORMER_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateHealth(float CurrentHealth, float MaxHealth, float HealthPercentage);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHealthBarColor(FLinearColor Color);
};
```

**Implementation `Source/Platformer/HealthBarWidget.cpp`:**

```cpp
#include "HealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Set initial color
	if (HealthBar)
	{
		HealthBar->SetFillColorAndOpacity(FLinearColor::Green);
	}
}

void UHealthBarWidget::UpdateHealth(float CurrentHealth, float MaxHealth, float HealthPercentage)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercentage);

		// Change color based on health percentage
		FLinearColor BarColor = FLinearColor::Green;
		if (HealthPercentage < 0.3f)
			BarColor = FLinearColor::Red;
		else if (HealthPercentage < 0.6f)
			BarColor = FLinearColor::Yellow;

		HealthBar->SetFillColorAndOpacity(BarColor);
	}

	if (HealthText)
	{
		FString HealthString = FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth);
		HealthText->SetText(FText::FromString(HealthString));
	}
}

void UHealthBarWidget::SetHealthBarColor(FLinearColor Color)
{
	if (HealthBar)
	{
		HealthBar->SetFillColorAndOpacity(Color);
	}
}
```

**Create Widget Blueprint:**

1. Content Browser → Add → User Interface → Widget Blueprint
2. Name: `WBP_HealthBar`
3. Parent Class: `HealthBarWidget`
4. Add Progress Bar and Text Block, bind them in Blueprint

**Integrate with Character:**

Update `PlatformCharacter.h`:

```cpp
// Add to protected members
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
TSubclassOf<UHealthBarWidget> HealthBarWidgetClass;

UPROPERTY()
UHealthBarWidget* HealthBarWidget;
```

Update `PlatformCharacter.cpp`:

```cpp
// In BeginPlay:
if (HealthBarWidgetClass)
{
	HealthBarWidget = CreateWidget<UHealthBarWidget>(GetWorld(), HealthBarWidgetClass);
	if (HealthBarWidget)
	{
		HealthBarWidget->AddToViewport();
	}
}

// Update OnHealthChanged:
void APlatformCharacter::OnHealthChanged(float CurrentHealth, float MaxHealth, float HealthPercentage)
{
	UE_LOG(LogTemp, Log, TEXT("Player health changed: %.1f/%.1f (%.1f%%)"),
		CurrentHealth, MaxHealth, HealthPercentage * 100.0f);

	if (HealthBarWidget)
	{
		HealthBarWidget->UpdateHealth(CurrentHealth, MaxHealth, HealthPercentage);
	}
}
```

### Step 3.17: Final Integration & Testing (45 minutes)

**Level Setup:**

1. Place `BP_Enemy` in level with patrol points assigned
2. Ensure NavMesh covers the area (Build → Build Navigation)
3. Set `BP_PlatformCharacter` to have health bar widget class assigned
4. Test all systems together

**Comprehensive Test Checklist:**

**AI Behavior:**

- [ ] Enemy patrols between points when undisturbed ✅
- [ ] Enemy detects player within sight range ✅
- [ ] Enemy chases player when detected ✅
- [ ] Enemy attacks when in range ✅
- [ ] Enemy returns to patrol after losing player ✅

**Combat System:**

- [ ] Player takes damage from enemy attacks ✅
- [ ] Enemy takes damage when attacked ✅
- [ ] Health UI updates correctly ✅
- [ ] Death states work properly ✅
- [ ] Respawn system functions ✅

**Performance:**

- [ ] No frame rate drops with multiple enemies ✅
- [ ] AI behavior tree runs efficiently ✅
- [ ] Perception system doesn't cause hitches ✅

**Error Handling:**

- [ ] No errors in Output Log ✅
- [ ] Graceful handling of missing components ✅
- [ ] Proper cleanup on death/destruction ✅

### Step 3.18: Optimization & Polish (30 minutes)

**Performance Tweaks:**

```cpp
// In EnemyAIController constructor, optimize perception:
SightConfig->SetMaxAge(3.0f); // Reduce from 5.0f
AIPerceptionComponent->SetPeripheralVisionAngle(30.0f); // Reduce from 60.0f

// In StatsComponent, reduce tick frequency for enemies:
PrimaryComponentTick.TickInterval = 0.2f; // 5 times per second instead of 10
```

**Debug Visualization:**
Add console variables for debugging:

```cpp
// Add to EnemyNPC.cpp
#if UE_BUILD_DEBUG
static TAutoConsoleVariable<bool> CVarShowEnemyDebug(
	TEXT("platformer.ShowEnemyDebug"),
	false,
	TEXT("Show enemy AI debug information"),
	ECVF_Default
);
#endif
```

## Acceptance Criteria Verification

**Final Testing Checklist:**

- [ ] Enemy patrols between assigned points ✅
- [ ] Enemy chases player on sight within configured range ✅
- [ ] Enemy deals damage to player when in attack range ✅
- [ ] Player health UI updates correctly ✅
- [ ] Death triggers respawn/log messages ✅
- [ ] Multiple enemies can operate independently ✅
- [ ] Performance remains stable with 3-5 enemies ✅
- [ ] No compilation warnings or runtime errors ✅

## Common Issues & Solutions

**Enemy doesn't move:**

- Check NavMesh exists and covers patrol area
- Verify patrol points are assigned and accessible
- Ensure AI Controller is properly set

**Enemy doesn't detect player:**

- Check sight range and peripheral vision settings
- Verify perception component is configured
- Test with different player positions

**Combat doesn't work:**

- Verify both player and enemy implement IDamageable
- Check attack range and cooldown settings
- Ensure stats components are properly initialized

**Performance issues:**

- Reduce perception update frequency
- Limit number of simultaneous enemies
- Optimize behavior tree complexity

## Next Steps

Once Milestone 3 is complete:

1. Commit your code to Git
2. Test with different enemy configurations
3. Consider adding enemy types with different behaviors
4. Begin [Milestone 4: Friendly NPCs + Interactive Events](/guide/milestone-4-detailed)

**Git Commit Message Example:**

```
Milestone 3: Implement stats, damage, and enemy AI system

- Create UStatsComponent with health/stamina and regeneration
- Add IDamageable interface for consistent damage handling
- Build AEnemyNPC with AI controller and behavior tree
- Set up AI perception for sight-based player detection
- Create patrol, chase, and attack behaviors with state management
- Integrate combat system with health UI and death/respawn logic
- Add custom behavior tree tasks for enemy actions
- Optimize performance with configurable tick rates and perception settings
```
