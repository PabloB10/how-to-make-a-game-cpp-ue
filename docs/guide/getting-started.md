# Getting Started

This site describes how to build a UE 5.6+ platformer in C++ in a separate companion repository.

Two-repo model

- Docs repo (this): VitePress site with patterns, parameters, and milestones.
- Game repo (companion): UE C++ project implementing the platformer.

Tools

- Visual Studio Code (primary IDE for docs and C++ authoring)
- Unreal Engine 5.6+ (Editor + toolchain)
- Visual Studio 2022 (v17.9+), Git, Node.js 18+

Start here

1. Check [Prerequisites](/guide/prerequisites) for system requirements and environment setup.
2. Read the Architecture and Gameplay Objects reference pages.
3. Create the companion game repo and UE C++ project (see Guide → Companion Repo Setup).
4. Follow [Milestone 1 Detailed](/guide/milestone-1-detailed) step-by-step guide.
5. Continue with remaining milestones and iterate.

Assets overview (for the game repo)

- Use Git LFS for large binaries (`.uasset`, `.umap`, textures, audio). Keep mocks lightweight and replaceable.
- Add `Binaries/`, `DerivedDataCache/`, `Intermediate/` to `.gitignore` (don’t commit derived data).

Quick links

- **Prerequisites**: /guide/prerequisites
- **Architecture**: /reference/architecture
- **Gameplay Objects**: /reference/gameplay-objects
- **Milestones**: /guide/milestones
- **Milestone 1 Detailed**: /guide/milestone-1-detailed
- **UE Setup**: /guide/ue-setup
- **Companion Repo Setup**: /guide/companion-repo-setup
- **C++ Primer**: /reference/cpp-primer-for-unreal
- **Troubleshooting**: /guide/troubleshooting
