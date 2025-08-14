# Console Variables (CVars) Guide

Console Variables provide runtime control over game parameters without recompilation. Essential for tuning and debugging.

---

## Implementation Strategy

### 1. Global CVars Module

**Create `Source/Platformer/Utilities/PlatformerCVars.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"

class PLATFORMER_API FPlatformerCVars
{
public:
    // Character Movement
    static TAutoConsoleVariable<float> CharacterWalkSpeed;
    static TAutoConsoleVariable<float> CharacterSprintSpeed;
    static TAutoConsoleVariable<float> CharacterJumpHeight;
    static TAutoConsoleVariable<int32> CharacterJumpCount;
    
    // Camera
    static TAutoConsoleVariable<float> CameraDistance;
    static TAutoConsoleVariable<float> CameraLagSpeed;
    static TAutoConsoleVariable<float> CameraRotationLag;
    static TAutoConsoleVariable<float> CameraFOV;
    
    // Interaction System
    static TAutoConsoleVariable<float> InteractionRange;
    static TAutoConsoleVariable<float> InteractionRadius;
    static TAutoConsoleVariable<bool> ShowInteractionDebug;
    static TAutoConsoleVariable<float> InteractionCooldown;
    
    // AI System
    static TAutoConsoleVariable<float> EnemySightRange;
    static TAutoConsoleVariable<float> EnemyHearingRange;
    static TAutoConsoleVariable<float> EnemyAttackRange;
    static TAutoConsoleVariable<float> EnemyPatrolSpeed;
    static TAutoConsoleVariable<float> EnemyChaseSpeed;
    
    // Debug and Performance
    static TAutoConsoleVariable<bool> ShowDebugInfo;
    static TAutoConsoleVariable<bool> ShowPerformanceStats;
    static TAutoConsoleVariable<int32> MaxEnemyCount;
    static TAutoConsoleVariable<bool> EnableAI;
    
    // Initialize all CVars
    static void Initialize();
};
```

**Implementation `Source/Platformer/Utilities/PlatformerCVars.cpp`:**

