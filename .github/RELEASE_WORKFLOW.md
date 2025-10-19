# GitHub Release Workflow

## Overview
This workflow automatically creates GitHub releases based on git tags created by commitizen.

## Pre-release Detection

The workflow automatically detects if a release is alpha, beta, or a release candidate based on the tag name:

### Tag Format Examples

| Tag Version | Release Type | Pre-release Flag |
|-------------|--------------|------------------|
| `v1.0.0` | Stable Release | ❌ No |
| `v1.0.0a0` | Alpha | ✅ Yes |
| `v1.0.0a1` | Alpha | ✅ Yes |
| `v1.0.0a2` | Alpha | ✅ Yes |
| `v1.0.0b0` | Beta | ✅ Yes |
| `v1.0.0b1` | Beta | ✅ Yes |
| `v1.0.0b2` | Beta | ✅ Yes |
| `v1.0.0rc0` | Release Candidate | ✅ Yes |
| `v1.0.0rc1` | Release Candidate | ✅ Yes |
| `v2.0.0` | Stable Release | ❌ No |

The workflow also supports alternative formats like `v1.0.0-alpha.1`, `v1.0.0-beta.1`, etc.

### Features

- **Case-insensitive matching**: Handles `alpha`, `Alpha`, `ALPHA`, etc.
- **Automatic detection**: No manual configuration needed
- **Pre-release marking**: Pre-releases are marked with the pre-release flag in GitHub
- **Release type labels**: Each release shows its type (Alpha, Beta, RC, or Stable)

## Creating Releases with Commitizen

### Stable Release
```bash
cz bump
git push --follow-tags
```
This creates a tag like `v1.0.0` → Stable Release

### Alpha Release
```bash
cz bump --prerelease alpha
git push --follow-tags
```
This creates a tag like `v1.0.0a0` → Alpha Pre-release

### Beta Release
```bash
cz bump --prerelease beta
git push --follow-tags
```
This creates a tag like `v1.0.0b0` → Beta Pre-release

### Release Candidate
```bash
cz bump --prerelease rc
git push --follow-tags
```
This creates a tag like `v1.0.0rc0` → RC Pre-release

## Manual Workflow Trigger

You can also manually trigger the release workflow:

1. Go to **Actions** → **Release Multi-Platform**
2. Click **Run workflow**
3. Enter the version tag (e.g., `v1.0.0b1`)
4. The workflow will automatically detect if it's a pre-release

## How It Works

The workflow:

1. **Extracts version** from the git tag
2. **Analyzes the version** string (case-insensitive)
3. **Detects pre-release keywords**: `alpha`, `beta`, or `rc`
4. **Sets appropriate flags**:
   - `is_prerelease`: `true` or `false`
   - `release_type`: `Alpha`, `Beta`, `Release Candidate`, or `Stable Release`
5. **Creates GitHub release** with the correct pre-release flag
6. **Builds artifacts** for Linux, macOS, and Windows
7. **Uploads artifacts** to the release

## Debug Output

The workflow provides clear debug output:

```
🔍 Analyzing version: v1.0.0b1
✅ Version: v1.0.0b1
✅ Is Pre-release: true
✅ Release Type: Beta
```

This helps verify the release is being created correctly.

