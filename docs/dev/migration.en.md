# FastQTools Script System Migration Guide

This document helps you migrate from the old script system to the new unified script interface.

## Overview

FastQTools has undergone a comprehensive script system refactoring, providing a clearer and more consistent interface. Main improvements include:

✅ **Unified command interface** - All scripts use consistent parameter format  
✅ **Common function library** - Eliminates duplicate code, improves maintainability  
✅ **Better error handling** - Detailed error messages and recovery suggestions  
✅ **Enhanced documentation** - Each script has complete help information  
✅ **Improved testing architecture** - Added integration test layer, better test organization  

---

## Script Mapping Table

### Build Scripts

| Old Command | New Command | Description |
|-------------|-------------|-------------|
| `./scripts/build.sh` | `./scripts/core/build` | Default build |
| `./scripts/build.sh clang Release` | `./scripts/core/build` | Same (default is clang Release) |
| `./scripts/build.sh gcc Debug` | `./scripts/core/build -c gcc -t Debug` | Specify compiler and type |
| `./scripts/build.sh clang Debug --asan` | `./scripts/core/build -t Debug -s asan` | Sanitizer support |
| `./scripts/build.sh --coverage` | `./scripts/core/build --coverage` | Coverage build |
| `./scripts/build-dev.sh` | `./scripts/core/build --dev` | Development mode |
| `./scripts/build-dev.sh gcc debug` | `./scripts/core/build --dev -c gcc` | Dev mode + gcc |

### Test Scripts

| Old Command | New Command | Description |
|-------------|-------------|-------------|
| `./scripts/test.sh` | `./scripts/core/test` | Run all tests |
| `./scripts/test.sh -c clang -t Debug` | `./scripts/core/test -c clang -t Debug` | Specify build config |
| `./scripts/test.sh -C` | `./scripts/core/test --coverage` | Coverage testing |
| `./scripts/test.sh -f "*timer*"` | `./scripts/core/test --filter "*timer*"` | Filter tests |
| `./scripts/test.sh -v` | `./scripts/core/test --verbose` | Verbose output |
| `./scripts/test.sh -r 5` | `./scripts/core/test --repeat 5` | Repeat tests |
| N/A | `./scripts/core/test --unit` | Run unit tests only (new) |
| N/A | `./scripts/core/test --integration` | Run integration tests only (new) |

### Code Quality Scripts

| Old Command | New Command | Description |
|-------------|-------------|-------------|
| `./scripts/lint.sh format` | `./scripts/core/lint format` | Format code |
| `./scripts/lint.sh format-check` | `./scripts/core/lint check` | Check formatting |
| `./scripts/lint.sh lint` | `./scripts/core/lint tidy` | Static analysis |
| `./scripts/lint.sh lint-fix` | `./scripts/core/lint tidy-fix` | Auto-fix |
| N/A | `./scripts/core/lint all` | Run all checks (new) |

### Dependency Installation Scripts

| Old Command | New Command | Description |
|-------------|-------------|-------------|
| `./scripts/install_deps.sh` | `./scripts/core/install-deps` | Install dev dependencies |
| `./scripts/install_deps.sh --dev` | `./scripts/core/install-deps` | Same |
| `./scripts/install_deps.sh --runtime` | `./scripts/core/install-deps --runtime` | Runtime only |
| `./scripts/install_runtime.sh` | `./scripts/core/install-deps --runtime` | Merged |
| N/A | `./scripts/core/install-deps --dry-run` | Preview installation (new) |

### Other Scripts

| Old Script | New Script | Status |
|------------|------------|--------|
| `package_release.sh` | `scripts/tools/package-release` | Migrated |
| `docker_deploy.sh` | `scripts/tools/deploy` | Migrated |
| `gcov_wrapper.sh` | `scripts/lib/gcov-wrapper` | Migrated |
| `devcontainer_setup.sh` | `scripts/tools/setup-devcontainer` | Migrated |
| `benchmark_io.sh` | `scripts/tools/benchmark-io` | Migrated |

---

## Detailed Migration Steps

### Step 1: Backup Existing Configuration

If you have custom build configurations or scripts:

```bash
# Backup your modifications
cp scripts/build.sh scripts/build.sh.backup
cp scripts/test.sh scripts/test.sh.backup
```

### Step 2: Update Script Permissions

New scripts need executable permissions:

```bash
chmod +x scripts/core/build
chmod +x scripts/core/test
chmod +x scripts/core/lint
chmod +x scripts/core/install-deps
```

### Step 3: Test New Scripts

Test in non-critical environment first:

```bash
# Test build
./scripts/core/build --help
./scripts/core/build --dev

# Test test script
./scripts/core/test --help
./scripts/core/test --unit

# Test code quality
./scripts/core/lint --help
./scripts/core/lint check
```

### Step 4: Update CI/CD Configuration

#### GitHub Actions

**Old configuration**:
```yaml
- name: Build
  run: ./scripts/build.sh clang Release

- name: Test
  run: ./scripts/test.sh -c clang -t Release -C
```