```cpp
#include "PlatformerCVars.h"

// Character Movement CVars
TAutoConsoleVariable<float> FPlatformerCVars::CharacterWalkSpeed(
    TEXT("platformer.Character.WalkSpeed"),
    600.0f,
    TEXT("Character walking speed"),
    ECVF_Cheat | ECVF_Scalability
);

TAutoConsoleVariable<float> FPlatformerCVars::CharacterSprintSpeed(
    TEXT("platformer.Character.SprintSpeed"),
    900.0f,
    TEXT("Character sprinting speed"),
    ECVF_Cheat | ECVF_Scalability
);

TAutoConsoleVariable<float> FPlatformerCVars::CharacterJumpHeight(
    TEXT("platformer.Character.JumpHeight"),
    700.0f,
    TEXT("Character jump Z velocity"),
    ECVF_Cheat
);

TAutoConsoleVariable<int32> FPlatformerCVars::CharacterJumpCount(
    TEXT("platformer.Character.JumpCount"),
    2,
    TEXT("Maximum number of jumps (1 = no double jump)"),
    ECVF_Cheat
);

// Camera CVars
TAutoConsoleVariable<float> FPlatformerCVars::CameraDistance(
    TEXT("platformer.Camera.Distance"),
    450.0f,
    TEXT("Camera boom arm length"),
    ECVF_Cheat
);

TAutoConsoleVariable<float> FPlatformerCVars::CameraLagSpeed(
    TEXT("platformer.Camera.LagSpeed"),
    12.0f,
    TEXT("Camera position lag speed"),
    ECVF_Cheat
);

TAutoConsoleVariable<float> FPlatformerCVars::CameraRotationLag(
    TEXT("platformer.Camera.RotationLag"),
    10.0f,
    TEXT("Camera rotation lag speed"),
    ECVF_Cheat
);

TAutoConsoleVariable<float> FPlatformerCVars::CameraFOV(
    TEXT("platformer.Camera.FOV"),
    90.0f,
    TEXT("Camera field of view"),
    ECVF_Cheat
);

// Interaction System CVars
TAutoConsoleVariable<float> FPlatformerCVars::InteractionRange(
    TEXT("platformer.Interaction.Range"),
    400.0f,
    TEXT("Maximum interaction trace distance"),
    ECVF_Cheat
);

TAutoConsoleVariable<float> FPlatformerCVars::InteractionRadius(
    TEXT("platformer.Interaction.Radius"),
    30.0f,
    TEXT("Interaction sphere trace radius"),
    ECVF_Cheat
);

TAutoConsoleVariable<bool> FPlatformerCVars::ShowInteractionDebug(
    TEXT("platformer.Interaction.ShowDebug"),
    false,
    TEXT("Show interaction system debug visualization"),
    ECVF_Cheat
);

TAutoConsoleVariable<float> FPlatformerCVars::InteractionCooldown(
    TEXT("platformer.Interaction.Cooldown"),
    0.5f,
    TEXT("Minimum time between interactions"),
    ECVF_Cheat
);

// AI System CVars
TAutoConsoleVariable<float> FPlatformerCVars::EnemySightRange(
    TEXT("platformer.AI.SightRange"),
    1800.0f,
    TEXT("Enemy AI sight perception range"),
    ECVF_Cheat
);

TAutoConsoleVariable<float> FPlatformerCVars::EnemyHearingRange(
    TEXT("platformer.AI.HearingRange"),
    1000.0f,
    TEXT("Enemy AI hearing perception range"),
    ECVF_Cheat
);

TAutoConsoleVariable<float> FPlatformerCVars::EnemyAttackRange(
    TEXT("platformer.AI.AttackRange"),
    150.0f,
    TEXT("Enemy AI attack range"),
    ECVF_Cheat
);

TAutoConsoleVariable<float> FPlatformerCVars::EnemyPatrolSpeed(
    TEXT("platformer.AI.PatrolSpeed"),
    200.0f,
    TEXT("Enemy AI patrol movement speed"),
    ECVF_Cheat
);

TAutoConsoleVariable<float> FPlatformerCVars::EnemyChaseSpeed(
    TEXT("platformer.AI.ChaseSpeed"),
    500.0f,
    TEXT("Enemy AI chase movement speed"),
    ECVF_Cheat
);

// Debug and Performance CVars
TAutoConsoleVariable<bool> FPlatformerCVars::ShowDebugInfo(
    TEXT("platformer.Debug.ShowInfo"),
    false,
    TEXT("Show general debug information overlay"),
    ECVF_Cheat
);

TAutoConsoleVariable<bool> FPlatformerCVars::ShowPerformanceStats(
    TEXT("platformer.Debug.ShowPerformance"),
    false,
    TEXT("Show performance statistics"),
    ECVF_Cheat
);

TAutoConsoleVariable<int32> FPlatformerCVars::MaxEnemyCount(
    TEXT("platformer.Performance.MaxEnemies"),
    20,
    TEXT("Maximum number of active enemies"),
    ECVF_Scalability
);

TAutoConsoleVariable<bool> FPlatformerCVars::EnableAI(
    TEXT("platformer.AI.Enable"),
    true,
    TEXT("Enable/disable AI processing"),
    ECVF_Cheat
);

void FPlatformerCVars::Initialize()
{
    // CVars are automatically registered via TAutoConsoleVariable
    // This function can be used for any additional setup
    UE_LOG(LogTemp, Log, TEXT("Platformer CVars initialized"));
}
```

---

## Integration with Game Systems

### 1. Character Movement Integration

**In `APlatformCharacter::BeginPlay()`:**

