# Milestone 2 - Starter Template

This template provides the interaction system foundation for Milestone 2.

## What's Included

- **IInteractable Interface** - Base interface for interactive objects
- **InteractionComponent** - Player component for detecting interactions
- **InteractiveActor** - Base class for interactive objects
- **PickupActor** - Example pickup implementation
- **UI Widget Base** - Starter widget for interaction prompts

## Prerequisites

- Completed Milestone 1
- Working character movement and camera system

## How to Use

1. Ensure Milestone 1 is complete and working
2. Copy these template files to your project
3. Follow the detailed [Milestone 2 Guide](/guide/milestone-2-detailed)
4. Implement the interaction system step by step

## File Structure

```
Source/Platformer/
├── Interactable.h (Interface)
├── InteractionComponent.h/.cpp
├── InteractiveActor.h/.cpp
├── PickupActor.h/.cpp
└── UI/
    └── InteractionPromptWidget.h/.cpp
```

## Key Features to Implement

- [ ] IInteractable interface with GetInteractionVerb()
- [ ] UInteractionComponent with sphere tracing
- [ ] AInteractiveActor base class
- [ ] Pickup and Door example actors
- [ ] UI prompt showing interaction text
- [ ] Range-based interaction detection

## Testing Your Implementation

1. Create a pickup in the level
2. Walk near it - prompt should appear
3. Press interact key - pickup should be consumed
4. Walk away - prompt should disappear

## Next Steps

After completing this milestone:

- Proceed to [Milestone 3](/guide/milestone-3-detailed) for stats and AI
- Add more interactive object types
- Enhance the UI with animations and styling
