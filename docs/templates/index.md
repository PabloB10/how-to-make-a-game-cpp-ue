# Project Templates

This directory contains starter templates to help you implement the C++ Unreal Engine platformer game following the milestone-based approach.

## Available Templates

### 🚀 [Milestone 1 Starter](./milestone-1-starter/)

**Character, Camera & Input Foundation**

- Basic character class structure
- Enhanced Input system setup
- Camera component configuration
- Data asset tuning system

**Use this when:** Starting the course from scratch

---

### 🎯 [Milestone 2 Starter](./milestone-2-starter/)

**Interaction System Foundation**

- IInteractable interface
- InteractionComponent for detection
- Base interactive actor classes
- UI prompt system structure

**Use this when:** Completed Milestone 1 and ready for interactions

---

### 🤖 [Milestone 3 Starter](./milestone-3-starter/)

**Stats, Damage & AI Foundation**

- StatsComponent structure
- IDamageable interface
- Enemy NPC base classes
- AI controller framework

**Use this when:** Completed Milestones 1-2 and ready for AI

---

### 💬 [Milestone 4 Starter](./milestone-4-starter/)

**NPCs & Events Foundation**

- Friendly NPC classes
- ITalkable interface
- Dialog system structure
- Event trigger framework

**Use this when:** Completed Milestones 1-3 and ready for NPCs

---

### ✨ [Milestone 5 Starter](./milestone-5-starter/)

**Testing & Polish Foundation**

- Test framework setup
- Console variable system
- Debug helper utilities
- Performance monitoring tools

**Use this when:** Completed Milestones 1-4 and ready for testing

---

### 🎮 [Complete Project](./complete-project/)

**Full Reference Implementation**

- Complete implementation of all milestones
- Production-ready code quality
- Comprehensive test suite
- Full documentation

**Use this when:** Need a reference implementation or starting point for your own game

---

## How to Choose

### 📚 **Learning Path (Recommended)**

If you're learning C++ and Unreal Engine:

1. Start with **Milestone 1 Starter**
2. Follow the detailed guides step by step
3. Use other starters as you progress through milestones
4. Reference **Complete Project** when stuck

### 🎯 **Quick Start Path**

If you have experience and want to move quickly:

1. Use **Complete Project** as a reference
2. Copy relevant files for your milestone
3. Focus on understanding rather than typing

### 🔧 **Custom Game Path**

If you're building your own game:

1. Start with **Complete Project**
2. Rename and modify classes for your game
3. Add your own gameplay systems
4. Use the architecture as a foundation

---

## Template Structure

Each template includes:

```
template-name/
├── README.md           # Template overview and instructions
├── *.h / *.cpp         # C++ source files
├── Assets/             # Blueprint and data assets (if any)
├── Config/             # Configuration files (if any)
└── Documentation/      # Additional docs (if any)
```

---

## Using Templates

### Step 1: Create Your UE Project

1. Open Epic Games Launcher
2. Create new C++ project (Third Person template)
3. Name it "Platformer" (or your preferred name)

### Step 2: Copy Template Files

1. Navigate to the appropriate template folder
2. Copy the source files to your project's `Source/Platformer/` directory
3. Maintain the folder structure shown in the template

### Step 3: Generate Project Files

1. Right-click your `.uproject` file
2. Select "Generate Visual Studio project files"
3. Open the solution in Visual Studio

### Step 4: Compile and Test

1. Build the project (Build → Build Solution)
2. Launch the editor
3. Test the implemented features

---

## Support

If you encounter issues with templates:

1. **Check Prerequisites:** Ensure your UE version and VS setup match requirements
2. **Review Guides:** Each template links to detailed implementation guides
3. **Compare with Complete:** Use the complete project as a reference
4. **Check Common Issues:** Review troubleshooting documentation

---

## Contributing

These templates are designed to be educational resources. If you find improvements or have suggestions:

1. Check the documentation for the intended learning approach
2. Consider if changes align with educational goals
3. Test changes thoroughly across different UE versions

---

## License

Templates are provided for educational purposes following the course structure and learning objectives.
