# Milestone 1 - Starter Template

This template provides the foundational files to begin Milestone 1 implementation.

## What's Included

- **PlatformCharacter.h/.cpp** - Base character class structure
- **Input Action Assets** - Enhanced Input configuration files
- **Camera Tuning Data** - Data asset for camera settings
- **Character Tuning Data** - Data asset for movement settings

## Download Template

📁 **[Download Template Files](https://github.com/your-repo/templates/milestone-1-starter)**

Or browse the files directly in the repository's `templates/milestone-1-starter/` directory.

## How to Use

1. Create a new UE C++ project named "Platformer"
2. Copy these template files to your project's Source folder
3. Follow the detailed milestone guide to implement functionality
4. Customize the tuning data assets for your game feel

## File Structure

```
Source/Platformer/
├── PlatformCharacter.h
├── PlatformCharacter.cpp
├── TuningData/
│   ├── CharacterTuningData.h
│   ├── CharacterTuningData.cpp
│   ├── CameraTuningData.h
│   └── CameraTuningData.cpp
└── Input/
    └── InputConfig.h
```

## Key Features

- ✅ Enhanced Input system integration
- ✅ Spring arm camera with lag settings
- ✅ Data-driven movement parameters
- ✅ Jump and sprint functionality foundation
- ✅ Configurable camera behavior

## Implementation Guide

After copying the template files:

1. **Follow the Detailed Guide**: [Milestone 1 - Step by Step](/guide/milestone-1-detailed)
2. **Implement Input Actions**: Set up Enhanced Input mapping context
3. **Configure Components**: Wire up spring arm and camera
4. **Tune Movement**: Adjust character and camera settings
5. **Test Functionality**: Verify movement, jumping, and camera work

## Next Steps

1. Test your character movement and camera
2. Proceed to [Milestone 2 Template](/templates/milestone-2-starter/) for interactions
3. Reference the [Complete Project](/templates/complete-project/) if you get stuck

## Support

If you encounter issues:

- Check the [Troubleshooting Guide](/guide/troubleshooting)
- Review [Common Mistakes](/guide/common-mistakes)
- Ensure all [Prerequisites](/guide/prerequisites) are met

## Code Preview

### PlatformCharacter.h (Header Structure)

```cpp
UCLASS()
class PLATFORMER_API APlatformCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlatformCharacter();

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    UCameraComponent* CameraComponent;

    // Input Actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* JumpAction;

    // ... more implementation details
};
```

This template provides the foundation - follow the detailed guide to complete the implementation!