**New configuration**:
```yaml
- name: Build
  run: ./scripts/core/build

- name: Test
  run: ./scripts/core/test --coverage
```

#### GitLab CI

**Old configuration**:
```yaml
build:
  script:
    - ./scripts/build.sh gcc Debug
    
test:
  script:
    - ./scripts/test.sh -c gcc -t Debug
```

**New configuration**:
```yaml
build:
  script:
    - ./scripts/core/build -c gcc -t Debug
    
test:
  script:
    - ./scripts/core/test -c gcc -t Debug
```

### Step 5: Update Documentation and Script References

Search for old script references in project documentation and update:

```bash
# Find references that need updating
grep -r "build.sh" docs/
grep -r "test.sh" docs/
grep -r "install_deps.sh" docs/
```

### Step 6: Update Developer Habits

Changes team members need to know:

1. **Help information**: All new scripts support `--help`
   ```bash
   ./scripts/core/build --help
   ```

2. **Development mode**: Use `--dev` for quick development
   ```bash
   ./scripts/core/build --dev
   ```

3. **Test layers**: Can run different levels of tests separately
   ```bash
   ./scripts/core/test --unit        # Quick unit tests
   ./scripts/core/test --integration # Integration tests
   ./scripts/core/test --e2e         # End-to-end tests
   ```

---

## Frequently Asked Questions

### Q1: When will old scripts be removed?

**A**: Old scripts were cleaned up on 2026-02-24. All functionality has been migrated to `scripts/core/` and `scripts/tools/`.

### Q2: What if new scripts don't work?

**A**: 
1. Check detailed output: `./scripts/core/build --verbose`
2. Check if `scripts/lib/common.sh` exists
3. Can temporarily fall back to old scripts
4. Report issue to tracker

### Q3: How to switch between old and new scripts?

**A**: Both can coexist. New script names removed `.sh` extension:
```bash
./scripts/build.sh        # Old version
./scripts/core/build      # New version
```

### Q4: How to migrate custom build configurations?

**A**: New scripts provide more configuration options:

**Old way** (hardcoded):
```bash
# Modify in build.sh
BUILD_DIR="my-custom-dir"
```

**New way** (command line parameter):
```bash
./scripts/core/build --build-dir my-custom-dir
```

### Q5: Any performance changes?

**A**: New script performance is same or better:
- Less duplicate code execution
- Smarter cache checking
- Optional parallelization

---

## Known Issues and Solutions

### Issue 1: "common.sh not found"

**Symptom**: Error running new script that common.sh cannot be found

**Solution**:
```bash
# Ensure file exists
ls scripts/lib/common.sh

# If doesn't exist, re-pull
git pull origin main
```

### Issue 2: Permission errors

**Symptom**: `Permission denied`

**Solution**:
```bash
chmod +x scripts/lib/common.sh
chmod +x scripts/core/build
chmod +x scripts/core/test
chmod +x scripts/core/lint
chmod +x scripts/core/install-deps
```

### Issue 3: Parameter incompatibility

**Symptom**: Old parameter format doesn't work

**Solution**: Check help information to understand new format
```bash
./scripts/core/build --help
```

---

## Rollback Guide

If temporarily reverting to old scripts is needed:

### Option 1: Use Old Script Names

```bash
# Old scripts still available
./scripts/build.sh
./scripts/test.sh
./scripts/lint.sh
```

### Option 2: Git Revert

```bash
# Revert to pre-refactor version
git checkout <pre-refactor-commit> -- scripts/
```

### Option 3: Use Backup

```bash
# If you backed up
cp scripts/build.sh.backup scripts/build.sh
```

---

## Getting Help

### Documentation Resources

- **Script documentation**: `scripts/README.md`
- **Test documentation**: `tests/README.md`
- **API documentation**: Run `doxygen` to generate

### Command Help

Each script has detailed help:
```bash
./scripts/core/build --help
./scripts/core/test --help
./scripts/core/lint --help
./scripts/core/install-deps --help
```

### Community Support

- Submit issue: [GitHub Issues](https://github.com/your-repo/fastq-tools/issues)
- View examples: `docs/dev/build.md`
- Developer docs: `docs/dev/`

---

## Feedback

If you encounter issues or have improvement suggestions during migration:

1. Submit GitHub Issue and tag `migration`
2. In issue describe:
   - Old command you tried
   - Expected new command
   - Problem encountered
3. We will respond promptly and update this document

---

## Timeline

| Date | Milestone |
|------|-----------|
| 2026-01-08 | ✅ New script system released |
| 2026-02-01 | Old scripts marked deprecated |
| 2026-04-01 | Old scripts moved to `deprecated/` |
| 2026-07-01 | Completely removed old scripts |

**Current stage**: Old and new scripts coexist, recommend using new scripts

---

## Checklist

After migration is complete, confirm the following items:

- [ ] All CI/CD pipelines use new scripts
- [ ] Documentation script references are updated
- [ ] Team members are aware of new commands
- [ ] Custom scripts are migrated
- [ ] Tests pass
- [ ] Old scripts backed up (if custom modifications exist)

Once completed, you have successfully migrated to the new script system! 🎉
