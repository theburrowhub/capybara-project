# CI/CD Complete Guide

Complete guide for CI/CD pipeline, version management, and releases for Capybara Shoot'em Up.

**Table of Contents**
- [Quick Start](#quick-start)
- [CI/CD Overview](#cicd-overview)
- [Creating Releases](#creating-releases)
- [Commitizen Version Management](#commitizen-version-management)
- [Workflows](#workflows)
- [Building Locally](#building-locally)
- [Troubleshooting](#troubleshooting)
- [Reference](#reference)

---

## Quick Start

### Create Your First Release

```bash
# 1. Install commitizen
make cz-install

# 2. Make some commits
make cz-commit  # Interactive commit with conventional format

# 3. Create an alpha release
make cz-alpha
# → Creates tag v0.2.0-alpha.0

# 4. Review and push
git log -1
cat CHANGELOG.md
git push origin main
git push --tags

# 5. Wait ~15 minutes
# → Check: https://github.com/YOUR_USERNAME/capybara-project/releases
```

That's it! GitHub Actions builds for Linux, macOS, and Windows automatically.

---

## CI/CD Overview

### Architecture

```
Your Machine                GitHub Actions
    │                            │
    ├─ make cz-alpha ─────→     │
    ├─ git push --tags ──────→  │
    │                            ├─ Build Linux (static)
    │                            ├─ Build macOS (universal)
    │                            ├─ Build Windows (static)
    │                            ├─ Build USER_MANUAL.pdf
    │                            └─ Create GitHub Release
    │                            
    └─ Download binaries ←───────┘
```

### What Gets Built

**For each release tag**:
- **Linux**: Static binary (~10-15 MB)
- **macOS**: Universal binary (~5-8 MB) - Intel + Apple Silicon
- **Windows**: Static binary (~10-15 MB)
- **Documentation**: USER_MANUAL.pdf (auto-generated from LaTeX)

**Each package includes**:
- Main game executable
- Launcher script (`run.sh` / `run.bat`)
- USER_MANUAL.pdf
- GAMEPLAY.md
- README.md
- Assets

### When Builds Run

✅ **Release tags** (`v*.*.*`):
- Automatic builds for all platforms
- Creates GitHub release
- Uploads binaries

✅ **Pull requests**:
- Validates builds work
- Creates test artifacts (30 days)

❌ **Regular pushes**:
- No builds (saves CI minutes)
- Test locally instead

---

## Creating Releases

### Version Types

| Type | Format | Purpose | Command |
|------|--------|---------|---------|
| **Alpha** | `v1.0.0-alpha.0` | Early testing | `make cz-alpha` |
| **Beta** | `v1.0.0-beta.0` | Feature-complete testing | `make cz-beta` |
| **RC** | `v1.0.0-rc.0` | Release candidate | `make cz-rc` |
| **Stable** | `v1.0.0` | Production ready | `make cz-release` |

### Alpha Release (Early Testing)

```bash
# Make commits
make cz-commit  # Type: feat, Message: "add multiplayer"
make cz-commit  # Type: fix, Message: "fix collision"

# Create alpha
make cz-alpha
# → v1.1.0-alpha.0

# Review
git log -1
cat CHANGELOG.md

# Push
git push origin main
git push --tags

# Wait ~15 minutes
# Check: https://github.com/YOUR_USERNAME/capybara-project/releases
# Download and test
```

### Beta Release (Testing)

```bash
# After testing alpha, promote to beta
make cz-beta
# → v1.1.0-beta.0

git push && git push --tags
```

### Stable Release (Production)

```bash
# After beta is stable, promote to stable
make cz-release
# → v1.1.0

git push && git push --tags

# 🎉 Production release!
```

### Complete Release Workflow

**Recommended workflow for new versions**:

```bash
# Step 1: Development
make cz-commit  # feat: add feature
make cz-commit  # fix: bug fixes
./build_static.sh  # Test locally

# Step 2: Alpha (v1.1.0-alpha.0)
make cz-alpha
git push && git push --tags
# Download, test, find issues

# Step 3: Fix issues
make cz-commit  # fix: alpha issues
make cz-alpha  # v1.1.0-alpha.1
git push && git push --tags

# Step 4: Beta (v1.1.0-beta.0)
make cz-beta
git push && git push --tags
# Beta testers test

# Step 5: Release candidate (optional)
make cz-rc  # v1.1.0-rc.0
git push && git push --tags

# Step 6: Stable (v1.1.0)
make cz-release
git push && git push --tags
# 🎉 Released!
```

---

## Commitizen Version Management

### Installation

```bash
# Install commitizen
make cz-install
# or
pip install --user commitizen

# Verify
make cz-check
```

### Making Commits

**Interactive (recommended)**:
```bash
make cz-commit
```

Follow prompts:
1. Select type (feat, fix, docs, etc.)
2. Enter scope (optional)
3. Write short description
4. Add longer description (optional)
5. Add footer (optional)

**Manual**:
```bash
git commit -m "feat(player): add dash ability"
git commit -m "fix(collision): correct hitbox detection"
```

### Commit Types

| Type | Description | Version Impact |
|------|-------------|----------------|
| `feat` | New feature | MINOR (0.x.0) |
| `fix` | Bug fix | PATCH (0.0.x) |
| `perf` | Performance | PATCH |
| `refactor` | Code refactor | PATCH |
| `docs` | Documentation | None |
| `style` | Code style | None |
| `test` | Tests | None |
| `build` | Build system | None |
| `ci` | CI/CD | None |
| `chore` | Maintenance | None |
| `BREAKING CHANGE` | Breaking change | MAJOR (x.0.0) |

### Version Bumping

**Automatic** (analyzes commits):
```bash
make cz-bump
# Determines version from commit history
```

**Manual** (explicit control):
```bash
make cz-bump-major  # v1.0.0 → v2.0.0 (breaking)
make cz-bump-minor  # v1.0.0 → v1.1.0 (features)
make cz-bump-patch  # v1.0.0 → v1.0.1 (fixes)
```

### Pre-Release Commands

```bash
make cz-alpha    # Create/increment alpha
make cz-beta     # Create/increment beta
make cz-rc       # Create/increment RC
make cz-release  # Promote to stable
```

### Commitizen Makefile Targets

```bash
# Setup
make cz-install      # Install commitizen
make cz-check        # Verify installation
make cz-version      # Show current version
make cz-help         # Show all commands

# Commits
make cz-commit       # Interactive commit

# Version bumps (automatic)
make cz-bump         # Auto-detect from commits

# Version bumps (manual)
make cz-bump-major   # Breaking changes
make cz-bump-minor   # New features
make cz-bump-patch   # Bug fixes

# Pre-releases
make cz-alpha        # Alpha release
make cz-beta         # Beta release
make cz-rc           # Release candidate
make cz-release      # Stable release

# Other
make cz-changelog    # Update CHANGELOG.md only
```

---

## Workflows

### Build Workflow (`.github/workflows/build.yml`)

**Triggers**:
- Pull requests
- Manual dispatch

**Purpose**: Testing and validation

**Output**: Build artifacts (30-day retention)

**Does NOT run on**: Regular pushes (saves CI minutes)

### Release Workflow (`.github/workflows/release.yml`)

**Triggers**:
- Version tags (`v*.*.*`)
- Manual dispatch with version input

**Purpose**: Production releases

**Output**: GitHub release with binaries

**What it does**:
1. Detects if pre-release (alpha/beta/rc)
2. Builds for Linux, macOS, Windows
3. Installs LaTeX and builds USER_MANUAL.pdf
4. Strips debug symbols
5. Creates release packages with:
   - Game executable
   - Launcher script
   - USER_MANUAL.pdf
   - Documentation
   - Assets
6. Uploads to GitHub release

**Build times**: ~15 minutes (all platforms in parallel)

### Pre-Release Detection

The workflow automatically detects release type:

```yaml
v1.0.0-alpha.0  → Pre-release: Alpha
v1.0.0-beta.1   → Pre-release: Beta
v1.0.0-rc.0     → Pre-release: Release Candidate
v1.0.0          → Stable Release
```

---

## Building Locally

### Quick Build

**Using CMake (CI/CD compatible)**:
```bash
./build_static.sh
./build/shootemup
```

**Using Make (faster for development)**:
```bash
make clean
make -j$(nproc)
./bin/shootemup
```

### Build Systems

**Make** (traditional):
- Fast for local development
- Original build system
- Use for daily work

**CMake** (cross-platform):
- Used by CI/CD
- Better cross-platform support
- Test with `./build_static.sh`

### Build Scripts

**Linux/macOS**: `./build_static.sh`
- Detects platform
- Configures CMake
- Builds statically (Linux) or dynamically (macOS)

**Windows**: `build_static.bat`
- Uses MinGW
- Static linking
- Portable executable

### Verification

```bash
# Verify setup
./verify_cicd_setup.sh
# Should show: ✓ All checks passed

# Check build
./build_static.sh
file build/shootemup

# Check dependencies
ldd build/shootemup         # Linux
otool -L build/shootemup    # macOS
```

---

## Troubleshooting

### Commitizen Issues

**Not installed**:
```bash
make cz-install
# or
pip install --user commitizen
```

**Version mismatch**:
```bash
# Check version
make cz-version

# Reset if needed (careful!)
# Edit .cz.json manually
```

**Wrong version bump**:
```bash
# Undo last bump
git tag -d v1.2.0        # Delete local tag
git reset --hard HEAD~1  # Undo commit

# Bump correctly
make cz-bump-patch
```

### Build Issues

**Local build fails**:
```bash
# Check dependencies
pkg-config --libs raylib

# Clean and retry
rm -rf build/
./build_static.sh
```

**CI build fails**:
1. Check workflow logs: Actions → Failed workflow → Job
2. Look for error (red text)
3. Fix locally
4. Push again

### Release Issues

**Tag doesn't trigger release**:
- Must start with `v`: `v1.0.0` ✅, `1.0.0` ❌
- Verify push: `git push origin v1.0.0`
- Check Actions tab

**Wrong release type**:
- Alpha/beta detection looks for keywords in tag
- Use proper format: `v1.0.0-alpha.0`

**Missing binaries**:
- Check workflow logs for each platform
- Verify builds succeeded
- Check artifact upload step

---

## Reference

### File Structure

```
Project Root
├── .cz.json                           # Commitizen config (JSON)
├── CMakeLists.txt                     # Cross-platform build
├── Makefile                           # Traditional build + cz targets
├── build_static.sh                    # Local build script (Unix)
├── build_static.bat                   # Local build script (Windows)
├── verify_cicd_setup.sh               # Setup verification
│
├── .github/
│   └── workflows/
│       ├── build.yml                  # CI (PR testing)
│       └── release.yml                # CD (releases)
│
└── docs/
    └── CI_CD_COMPLETE_GUIDE.md        # This file
```

### Makefile Targets

**Build targets**:
```bash
make              # Build all
make game         # Build main game only
make clean        # Clean artifacts
make run          # Build and run
```

**Commitizen targets**:
```bash
make cz-help      # Show all cz commands
make cz-commit    # Interactive commit
make cz-alpha     # Alpha release
make cz-beta      # Beta release
make cz-release   # Stable release
```

### Version Format

```
v MAJOR . MINOR . PATCH - PRERELEASE . NUMBER
  │       │       │       │            │
  │       │       │       │            └─ Pre-release increment
  │       │       │       └─────────────── Pre-release type
  │       │       └─────────────────────── Bug fixes
  │       └─────────────────────────────── New features
  └─────────────────────────────────────── Breaking changes

Examples:
v1.0.0           # Stable
v1.0.1           # Patch
v1.1.0           # Minor
v2.0.0           # Major
v1.1.0-alpha.0   # Alpha
v1.1.0-beta.1    # Beta
v1.1.0-rc.0      # Release candidate
```

### CI Minutes Usage

**GitHub Free Tier**: 2,000 minutes/month

**Per build cycle**:
- Linux: 8 min × 1 = 8 minutes
- Windows: 12 min × 2 = 24 minutes
- macOS: 10 min × 10 = 100 minutes
- **Total**: 132 effective minutes

**Monthly capacity**: ~15 releases

**Optimization**:
- Builds only on releases and PRs
- No builds on regular pushes
- Saves ~80% CI minutes

### Platform Specifics

**Linux**:
- Ubuntu latest
- Static linking
- Portable binary
- ~10-15 MB

**macOS**:
- Universal binary (Intel + Apple Silicon)
- Dynamic linking with system frameworks
- Minimum: macOS 10.15
- ~5-8 MB

**Windows**:
- MinGW64
- Static linking
- Portable executable
- ~10-15 MB

### Release Contents

Each platform package includes:

```
capybara-shootemup-<platform>/
├── shootemup (.exe on Windows)
├── run.sh / run.bat
├── USER_MANUAL.pdf
├── GAMEPLAY.md
├── README.md
└── assets/
    ├── sprites/
    └── audio/ (if included)
```

---

## Best Practices

### 1. Test Locally First
```bash
./build_static.sh
./build/shootemup
# Verify it works before creating release
```

### 2. Use Pre-Releases
```bash
# Don't jump to stable
make cz-alpha    # Test
make cz-beta     # Test more
make cz-release  # Release
```

### 3. Write Good Commit Messages
```bash
# Good
make cz-commit
# Type: feat, Scope: player, Message: "add dash ability"

# Bad
git commit -m "stuff"
```

### 4. Review Before Pushing
```bash
git log -1           # Check commit
cat CHANGELOG.md     # Check changelog
git push && git push --tags
```

### 5. Keep Changelog Updated
Commitizen generates CHANGELOG.md automatically from commits.
Review it before pushing tags.

---

## Summary

### What You Have

✅ **Multi-platform CI/CD** - Linux, macOS, Windows  
✅ **Version management** - Commitizen with alpha/beta support  
✅ **Makefile integration** - Simple `make` commands  
✅ **Automatic releases** - Tag push triggers build  
✅ **Static binaries** - Portable (where possible)  
✅ **PDF manual** - Auto-generated from LaTeX  
✅ **CI minute optimization** - Only build on releases  

### Your Workflow

```bash
# Daily development
make              # Build
./bin/shootemup   # Test
make cz-commit    # Commit

# Create release
make cz-alpha     # Alpha
git push && git push --tags

# Promote
make cz-beta      # Beta
make cz-release   # Stable
git push && git push --tags
```

### Everything Is Manual

- ✅ You run commitizen locally
- ✅ You create tags
- ✅ You push when ready
- ❌ CI/CD never runs commitizen
- ❌ No automatic version bumps

### Resources

- **Quick help**: `make cz-help`
- **Verify setup**: `./verify_cicd_setup.sh`
- **This guide**: `docs/CI_CD_COMPLETE_GUIDE.md`

---

**You have complete control over versions and releases. The pipeline just builds what you tell it to.** 🚀