```cpp
void APlatformCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Apply CVar values at startup
    ApplyCVarSettings();
    
    // Register for CVar change notifications
    RegisterCVarCallbacks();
}

void APlatformCharacter::ApplyCVarSettings()
{
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->MaxWalkSpeed = FPlatformerCVars::CharacterWalkSpeed.GetValueOnGameThread();
        MovementComp->JumpZVelocity = FPlatformerCVars::CharacterJumpHeight.GetValueOnGameThread();
        MovementComp->JumpMaxCount = FPlatformerCVars::CharacterJumpCount.GetValueOnGameThread();
        
        // Update sprint speed reference
        SprintSpeed = FPlatformerCVars::CharacterSprintSpeed.GetValueOnGameThread();
    }
    
    UE_LOG(LogTemp, Log, TEXT("Applied CVar settings to character %s"), *GetName());
}

void APlatformCharacter::RegisterCVarCallbacks()
{
    // Register callback for dynamic updates
    FPlatformerCVars::CharacterWalkSpeed->AsVariable()->SetOnChangedCallback(
        FConsoleVariableDelegate::CreateUObject(this, &APlatformCharacter::OnWalkSpeedChanged)
    );
    
    FPlatformerCVars::CharacterSprintSpeed->AsVariable()->SetOnChangedCallback(
        FConsoleVariableDelegate::CreateUObject(this, &APlatformCharacter::OnSprintSpeedChanged)
    );
}

void APlatformCharacter::OnWalkSpeedChanged(IConsoleVariable* Variable)
{
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        float NewSpeed = Variable->GetFloat();
        MovementComp->MaxWalkSpeed = NewSpeed;
        
        // If not sprinting, update current speed
        if (!bIsSprinting)
        {
            WalkSpeed = NewSpeed;
        }
        
        UE_LOG(LogTemp, Log, TEXT("Walk speed changed to %.1f for %s"), NewSpeed, *GetName());
    }
}

void APlatformCharacter::OnSprintSpeedChanged(IConsoleVariable* Variable)
{
    float NewSprintSpeed = Variable->GetFloat();
    SprintSpeed = NewSprintSpeed;
    
    // If currently sprinting, apply immediately
    if (bIsSprinting)
    {
        if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
        {
            MovementComp->MaxWalkSpeed = NewSprintSpeed;
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Sprint speed changed to %.1f for %s"), NewSprintSpeed, *GetName());
}
```

### 2. Camera System Integration

**In Camera Setup:**

```cpp
void APlatformCharacter::UpdateCameraSettings()
{
    if (CameraBoom)
    {
        CameraBoom->TargetArmLength = FPlatformerCVars::CameraDistance.GetValueOnGameThread();
        CameraBoom->CameraLagSpeed = FPlatformerCVars::CameraLagSpeed.GetValueOnGameThread();
        CameraBoom->CameraRotationLagSpeed = FPlatformerCVars::CameraRotationLag.GetValueOnGameThread();
    }
    
    if (FollowCamera)
    {
        FollowCamera->SetFieldOfView(FPlatformerCVars::CameraFOV.GetValueOnGameThread());
    }
}

// Call this in Tick for real-time updates (or use callbacks)
void APlatformCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update camera settings if CVars changed
    static float LastCameraDistance = -1.0f;
    float CurrentCameraDistance = FPlatformerCVars::CameraDistance.GetValueOnGameThread();
    
    if (!FMath::IsNearlyEqual(LastCameraDistance, CurrentCameraDistance))
    {
        UpdateCameraSettings();
        LastCameraDistance = CurrentCameraDistance;
    }
}
```

### 3. Interaction System Integration

**In `UInteractionComponent`:**

