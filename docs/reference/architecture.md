# Architecture

This page summarizes systems and patterns used in the UE platformer.

The following content is sourced from `Docs/architecture.md`:

---

Guiding principles

- Composition over inheritance; small focused components.
- Data-driven design via Primary Data Assets (tuning); Blueprints for presentation.
- Clear interfaces for capabilities (Interactable, Damageable, Talkable, etc.).
- Event-driven updates; avoid Tick unless necessary.

Core modules/classes

- Character & Camera
  - APlatformCharacter (derived from ACharacter)
    - Components: USpringArmComponent, UCameraComponent, UStatsComponent, UInteractionComponent
    - Input: Enhanced Input (IMC_Player)
  - UPlayerCameraSettings (Data Asset) for camera tuning

- Interaction System
  - UInteractionComponent: performs line/sphere traces; manages current focus target
  - IInteractable: interface with GetInteractionVerb, CanInteract, Interact
  - Widget prompt: UUserWidget (WBP_InteractPrompt)

- Stats & Damage
  - UStatsComponent: Health, Stamina, optional Shield; regen settings
  - IDamageable: ApplyDamage, OnDeath delegate
  - Damage types via UDamageType subclasses (e.g., Melee, Ranged)

- NPCs
  - ABaseNPC: pawn/character with UStatsComponent, UCapsuleComponent, UWidgetComponent for markers
  - AFriendlyNPC: adds dialog capability (ITalkable)
  - AEnemyNPC: AIController + Behavior Tree (BT_Enemy), Blackboard (BB_Enemy)

- AI
  - Perception via UAIPerceptionComponent (sight)
  - BehaviorTree tasks/services for patrol, chase, attack
  - EQS optional for smarter movement

- World Interaction
  - AInteractiveActor: base for levers, pickups, doors; exposes IInteractable
  - ATriggerEventActor: fires gameplay events (delegates) when overlapped or interacted
  - Gameplay effects/actions defined as data (UPlatformEffect definition)

Data assets (examples)

- UCharacterTuningData: WalkSpeed, AirControl, JumpCount, GravityScale
- UCameraTuningData: TargetArmLength, Lag, RotationLag, FOV
- UEnemyTuningData: PerceptionRange, AttackRange, AttackCooldown, Damage
- UInteractableData: DisplayName, Verb, Cooldown, UIIcon, SFX, VFX

Subsystems

- UPlatformerGameInstanceSubsystem: global services (save/load, tag registry)
- UPlatformerWorldSubsystem: world-level systems (spawners, registries)

Testing

- Functional tests (Gauntlet or Automation Spec) for: movement, interaction prompt, enemy chase, damage/death.

Performance

- Avoid per-frame allocations; reserve arrays; reuse timers.
- Keep collision channels and queries lean; batch traces when possible.

Logging/Debug

- Categories: LogPlatformer, LogAI, LogInteraction
- Minimal on-screen debug via DrawDebug helpers, toggled via cvars.

Layers and responsibilities

- Input Layer: Enhanced Input mapping context and actions; converts raw input to intent (Move, Look, Jump, Interact, Sprint).
- Character Layer: Movement and camera; contains minimal state; delegates features to components.
- Systems Layer (Components/Interfaces): Interaction, Stats/Damage, Dialogue, Effects.
- AI Layer: Perception, Behavior Trees, Tasks/Services; interacts with Systems layer via interfaces.
- Data Layer: Primary Data Assets for tuning; no logic.

Interfaces (contracts)

- IInteractable
  - GetInteractionVerb() -> FText
  - CanInteract(AActor\* Instigator) -> bool
  - Interact(AActor\* Instigator) -> void
- IDamageable
  - ApplyDamage(const FDamageData&) -> float (actual applied)
  - OnDeath: Multicast delegate with killer/instigator
- ITalkable (optional)
  - BeginDialog(AActor\* Instigator) -> void

Dependencies and coupling

- Character depends on components via composition; systems accessed via interfaces (no hard cast to concrete types).
- AI depends on Blackboard keys and interfaces; no direct references to player character class.
- UI (prompts) depends on interfaces and data only; no heavy business logic.

Configuration and cvars (examples)

- platformer.CameraLag=12.0
- platformer.InteractionRadius=60.0
- ai.EnemySightRadius=1800

Error handling

- Use ensureMsgf for recoverable config mistakes; checkNoEntry for illegal paths.
- Validate Data Assets at load (OnAssetCreated/EditorValidator optional) and at BeginPlay.

Diagrams

System flow (high level)

```
Input (Enhanced Input)
	└─ IMC_Player (Move, Look, Jump, Interact, Sprint)
				│
				▼
Character (APlatformCharacter)
	├─ MovementComponent
	├─ Camera (SpringArm + CameraComponent)
	├─ UInteractionComponent ──► IInteractable targets
	└─ UStatsComponent ──► IDamageable (apply damage)
				│
				▼
AI / NPCs
	├─ AEnemyNPC + AIController + BT/BB + Perception
	└─ AFriendlyNPC (ITalkable)
```

Interaction flow

```
Tick/Timer/Trigger → UInteractionComponent → SphereTrace
	→ Focus target found? → Update prompt (UI)
	→ On Interact Action → IInteractable::CanInteract → IInteractable::Interact
		→ Execute effects (data-driven) → UI feedback (SFX/VFX optional)
```
