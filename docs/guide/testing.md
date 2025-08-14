# Testing (Quick Start)

Approach

- Prefer C++ Automation Spec for core logic; use functional tests for PIE behaviors.
- Keep tests fast and deterministic; avoid latent delays where possible.

C++ Automation Spec (outline)

- Create a `Tests` module or folder in `Source/Platformer/` guarded for editor builds.
- Example specs:
  - Movement speed within expected bounds after sprint toggle.
  - Interaction prompt visibility when entering/exiting radius.
  - Enemy AI transitions: patrol → chase within perception radius.

Blueprint Functional Tests

- Create functional test maps for interaction and AI.
- Use automation to run them in PIE with minimal assets.

CI hint

- Run a headless editor session for tests when the companion repo adds CI.
