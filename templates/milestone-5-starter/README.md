# Milestone 5 - Starter Template

This template provides the testing and polish foundation for Milestone 5.

## What's Included

- **Test Framework** - Automation test base classes
- **Test Suites** - Movement, interaction, and AI tests
- **Console Variables** - Runtime tuning system
- **Debug Tools** - Visualization and logging helpers
- **Build Validation** - Asset and code verification

## Prerequisites

- Completed Milestones 1-4
- Full game systems implemented and working

## How to Use

1. Ensure Milestones 1-4 are complete
2. Copy these template files to your project
3. Follow the detailed [Milestone 5 Guide](/guide/milestone-5-detailed)
4. Implement testing and polish systems

## File Structure

```
Source/Platformer/
├── Tests/
│   ├── PlatformerTestBase.h/.cpp
│   ├── MovementTests.cpp
│   ├── InteractionTests.cpp
│   ├── AITests.cpp
│   └── BuildValidationTests.cpp
├── Utilities/
│   ├── PlatformerCVars.h/.cpp
│   ├── DebugHelpers.h/.cpp
│   └── GameplayHelpers.h/.cpp
└── Documentation/
    └── CVarReference.md
```

## Key Features to Implement

- [ ] Automated test suite
- [ ] Console variable system
- [ ] Debug visualization tools
- [ ] Performance monitoring
- [ ] Build validation
- [ ] Code documentation and cleanup

## Testing Your Implementation

1. Run automation tests in UE Editor
2. Use console variables to tune gameplay
3. Enable debug visualizations
4. Check build for warnings/errors
5. Validate all systems work together

## Next Steps

After completing this milestone:

- Your game foundation is complete!
- Add custom gameplay features
- Create levels and content
- Consider stretch goals from the main guide

## Console Variables Reference

Key CVars to implement:

- `platformer.camera.LagSpeed` - Camera responsiveness
- `platformer.interaction.Range` - Interaction distance
- `platformer.movement.WalkSpeedMultiplier` - Movement tuning
- `platformer.ai.DetectionRange` - AI perception range
- `platformer.debug.*` - Various debug tools
