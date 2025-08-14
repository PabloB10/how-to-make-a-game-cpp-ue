# C++ + Unreal Primer

Key UE-C++ concepts

- Reflection: UCLASS/USTRUCT/UENUM/UPROPERTY/UFUNCTION enable editor/serialization/GC.
- Memory: UObject is GC-managed; non-UObject types use RAII; prefer TUniquePtr/TSharedPtr.
- Modules: Build.cs defines dependencies; keep includes in .cpp, forward-declare in headers.
- Gameplay Framework: GameMode, GameState, PlayerController, PlayerState, Pawn/Character.
- Components: Reusable behavior attached to Actors; favor this over deep inheritance.

Syntax and idioms

- Use const correctness; pass by const reference or value (small POD) where appropriate.
- Prefer ranges-based for where available; avoid dynamic_cast; use Cast<> and Interfaces.
- Avoid exceptions; UE uses ensure/check/log and error codes.
- Timers via FTimerManager; Delegates for eventing; Multicast for signals.

Patterns in this project

- Component-driven: UStatsComponent, UInteractionComponent, UHealthBarComponent (optional UI)
- Interfaces for capabilities: IInteractable, IDamageable, ITalkable
- Data assets for tuning; blueprints extend visuals/FX only.

Build and tooling

- Use VS 2022 + Epic toolchain. Run Build from UE or UnrealBuildTool. UHT must succeed.
- Add modules: "EnhancedInput", "AIModule", "UMG", "GameplayTasks", "GameplayTags" (optional), "NavigationSystem".

Testing

- Use Automation Spec (C++), or Functional Tests (Blueprint) triggered in PIE.
- Prefer fast, deterministic tests; avoid latent tests where possible.

Do/Don’t

- Do keep .h small and stable; move impl to .cpp.
- Do annotate UPROPERTY with EditDefaultsOnly, EditAnywhere, BlueprintReadOnly appropriately.
- Don’t tick by default; use events and timers.
- Don’t hold raw owning pointers to UObjects; rely on UPROPERTY or TWeakObjectPtr.
