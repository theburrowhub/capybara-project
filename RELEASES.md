# Creating Releases

Quick reference for creating releases with the CI/CD pipeline.

## Quick Start

```bash
# 1. Install commitizen (first time only)
make cz-install

# 2. Make commits
make cz-commit    # Interactive commit

# 3. Create release
make cz-alpha     # v1.0.0-alpha.0

# 4. Push
git push origin main
git push --tags

# Wait ~15 minutes
# → Linux, macOS, Windows binaries ready!
```

## Release Types

| Type | Command | Version | Purpose |
|------|---------|---------|---------|
| **Alpha** | `make cz-alpha` | v1.0.0-alpha.0 | Early testing |
| **Beta** | `make cz-beta` | v1.0.0-beta.0 | Feature-complete testing |
| **RC** | `make cz-rc` | v1.0.0-rc.0 | Release candidate |
| **Stable** | `make cz-release` | v1.0.0 | Production ready |

## Full Workflow Example

```bash
# Development
make cz-commit  # feat: add multiplayer
make cz-commit  # fix: network sync

# Alpha testing
make cz-alpha              # v1.1.0-alpha.0
git push && git push --tags
# Test... find bugs

make cz-commit             # fix: bugs
make cz-alpha              # v1.1.0-alpha.1
git push && git push --tags

# Beta testing
make cz-beta               # v1.1.0-beta.0
git push && git push --tags
# Test more...

# Stable release
make cz-release            # v1.1.0
git push && git push --tags
# 🎉 Released!
```

## Commit Format

Use conventional commits for automatic changelog generation:

```bash
# Feature (bumps minor: 0.x.0)
git commit -m "feat(player): add dash ability"

# Bug fix (bumps patch: 0.0.x)
git commit -m "fix(collision): correct hitbox"

# Breaking change (bumps major: x.0.0)
git commit -m "feat(api)!: redesign weapon system

BREAKING CHANGE: old weapon configs incompatible"
```

Or use interactive mode:
```bash
make cz-commit
# Follow prompts
```

## What Happens

When you push a tag:

1. **GitHub Actions triggers**
2. **Builds 3 platforms** (Linux, macOS, Windows)
3. **Generates USER_MANUAL.pdf** from LaTeX
4. **Creates GitHub release**
5. **Uploads binaries** (~15 minutes total)

Each release includes:
- Game executable
- Launcher script
- USER_MANUAL.pdf
- Documentation
- Assets

## Check Your Release

After pushing tags:
- Go to: `https://github.com/YOUR_USERNAME/capybara-project/releases`
- Download and test binaries
- Share with users!

## Commitizen Commands

```bash
make cz-install      # Install commitizen
make cz-check        # Verify installation
make cz-help         # Show all commands
make cz-commit       # Interactive commit
make cz-alpha        # Alpha release
make cz-beta         # Beta release
make cz-rc           # Release candidate
make cz-release      # Stable release
make cz-version      # Show current version
```

## Pre-Release Detection

The CI/CD automatically detects release type:
- `v1.0.0-alpha.0` → Marked as "Pre-release: Alpha"
- `v1.0.0-beta.1` → Marked as "Pre-release: Beta"
- `v1.0.0-rc.0` → Marked as "Pre-release: RC"
- `v1.0.0` → Marked as "Latest Release"

## Tips

✅ **Test locally first**: `./build_static.sh`  
✅ **Use pre-releases**: alpha → beta → stable  
✅ **Review before pushing**: Check `git log -1` and `CHANGELOG.md`  
✅ **Good commit messages**: Use `make cz-commit`  
✅ **Push tags separately**: `git push && git push --tags`  

## Troubleshooting

**Commitizen not found**: `make cz-install`  
**Build fails**: Check Actions tab logs  
**Tag doesn't trigger**: Must start with `v` (v1.0.0)  

## Complete Documentation

For detailed information, see:
- **[Complete CI/CD Guide](docs/CI_CD_COMPLETE_GUIDE.md)** - Everything about CI/CD
- **[Build Instructions](docs/BUILD.md)** - Local building
- **Makefile**: Run `make cz-help` for all commands

---

**Ready to release?** Start with `make cz-alpha` and push! 🚀

