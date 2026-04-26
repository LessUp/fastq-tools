# RFC-0006: Benchmark Maintenance and Release Policy

> **Status**: Active  
> **Created**: 2026-04-26  
> **Authors**: FastQTools Core Team  
> **Motivation**: Phase 1 baseline consolidation; clarify benchmark obligations across release cycles

## Context

The benchmark system (RFC-0003) provides systematic performance tracking. However, the operational and quality standards for benchmark results published to users need clarification:

1. **When do we publish benchmarks?** (Per commit vs. per release)
2. **Which hardware / OS combinations are official?** (SLA vs. informational)
3. **How do we handle regressions?** (Abort release vs. document vs. waive)
4. **Archive policy**: How long do we keep historical results?

## Decision

### Publication Policy

#### Benchmark Results Publication

- **Published in Release Notes** (`CHANGELOG.md`, GitHub Releases):
  - Latest stable benchmark metrics (1–2 representative results)
  - Hardware: AMD Ryzen 9 5900X (primary SLA platform)
  - OS: Ubuntu 22.04 LTS (primary SLA platform)
  - Compiler: Clang 21.0 Release build
  - Thread: Single-threaded throughput (user can parallelize)
  - Status: **Guaranteed** for releases >= v3.1.0

- **Published in GitHub Pages** (`docs/performance/benchmark-report.md`):
  - Complete benchmark dataset from latest CI run
  - All configurations (1–8 threads, multiple input sizes)
  - Multiple platforms (Debian, Ubuntu, Alpine, musl, aarch64 if available)
  - Last updated timestamp and Git commit
  - Status: **Informational** (may fluctuate, not guaranteed SLA)

- **Stored in Repository** (`docs/performance/benchmark-reports/results/`):
  - All benchmark runs retained for 6 months (by timestamp)
  - Organized as `YYYY-MM-DD-hh-mm-ss-<branch>.json`
  - Older runs moved to archive or deleted per `openspec/archive/`

#### SLA Metrics (Guaranteed for Releases)

For any release >= v3.1.0, the following SLA applies:

| Metric | Baseline | Tolerance | Platform |
|--------|----------|-----------|----------|
| FASTQ read throughput | 1696 MB/s | ±15% | Ryzen 5900X, Clang 21, `-O3` |
| FASTQ write throughput | 1.76M reads/sec | ±15% | Ryzen 5900X, Clang 21, `-O3` |
| Stat pass throughput | 302 MB/s | ±20% | Ryzen 5900X, Clang 21, Release |
| Filter pass throughput | 1.67M reads/sec | ±15% | Ryzen 5900X, Clang 21, Release |

**Notes**:
- These are single-threaded results on a synthetic 150 bp FASTQ dataset (100K reads).
- Tolerance accounts for OS noise, disk I/O variance, and minor compiler variations.
- If a release fails these thresholds:
  - **Deviation < 15%**: Release proceeds with a note in release notes ("Performance variance detected").
  - **Deviation 15–25%**: Release proceeds with investigation note ("Performance investigation recommended before production deployment").
  - **Deviation > 25%**: Release blocked until root cause identified (unless explicitly waived by maintainers).

### CI Benchmark Workflow

#### Trigger Rules

| Trigger | Frequency | Reports |
|---------|-----------|---------|
| Push to `master` | Per commit | ✓ Results stored, no blockage |
| PR to `master` | Per push | ✓ Results stored, comparison generated |
| Release tag (v*) | Per release | ✓ Results verified against SLA |
| Scheduled | Weekly (Saturday 02:00 UTC) | ✓ Long-running configs, trend analysis |

#### Regression Response

1. **CI finds > 20% regression**:
   - Flag in PR / commit check (informational, not blocking)
   - Log to `docs/performance/benchmark-reports/regressions.md`
   - Notify maintainers (comment on PR if available)

2. **Release benchmark fails SLA**:
   - Email maintainer list (or post issue)
   - Release pipeline pauses for review
   - Maintainer decision: proceed with note, investigate, or skip release

3. **Regression root-cause identified**:
   - Document in corresponding commit's changelog entry
   - Update SLA baseline if intentional (via RFC update)
   - Re-run benchmark to confirm fix

### Hardware and Platform Policy

#### Primary SLA Platform
- **CPU**: AMD Ryzen 9 5900X (12-core, 3.7–4.6 GHz)
- **RAM**: ≥ 32 GB
- **Disk**: NVMe SSD (≥ 1 GB/s throughput)
- **OS**: Ubuntu 22.04 LTS + Debian 12 (glibc)
- **Compiler**: Clang 21.0 (Release configuration)

#### Informational Platforms (no SLA, for awareness)
- Alpine Linux (musl libc)
- aarch64 (ARM64) on various OS
- Older/newer Clang versions (18–22)
- GCC 11, GCC 15

#### Adding New Platforms to SLA
- Requires RFC update (amendment to this document)
- Include: hardware specs, baseline measurements, tolerance band
- Maintainer consensus required

### Benchmark Maintenance Schedule

| Task | Frequency | Owner |
|------|-----------|-------|
| Review new regressions | Per PR / commit | Automated check |
| Run full benchmark suite | Weekly (scheduled) | GitHub Actions |
| Update SLA baseline | Per major release | Release manager |
| Archive old results | Monthly | Cleanup script |
| Hardware refresh assessment | Yearly | Architecture team |

### Historical Data Retention

- **Recent results** (< 3 months): Retain all
- **Archival period** (3–6 months): Aggregate by month, retain top 5 runs
- **Historical archive** (> 6 months): Summarize as trend, remove raw files

Archived data stored in:
```
openspec/archive/YYYY/MM-benchmark-summary/
├── monthly-aggregates.json
├── trend-analysis.md
└── raw-runs/ (if under 50 MB)
```

### Regression Detection Thresholds

- **Warning**: 10% degradation → informational log
- **Critical**: 20% degradation → CI check flag
- **Release-blocking**: 25% degradation on SLA platform

### Change to Benchmarks SLA

To change SLA metrics or tolerance bands:
1. File RFC in `openspec/changes/` with justification
2. Include data from ≥ 3 benchmark runs showing trend
3. Document why current baseline no longer applicable
4. Obtain maintainer consensus
5. Update this document + `CHANGELOG.md` + release notes

---

## Consequences

### Positive
- Clear expectations for published performance
- Automated regression detection prevents silent degradation
- Release blocking prevents shipping major regressions
- Transparent historical data for users

### Negative
- Adds operational overhead (SLA monitoring, hardware maintenance)
- Release may be delayed by benchmark failures
- Risk of SLA tolerance being too tight or too loose (requires iteration)

## Related Documents

- [RFC-0003: Benchmark System](0003-benchmark-system.md)
- [Product Specification](../product/fastq-processing.md)
- [GitHub Actions Benchmark Workflow](.github/workflows/benchmark.yml)
- [Benchmark Guide](../../../docs/dev/benchmark-guide.md)