```cpp
void UInteractionComponent::UpdateFocusActor()
{
    // Use CVar values for range and radius
    InteractionRange = FPlatformerCVars::InteractionRange.GetValueOnGameThread();
    InteractionRadius = FPlatformerCVars::InteractionRadius.GetValueOnGameThread();
    bShowDebugTraces = FPlatformerCVars::ShowInteractionDebug.GetValueOnGameThread();
    
    // Rest of implementation...
}

bool UInteractionComponent::CanInteractNow() const
{
    // Use CVar for cooldown
    float CooldownTime = FPlatformerCVars::InteractionCooldown.GetValueOnGameThread();
    float TimeSinceLastInteraction = GetWorld()->GetTimeSeconds() - LastInteractionTime;
    
    return TimeSinceLastInteraction >= CooldownTime;
}
```

---

## CVar Categories and Organization

### By System

```cpp
// Movement
platformer.Character.WalkSpeed
platformer.Character.SprintSpeed
platformer.Character.JumpHeight
platformer.Character.JumpCount

// Camera
platformer.Camera.Distance
platformer.Camera.LagSpeed
platformer.Camera.RotationLag
platformer.Camera.FOV

// Interaction
platformer.Interaction.Range
platformer.Interaction.Radius
platformer.Interaction.ShowDebug
platformer.Interaction.Cooldown

// AI
platformer.AI.SightRange
platformer.AI.HearingRange
platformer.AI.AttackRange
platformer.AI.PatrolSpeed
platformer.AI.ChaseSpeed
platformer.AI.Enable

// Debug
platformer.Debug.ShowInfo
platformer.Debug.ShowPerformance

// Performance
platformer.Performance.MaxEnemies
```

---

## Advanced CVar Features

### 1. CVar Presets

**Create preset configurations:**

```cpp
// Console command to apply preset configurations
class FPlatformerCVarPresets
{
public:
    static void ApplyDevPreset()
    {
        FPlatformerCVars::ShowDebugInfo->AsVariable()->Set(true, ECVF_SetByConsole);
        FPlatformerCVars::ShowInteractionDebug->AsVariable()->Set(true, ECVF_SetByConsole);
        FPlatformerCVars::CharacterWalkSpeed->AsVariable()->Set(800.0f, ECVF_SetByConsole);
        
        UE_LOG(LogTemp, Log, TEXT("Applied development preset"));
    }
    
    static void ApplyProductionPreset()
    {
        FPlatformerCVars::ShowDebugInfo->AsVariable()->Set(false, ECVF_SetByConsole);
        FPlatformerCVars::ShowInteractionDebug->AsVariable()->Set(false, ECVF_SetByConsole);
        FPlatformerCVars::MaxEnemyCount->AsVariable()->Set(10, ECVF_SetByConsole);
        
        UE_LOG(LogTemp, Log, TEXT("Applied production preset"));
    }
    
    static void ApplyPerformancePreset()
    {
        FPlatformerCVars::MaxEnemyCount->AsVariable()->Set(5, ECVF_SetByConsole);
        FPlatformerCVars::EnableAI->AsVariable()->Set(false, ECVF_SetByConsole);
        FPlatformerCVars::CameraLagSpeed->AsVariable()->Set(20.0f, ECVF_SetByConsole);
        
        UE_LOG(LogTemp, Log, TEXT("Applied performance preset"));
    }
};

// Register console commands
static FAutoConsoleCommand ApplyDevPresetCommand(
    TEXT("platformer.preset.dev"),
    TEXT("Apply development preset settings"),
    FConsoleCommandDelegate::CreateStatic(&FPlatformerCVarPresets::ApplyDevPreset)
);

static FAutoConsoleCommand ApplyProdPresetCommand(
    TEXT("platformer.preset.production"),
    TEXT("Apply production preset settings"),
    FConsoleCommandDelegate::CreateStatic(&FPlatformerCVarPresets::ApplyProductionPreset)
);

static FAutoConsoleCommand ApplyPerfPresetCommand(
    TEXT("platformer.preset.performance"),
    TEXT("Apply performance preset settings"),
    FConsoleCommandDelegate::CreateStatic(&FPlatformerCVarPresets::ApplyPerformancePreset)
);
```

### 2. CVar Validation

