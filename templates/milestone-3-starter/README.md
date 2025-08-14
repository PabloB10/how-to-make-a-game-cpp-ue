# Milestone 3 - Starter Template

This template provides the stats, damage, and AI foundation for Milestone 3.

## What's Included

- **UStatsComponent** - Health and stamina management
- **IDamageable Interface** - For damage handling
- **AEnemyNPC** - Base enemy class with AI
- **Damage System** - Basic damage types and handling
- **AI Framework** - Controller and behavior tree structure

## Prerequisites

- Completed Milestones 1-2
- Working character and interaction system

## How to Use

1. Ensure Milestones 1-2 are complete
2. Copy these template files to your project
3. Follow the detailed [Milestone 3 Guide](/guide/milestone-3-detailed)
4. Implement AI behaviors and combat system

## File Structure

```
Source/Platformer/
├── StatsComponent.h/.cpp
├── Damageable.h
├── EnemyNPC.h/.cpp
├── EnemyAIController.h/.cpp
└── AI/
    └── (Behavior Tree assets)
```

## Key Features to Implement

- [ ] Health and stamina tracking
- [ ] Damage application and resistance
- [ ] Enemy AI with perception
- [ ] Patrol and chase behaviors
- [ ] Combat mechanics
- [ ] Death and respawn system

## Testing Your Implementation

1. Spawn an enemy in the level
2. Enemy should patrol around spawn point
3. Get close - enemy should detect and chase you
4. Take damage from enemy attacks
5. Health should decrease and regenerate

## Next Steps

After completing this milestone:

- Proceed to [Milestone 4](/guide/milestone-4-detailed) for NPCs and events
- Add more enemy types and behaviors
- Enhance combat with special attacks
