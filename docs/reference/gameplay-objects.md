# Gameplay Objects and Parameters

All parameters should be exposed via Primary Data Assets and/or UPROPERTY(EditDefaultsOnly) for designer tuning.

Character (APlatformCharacter)

- Movement
  - WalkSpeed: 600 uu/s (range 500–700)
  - SprintSpeed: 900 uu/s (range 800–1000)
  - AirControl: 0.5 (range 0.2–0.8)
  - JumpMaxCount: 2 (double jump)
  - GravityScale: 1.0 (range 0.9–1.3)
- Camera (UCameraComponent via SpringArm)
  - TargetArmLength: 450 (range 350–600)
  - bUsePawnControlRotation: false
  - LagSpeed: 12.0; RotationLagSpeed: 10.0 (lower=snappier)
  - FOV: 90 (range 80–100)
- Input (Enhanced Input)
  - Move: Axis2D (WASD/Stick)
  - Look: Axis2D (Mouse/RightStick)
  - Jump: Action (Press to Jump, Hold optional)
  - Interact: Action (E / X)
  - Sprint: Action (Toggle/Hold)

Interaction System

- Trace: Sphere, Radius 60 (range 40–80), Range 250 (range 200–350), Channel Visibility or custom Interactable
- UI Prompt: "{Verb} {Name}", show/hide on focus; 0.1s fade

Enemy (AEnemyNPC)

- Perception: Sight 1800/2200, FOV 60 deg (increase for outdoor spaces)
- Combat: AttackRange 200 (melee), Cooldown 1.2s, Damage 15
- Movement: Optional patrol points, AcceptanceRadius 60

Friendly NPC (AFriendlyNPC)

- Verb: Talk; DisplayName: e.g., Guide; DialogLines: array; CanTrade: false

Interactive Object (AInteractiveActor)

- DisplayName; Verb; Cooldown 0.5s; OneShot true/false
- Effects: ApplyStatDelta; GrantTag; TriggerEvent (ID)

Interactive Event (ATriggerEventActor)

- Trigger: Box/Sphere/Capsule; Pawn-only; Repeatable false
- Payload: Event ID (FName), Parameters (map), Delay 0.0

Stats (UStatsComponent)

- HealthMax 100; StaminaMax 100; Regen 10/s (delay 1.0s); Armor 0; OnDeath delegate

Damage/Hit Reactions

- DamageType: Melee/Ranged/Environmental; HitPause 0.05s; Knockback configurable

Save/Load (stretch)

- Save slots; player stats; one-shot interactable toggles.

Rationale

- Keep movement and camera responsive; tune lag and FOV for readability.
- Use sphere trace for forgiving interactions; prompts must be unambiguous.
- Enemy perception ranges should allow reaction time; telegraph attacks where possible.
- Data-drive everything configurable to enable fast iteration without recompiles.