```cpp
// Custom CVar with validation
class FValidatedFloatCVar
{
private:
    TAutoConsoleVariable<float> CVar;
    float MinValue;
    float MaxValue;
    
public:
    FValidatedFloatCVar(const TCHAR* Name, float DefaultValue, const TCHAR* Help, 
                       float InMinValue, float InMaxValue)
        : CVar(Name, DefaultValue, Help, ECVF_Cheat)
        , MinValue(InMinValue)
        , MaxValue(InMaxValue)
    {
        CVar->AsVariable()->SetOnChangedCallback(
            FConsoleVariableDelegate::CreateRaw(this, &FValidatedFloatCVar::OnValueChanged)
        );
    }
    
    void OnValueChanged(IConsoleVariable* Variable)
    {
        float Value = Variable->GetFloat();
        if (Value < MinValue || Value > MaxValue)
        {
            float ClampedValue = FMath::Clamp(Value, MinValue, MaxValue);
            Variable->Set(ClampedValue, ECVF_SetByConsole);
            
            UE_LOG(LogTemp, Warning, TEXT("CVar value %.2f out of range [%.2f, %.2f], clamped to %.2f"),
                Value, MinValue, MaxValue, ClampedValue);
        }
    }
    
    float GetValue() const { return CVar.GetValueOnGameThread(); }
    IConsoleVariable* AsVariable() { return CVar.AsVariable(); }
};

// Usage
static FValidatedFloatCVar ValidatedWalkSpeed(
    TEXT("platformer.Character.WalkSpeedValidated"),
    600.0f,
    TEXT("Character walk speed (validated)"),
    100.0f,  // Min
    2000.0f  // Max
);
```

---

## Console Usage Examples

### Runtime Tuning Commands

```cpp
// Basic usage
platformer.Character.WalkSpeed 800
platformer.Camera.Distance 300
platformer.Interaction.ShowDebug true

// Batch commands
platformer.preset.dev
platformer.preset.performance

// Debug commands
platformer.Debug.ShowInfo true
stat fps
stat memory

// AI tuning
platformer.AI.SightRange 1200
platformer.AI.ChaseSpeed 600
platformer.AI.Enable false

// Performance testing
platformer.Performance.MaxEnemies 5
platformer.Debug.ShowPerformance true
```

### Save/Load CVar Configurations

```cpp
// Save current settings
exec Platformer_SaveCVars

// Load saved settings
exec Platformer_LoadCVars

// Implementation
static FAutoConsoleCommand SaveCVarsCommand(
    TEXT("Platformer_SaveCVars"),
    TEXT("Save current CVar values to config"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        GConfig->SetFloat(TEXT("Platformer"), TEXT("WalkSpeed"), 
            FPlatformerCVars::CharacterWalkSpeed.GetValueOnGameThread(), GGameUserSettingsIni);
        GConfig->SetFloat(TEXT("Platformer"), TEXT("CameraDistance"), 
            FPlatformerCVars::CameraDistance.GetValueOnGameThread(), GGameUserSettingsIni);
        // ... save other CVars
        
        GConfig->Flush(false, GGameUserSettingsIni);
        UE_LOG(LogTemp, Log, TEXT("CVars saved to config"));
    })
);
```

---

## Best Practices

### 1. Naming Conventions
- Use hierarchical naming: `system.subsystem.property`
- Keep names descriptive but concise
- Use consistent capitalization

### 2. Value Ranges
- Always consider valid ranges for numeric CVars
- Implement validation for critical values
- Provide reasonable defaults

### 3. Performance
- Use callbacks sparingly in hot paths
- Cache CVar values when appropriate
- Consider using ECVF_Scalability for performance-related CVars

### 4. Documentation
- Provide clear help text for each CVar
- Document expected value ranges
- Include usage examples

CVars are powerful tools for real-time tuning and debugging. Use them wisely to create a professional, tunable game experience.
