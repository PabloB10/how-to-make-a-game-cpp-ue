# Troubleshooting Guide

## Compilation Errors

### "Cannot find UnrealBuildTool.exe"

**Cause:** Visual Studio C++ workload not properly installed or UE can't find it.

**Solutions:**

1. Open Visual Studio Installer → Modify → Ensure "Game development with C++" workload is installed
2. Restart Epic Games Launcher after VS installation
3. Verify UE Editor → Edit → Editor Preferences → Source Code → Source Code Editor is set to Visual Studio 2022

### "Module 'Platformer' could not be loaded"

**Cause:** C++ compilation failed or module not properly built.

**Solutions:**

1. Close UE Editor completely
2. Delete `Binaries/` and `Intermediate/` folders from your project
3. Right-click `.uproject` file → "Generate Visual Studio Project Files"
4. Open `.sln` in Visual Studio → Build → Build Solution
5. If successful, launch UE project from Visual Studio (F5) or double-click `.uproject`

### "Unresolved external symbol" Errors

**Cause:** Missing module dependencies or incorrect linking.

**Solutions:**

1. Check `Source/Platformer/Platformer.Build.cs` includes required modules:
   ```csharp
   PublicDependencyModuleNames.AddRange(new string[] {
       "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
       "AIModule", "UMG", "GameplayTasks", "NavigationSystem"
   });
   ```
2. Ensure UCLASS/USTRUCT macros are properly declared
3. Verify forward declarations vs. full includes in headers

## Enhanced Input Issues

### "Input Action not found" or Input Not Responding

**Solutions:**

1. Verify Input Action assets exist in Content Browser
2. Check Input Mapping Context is created and configured
3. Ensure IMC is added to player controller:
   ```cpp
   // In BeginPlay or similar
   if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
       ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
   {
       Subsystem->AddMappingContext(InputMappingContext, 0);
   }
   ```
4. Verify action bindings in SetupPlayerInputComponent

### "Enhanced Input plugin not found"

**Solutions:**

1. UE Editor → Edit → Plugins → Search "Enhanced Input" → Enable
2. Restart editor when prompted
3. If still missing, verify UE5.1+ (Enhanced Input built-in from 5.1)

## Git LFS Problems

### "Git LFS not tracking files" or Large Files in Regular Git

**Solutions:**

1. Verify Git LFS installed: `git lfs version`
2. Initialize in repo: `git lfs install`
3. Check `.gitattributes` file exists with proper patterns:
   ```txt
   *.uasset filter=lfs diff=lfs merge=lfs -text
   *.umap filter=lfs diff=lfs merge=lfs -text
   ```
4. Migrate existing files: `git lfs migrate import --include="*.uasset,*.umap"`

### "Git LFS bandwidth limit exceeded"

**Solutions:**

- Use GitHub Pro for increased LFS bandwidth
- Or consider alternative: GitLab (free LFS) or Azure DevOps
- Minimize asset size during development (use placeholders)

## Runtime Issues

### "Character doesn't move" in PIE

**Debugging checklist:**

1. Check Enhanced Input setup (common cause)
2. Verify `SetupPlayerInputComponent` calls parent: `Super::SetupPlayerInputComponent(PlayerInputComponent);`
3. Ensure player controller possesses the character
4. Check movement component settings (not disabled)
5. Add debug logs in Move function:
   ```cpp
   UE_LOG(LogTemp, Warning, TEXT("Move called with value: %s"), *Value.ToString());
   ```

### "Camera not following character"

**Solutions:**

1. Verify SpringArm and Camera components attached correctly
2. Check SpringArm collision (might be hitting world geometry)
3. Ensure camera lag settings are reasonable (not 0, not too high)
4. Verify `bUsePawnControlRotation = false` on SpringArm for platformer camera

### "AI not working" - Enemy doesn't move or react

**Debugging steps:**

1. Check AI Controller assigned to enemy pawn
2. Verify Behavior Tree asset exists and is valid
3. Ensure NavMesh exists in level (Build → Build Navigation)
4. Check Blackboard keys are correctly named and typed
5. Add debug logging in BT tasks
6. Use AI Debugging tools: Gameplay Debugger (apostrophe key)

## Plugin Issues

### "Plugin 'XXX' failed to load"

**Solutions:**

1. UE Editor → Edit → Plugins → Search plugin name → Enable
2. Restart editor when prompted
3. For code plugins, ensure proper module dependencies in `.Build.cs`
4. Check plugin compatibility with UE version

### Missing UMG/Widget functionality

**Solutions:**

1. Enable UMG plugin: Edit → Plugins → "UMG" → Enable
2. Add "UMG" to PublicDependencyModuleNames in Build.cs
3. Include headers: `#include "Components/WidgetComponent.h"`

## Performance Issues

### "Slow compilation times"

**Solutions:**

1. Use SSD for UE installation and projects
2. Exclude project folders from antivirus real-time scanning
3. Close unnecessary applications during compilation
4. Use Unity builds in Build.cs (for larger projects):
   ```csharp
   bUseUnityBuild = true;
   ```

### "Editor running slowly"

**Solutions:**

1. Lower editor quality: Settings → Engine Scalability Settings → Low
2. Disable real-time rendering in viewport
3. Close unnecessary editor tabs/windows
4. Consider using smaller test levels during development

## Project Structure Issues

### "Can't find header files" or IntelliSense errors

**Solutions:**

1. Regenerate project files: Right-click `.uproject` → Generate VS Project Files
2. Refresh VS Code: Ctrl+Shift+P → "C/C++: Refresh IntelliSense"
3. Verify proper include paths in VS Code settings
4. Use forward declarations in headers, full includes in .cpp files

## Getting Help

**When stuck:**

1. Check UE5 Documentation: [docs.unrealengine.com](https://docs.unrealengine.com/)
2. Unreal Engine Discord/Forums community
3. Search specific error messages on AnswerHub/Reddit
4. Use UE's built-in debugging tools (Output Log, Blueprint Debugger, etc.)

**Before asking for help:**

1. Include your UE version, VS version, and OS
2. Provide the exact error message
3. Share relevant code snippets
4. Mention what you've already tried

**Debugging mindset:**

- Make small, incremental changes
- Test frequently
- Use logging extensively during development
- Isolate problems (disable other systems to test one thing)
- Keep a working backup before major changes
