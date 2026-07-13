#!/usr/bin/env python3

import json
import math
import statistics
import sys
from pathlib import Path


def to_milliseconds(value: float, unit: str) -> float:
    factors = {"ns": 1e-6, "us": 1e-3, "ms": 1.0, "s": 1e3}
    return value * factors[unit]


def percentile95(values: list[float]) -> float:
    ordered = sorted(values)
    return ordered[max(0, math.ceil(len(ordered) * 0.95) - 1)]


def summarize(path: Path) -> dict[str, object] | None:
    with path.open(encoding="utf-8") as stream:
        payload = json.load(stream)

    samples = [
        item
        for item in payload.get("benchmarks", [])
        if item.get("run_type") == "iteration" and "aggregate_name" not in item
    ]
    if not samples:
        return None

    times = [to_milliseconds(item["real_time"], item["time_unit"]) for item in samples]
    throughputs = [item.get("bytes_per_second", 0.0) / (1024 * 1024) for item in samples]
    rss_path = path.with_suffix(".rss_kb")
    peak_rss_mib = float(rss_path.read_text().strip()) / 1024 if rss_path.exists() else 0.0

    return {
        "name": samples[0].get("run_name", samples[0]["name"]),
        "p50_ms": statistics.median(times),
        "p95_ms": percentile95(times),
        "throughput_mib_s": statistics.median(throughputs),
        "peak_rss_mib": peak_rss_mib,
        "samples": len(samples),
    }


def main() -> int:
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <result_dir>", file=sys.stderr)
        return 2

    result_dir = Path(sys.argv[1])
    rows = [row for path in sorted(result_dir.glob("*.json")) if (row := summarize(path))]
    if not rows:
        print("No backend benchmark samples found", file=sys.stderr)
        return 1

    lines = [
        "# Execution backend comparison",
        "",
        "| Benchmark | Samples | p50 (ms) | p95 (ms) | Median MiB/s | Peak RSS (MiB) |",
        "|---|---:|---:|---:|---:|---:|",
    ]
    for row in rows:
        lines.append(
            f"| {row['name']} | {row['samples']} | {row['p50_ms']:.2f} | "
            f"{row['p95_ms']:.2f} | {row['throughput_mib_s']:.2f} | "
            f"{row['peak_rss_mib']:.1f} |"
        )

    report = "\n".join(lines) + "\n"
    (result_dir / "summary.md").write_text(report, encoding="utf-8")
    print(report, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
