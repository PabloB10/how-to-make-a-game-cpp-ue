# Companion Repo Setup (UE C++ Project)

This page outlines how to initialize the separate game repository that implements the platformer described here.

Repo

- Create a new GitHub repository (e.g., `ue-platformer-game`).
- Clone locally and open in VS Code.

UE Project

- In Unreal Engine 5.6+, create a new C++ project (Third Person or Blank) named `Platformer`.
- Ensure plugins: Enhanced Input, AIModule/BehaviorTree, NavigationSystem, UMG (GameplayTags optional).

Git LFS

- Install Git LFS and track large assets:
  - `*.uasset`, `*.umap`, `*.wav`, `*.ogg`, `*.png`, `*.tga` (add more as needed)
- Add a `.gitattributes` like:

  ```txt
  *.uasset filter=lfs diff=lfs merge=lfs -text
  *.umap   filter=lfs diff=lfs merge=lfs -text
  *.wav    filter=lfs diff=lfs merge=lfs -text
  *.ogg    filter=lfs diff=lfs merge=lfs -text
  *.png    filter=lfs diff=lfs merge=lfs -text
  *.tga    filter=lfs diff=lfs merge=lfs -text
  ```

Git ignore

- Add `.gitignore` entries for derived/build artifacts:

  ```txt
  Binaries/
  DerivedDataCache/
  Intermediate/
  Saved/Logs/
  *.sln
  .vs/
  ```

Structure (proposed)

- Source/Platformer/ (C++ module)
- Config/
- Content/ (placeholders/mocks are OK)
- Tests/ (optional automation specs)

Next

- Follow this site’s Milestones and Reference sections to implement systems incrementally.
