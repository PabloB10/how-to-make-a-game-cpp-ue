# Milestone 4 - Starter Template

This template provides the friendly NPC and events foundation for Milestone 4.

## What's Included

- **AFriendlyNPC** - Talking NPC base class
- **ITalkable Interface** - For dialog interactions
- **Dialog System** - Basic conversation structure
- **ATriggerEventActor** - Event trigger implementation
- **Event Subsystem** - Game event routing

## Prerequisites

- Completed Milestones 1-3
- Working character, interaction, and AI systems

## How to Use

1. Ensure Milestones 1-3 are complete
2. Copy these template files to your project
3. Follow the detailed [Milestone 4 Guide](/guide/milestone-4-detailed)
4. Implement dialog and event systems

## File Structure

```
Source/Platformer/
├── FriendlyNPC.h/.cpp
├── Talkable.h
├── TriggerEventActor.h/.cpp
├── GameplayEventSubsystem.h/.cpp
└── UI/
    └── DialogWidget.h/.cpp
```

## Key Features to Implement

- [ ] Friendly NPC with dialog
- [ ] Conversation system
- [ ] Dialog UI display
- [ ] Event triggers and areas
- [ ] Event routing via subsystem
- [ ] Interactive world storytelling

## Testing Your Implementation

1. Create a friendly NPC in the level
2. Walk up to NPC - "Talk" prompt should appear
3. Press interact - dialog should start
4. Step into trigger areas - events should fire
5. Check logs for event messages

## Next Steps

After completing this milestone:

- Proceed to [Milestone 5](/guide/milestone-5-detailed) for testing and polish
- Add branching dialog options
- Create quest and objective systems
