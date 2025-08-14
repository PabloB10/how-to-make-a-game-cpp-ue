# General Instructions: VitePress Docs for UE 5.6+ Platformer (for GitHub Copilot)

Audience: Professional engineers (familiar with design patterns) new to C++ in Unreal Engine. These are instructions for GitHub Copilot to build a modern VitePress site that documents how to create a UE 5.6+ C++ platformer in a separate repository.

Repo purpose (goals)

- Build a GitHub Pages site powered by VitePress that teaches, step by step, how to implement a UE 5.6+ C++ platformer.
- Present clean, modern patterns (component-driven, data assets, interfaces) with concise guidance and tunable parameters.
- Keep content incremental and testable; provide clear milestones and acceptance criteria.

Out of scope

- Implementing the game in this repository. The game code will live in a separate repo and consume this documentation.
- Shipping Unreal content or assets here.

Prerequisites

- Windows 10/11, Git, Node.js LTS (>=18), npm (or pnpm). GitHub Pages enabled for the repo.
- IDE: Visual Studio Code for authoring docs and coding; Unreal Engine 5.6+ installed for validating steps in the companion game repo.

Site structure (proposed)

- docs/.vitepress/config.ts — site config (title, description, theme, nav, sidebar)
- docs/index.md — landing page (overview, quick links)
- docs/guide/
  - getting-started.md — how to use this site and the companion game repo
  - ue-setup.md — UE 5.6+ install, plugins, project creation
  - milestones.md — step-by-step plan and acceptance criteria
- docs/reference/
  - architecture.md — systems and patterns
  - cpp-primer-for-unreal.md — C+++UE cheat sheet
  - gameplay-objects.md — parameters and data assets
- docs/appendix/
  - faq.md, glossary.md (optional)

Note: Existing source docs in `Docs/` are the content backbone and should be migrated into `docs/reference/` and `docs/guide/` during site setup.

How Copilot should work in this repo

- Initialize a minimal VitePress site with typed config (`config.ts`) and a clean, accessible theme.
- Generate navigation and sidebar reflecting the structure above; ensure stable slugs and relative links.
- Migrate current markdown from `Docs/` into `docs/` with light editing for headings, frontmatter, and cross-links.
- Add npm scripts: `docs:dev`, `docs:build`, `docs:preview`.
- Add GitHub Actions workflow to build and deploy VitePress to GitHub Pages on push to `main`.
- Add Prettier and markdownlint for formatting and linting; fail CI on broken links (optional: link checker step).

Content guidelines

- Keep content concise and engineer-focused; prefer checklists and parameter tables.
- Code samples minimal and self-contained; provide Windows-specific notes when relevant.
- Emphasize UE modern practices: components, interfaces, data assets, Enhanced Input; avoid Tick by default.

Asset strategy (for the companion game repo)

- Use Git LFS for large binary assets (uasset/umap, audio, textures). Configure `.gitattributes` accordingly.
- Keep mock/placeholders lightweight (simple materials, cubes, basic sounds) to ease replacement; document expected paths/names.
- Avoid committing derived/build artifacts (e.g., Binaries/, DerivedDataCache/, Intermediate/). Add to `.gitignore`.
- Prefer storing tuning data as Data Assets or CSV/JSON (small text) where possible; binaries only when required by UE.

Build, run, deploy (reference)

- Local: `npm run docs:dev` to preview; `npm run docs:build` to emit static site.
- Deploy: GitHub Actions builds on push to `main` and publishes to Pages. Use VitePress official Pages workflow.

Quality gates

- Site builds without warnings; links resolve; markdownlint/Prettier pass on PR.
- Clear sidebar/nav; no orphaned pages; consistent headings and frontmatter.

Contribution workflow

- Small PRs per page or milestone. Conventional commits (feat:, fix:, docs:, refactor:, chore:).
- PR checklist: links checked, lints pass, local preview verified.

Content index (source in this repo)

- `Docs/architecture.md` — reference/architecture
- `Docs/cpp-primer-for-unreal.md` — reference/cpp-primer-for-unreal
- `Docs/gameplay-objects.md` — reference/gameplay-objects
- `Docs/milestones.md` — guide/milestones

Notes

- This repository hosts the documentation site only. The companion game implementation repo should follow the patterns and parameters defined here.

---

This document instructs GitHub Copilot on how to set up and maintain the VitePress site and how to organize the content that guides building the UE platformer in another repository.
