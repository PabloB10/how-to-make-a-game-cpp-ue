# Milestone 2 - Starter Template

This template provides the interaction system foundation for Milestone 2.

## What's Included

- **IInteractable Interface** - Base interface for interactive objects
- **InteractionComponent** - Player component for detecting interactions
- **InteractiveActor** - Base class for interactive objects
- **PickupActor** - Example pickup implementation
- **UI Widget Base** - Starter widget for interaction prompts

## Download Template

📁 **[Download Template Files](https://github.com/your-repo/templates/milestone-2-starter)**

Or browse the files in the repository's `templates/milestone-2-starter/` directory.

## Prerequisites

- ✅ Completed Milestone 1
- ✅ Working character movement and camera system

## Implementation Guide

Follow the detailed [Milestone 2 Guide](/guide/milestone-2-detailed) to implement:

1. **IInteractable Interface** - Define interaction contract
2. **InteractionComponent** - Sphere tracing for detection
3. **Interactive Objects** - Pickups, doors, and more
4. **UI System** - Interaction prompts and feedback
5. **Range Detection** - Distance-based interaction

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

- Proceed to [Milestone 3](/templates/milestone-3-starter/) for stats and AI
- Add more interactive object types
- Enhance the UI with animations and styling
