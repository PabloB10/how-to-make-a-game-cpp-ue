# Prerequisites

## System Requirements

**Minimum:**

- Windows 10 64-bit (version 1909 or later) or Windows 11
- 32 GB RAM (16 GB absolute minimum, but you'll struggle)
- 100 GB free disk space (UE5 + Visual Studio + project)
- DirectX 11/12 compatible graphics card
- Quad-core CPU (Intel i5-8400 / AMD Ryzen 5 2600 or equivalent)

**Recommended:**

- 64 GB RAM for comfortable development
- NVMe SSD for faster compilation and loading
- RTX 3060 / RX 6600 XT or better for editor performance
- 8-core CPU (Intel i7-10700K / AMD Ryzen 7 3700X or better)

## Required Software

**Essential:**

- [Unreal Engine 5.6+](https://www.unrealengine.com/) (Epic Games Launcher)
- [Visual Studio 2022](https://visualstudio.microsoft.com/) (v17.9+) with C++ workload
- [Git](https://git-scm.com/) with [Git LFS](https://git-lfs.github.io/)
- [Node.js 18+](https://nodejs.org/) (for this documentation site)

**Highly Recommended:**

- [Visual Studio Code](https://code.visualstudio.com/) for documentation and quick C++ edits
- [GitHub Desktop](https://desktop.github.com/) or command-line Git experience

## C++ Skill Level Expectations

**Required Knowledge:**

- Basic C++ syntax (classes, inheritance, pointers)
- Object-oriented programming concepts
- Basic understanding of memory management
- Comfortable with compilers and IDEs

**Helpful but Not Required:**

- Game development experience
- Previous Unreal Engine experience (UE4 knowledge transfers well)
- Advanced C++ features (templates, STL)

**Learning Resources if Needed:**

- [Learn C++](https://www.learncpp.com/) - Comprehensive C++ tutorial
- [Unreal Engine C++ Documentation](https://docs.unrealengine.com/5.3/en-US/unreal-engine-cpp-quick-start/)

## Time Commitment Estimates

**Per Milestone (assuming 2-3 hours per session):**

- **Milestone 1** (Character + Input): 6-8 hours over 2-3 sessions
- **Milestone 2** (Interaction System): 4-6 hours over 2 sessions
- **Milestone 3** (AI + Combat): 8-10 hours over 3-4 sessions
- **Milestone 4** (NPCs + Events): 4-6 hours over 2 sessions
- **Milestone 5** (Testing + Polish): 6-8 hours over 2-3 sessions

**Total Project Time:** 28-38 hours over 4-6 weeks (assuming 1-2 sessions per week)

## UE5 vs UE4 Migration Notes

**If Coming from UE4:**

- Enhanced Input replaces old Input system (completely different API)
- Behavior Trees are largely the same
- C++ compilation is faster in UE5
- Some Blueprint nodes have been renamed or moved
- Nanite and Lumen are new rendering features (not covered in this guide)

**New to Unreal Engine:**

- Don't worry about UE4 - start fresh with UE5 patterns
- Focus on the component-based architecture from day one
- Embrace data-driven design via Data Assets

## Development Environment Setup Checklist

Before starting Milestone 1, verify:

- [ ] UE5.6+ installed and can create new C++ project
- [ ] Visual Studio 2022 with C++ workload can compile simple UE project
- [ ] Git and Git LFS installed (`git lfs version` works)
- [ ] GitHub account created (for companion repo)
- [ ] VS Code installed with C++ extension (optional but recommended)
- [ ] At least 20 GB free space for initial project

## Common Setup Issues

**"Cannot find UnrealBuildTool"**

- Ensure Visual Studio C++ workload is fully installed
- Restart Epic Games Launcher after VS installation

**Git LFS Errors**

- Run `git lfs install` in command prompt as administrator
- Verify with `git lfs env`

**Slow Compilation**

- Exclude UE project folders from Windows Defender real-time scanning
- Use SSD for UE installation and projects
- Close unnecessary applications during compilation

## Next Steps

Once your environment is ready:

1. Read [Architecture](/reference/architecture) for the big picture
2. Follow [UE Setup](/guide/ue-setup) for plugin configuration
3. Create your [Companion Repo](/guide/companion-repo-setup)
4. Begin [Milestone 1](/guide/milestones#milestone-1-detailed)
