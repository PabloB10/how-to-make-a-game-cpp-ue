# Milestones

## Milestone 1 — Character + Camera + Input

**Estimated Time:** 6-8 hours over 2-3 sessions

**Quick Overview:**

- Create C++ project `Platformer`; enable Enhanced Input.
- Implement `APlatformCharacter` with SpringArm+Camera; camera centered.
- Input Mapping Context `IMC_Player` with actions: Move, Look, Jump, Interact, Sprint.
- Tune movement via `UCharacterTuningData` and `UCameraTuningData`.
- **Acceptance:** PIE starts with controllable player, camera centered, double jump and sprint.

**📋 [→ Detailed Step-by-Step Guide](/guide/milestone-1-detailed)**

## Milestone 2 — Interaction System + UI Prompt

**Estimated Time:** 4-6 hours over 2 sessions

**Overview:**

- `IInteractable` interface; `UInteractionComponent` with sphere trace.
- `AInteractiveActor` base; Pickup and Door examples.
- `WBP_InteractPrompt` shows verb+name on focus.
- **Acceptance:** Prompt appears within range; Interact performs action; hides out of range.

**📋 [→ Detailed Guide Coming Soon]**

## Milestone 3 — Stats + Damage + Enemy AI

**Estimated Time:** 8-10 hours over 3-4 sessions

**Overview:**

- `UStatsComponent` (Health/Stamina); damage and death.
- `IDamageable` interface; melee attack from enemy.
- `AEnemyNPC` with AIController, Perception (sight), Behavior Tree (patrol → chase → attack).
- **Acceptance:** Enemy patrols, chases on sight, deals damage, death triggers respawn/log.

**📋 [→ Detailed Guide Coming Soon]**

## Milestone 4 — Friendly NPC + Interactive Events

**Estimated Time:** 4-6 hours over 2 sessions

**Overview:**

- `AFriendlyNPC` as `IInteractable` (Talk); simple dialog log.
- `ATriggerEventActor` firing gameplay event on overlap; routed via subsystem.
- **Acceptance:** Talk works; entering trigger logs event with parameters.

**📋 [→ Detailed Guide Coming Soon]**

## Milestone 5 — Tests + Polish

**Estimated Time:** 6-8 hours over 2-3 sessions

**Overview:**

- Automation tests: movement, interaction prompt, AI chase.
- Cvars for camera lag and interaction radius.
- **Acceptance:** Tests pass; no warnings in build.

**📋 [→ Detailed Step-by-Step Guide](/guide/milestone-5-detailed)**

## Stretch Goals

- Save/load system with player progress
- Basic inventory system for pickups
- Simple ability system (cooldown-based actions)
- Data-table driven dialogs for NPCs
- Level progression and checkpoints
