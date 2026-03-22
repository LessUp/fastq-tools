#!/usr/bin/env python3
"""
Compiler benchmark visualization report.

Consumes compiler comparison JSON (from compiler_benchmark.py) and generates
Markdown/HTML reports with charts.
"""

from __future__ import annotations

import argparse
import json
import math
import os
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional

try:
    import matplotlib.pyplot as plt

    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False


SUMMARY_METRICS = [
    ("throughput_mbps", "Throughput (MB/s)", True),
    ("mean_time_ms", "Mean Time (ms)", False),
    ("max_rss_kb", "Max RSS (MB)", False),
    ("binary_size_bytes", "Binary Size (MB)", False),
]


def load_report(path: Path) -> Dict[str, Any]:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def format_value(metric: str, value: float) -> str:
    if metric == "binary_size_bytes":
        return f"{value / (1024 * 1024):.2f}"
    if metric == "max_rss_kb":
        return f"{value / 1024:.2f}"
    return f"{value:.2f}"


def format_delta(delta: float) -> str:
    return f"{delta:+.1f}%"


def short_benchmark_name(name: str, limit: int = 32) -> str:
    if "/" in name:
        name = name.split("/")[-1]
    if len(name) > limit:
        return name[: limit - 3] + "..."
    return name


def ensure_dir(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)


def generate_summary_table(comparison: Dict[str, Any]) -> str:
    lines = [
        "| Metric | GCC | Clang | Δ% |",
        "|--------|-----|-------|----|",
    ]
    summary = comparison.get("summary", {})
    for metric, label, _ in SUMMARY_METRICS:
        entry = summary.get(metric, {})
        gcc_val = entry.get("gcc", 0.0)
        clang_val = entry.get("clang", 0.0)
        delta = entry.get("delta_percent", 0.0)
        lines.append(
            "| {} | {} | {} | {} |".format(
                label,
                format_value(metric, gcc_val),
                format_value(metric, clang_val),
                format_delta(delta),
            )
        )
    return "\n".join(lines)


def generate_benchmark_table(comparison: Dict[str, Any]) -> str:
    lines = [
        "| Benchmark | GCC Time (ms) | Clang Time (ms) | Δ% | GCC Throughput (MB/s) | Clang Throughput (MB/s) | Δ% |",
        "|----------|---------------|----------------|----|-----------------------|------------------------|----|",
    ]
    for entry in sorted(comparison.get("benchmarks", []), key=lambda e: e.get("name", "")):
        metrics = entry.get("metrics", {})
        time_metric = metrics.get("mean_time_ms", {})
        throughput_metric = metrics.get("throughput_mbps", {})
        lines.append(
            "| {} | {} | {} | {} | {} | {} | {} |".format(
                entry.get("name", "-"),
                format_value("mean_time_ms", float(time_metric.get("gcc", 0.0))),
                format_value("mean_time_ms", float(time_metric.get("clang", 0.0))),
                format_delta(float(time_metric.get("delta_percent", 0.0))),
                format_value("throughput_mbps", float(throughput_metric.get("gcc", 0.0))),
                format_value("throughput_mbps", float(throughput_metric.get("clang", 0.0))),
                format_delta(float(throughput_metric.get("delta_percent", 0.0))),
            )
        )
    return "\n".join(lines)


def compute_radar_scores(compilers: Dict[str, Any]) -> Dict[str, List[float]]:
    values = {
        name: {
            "throughput_mbps": float(data["summary"].get("throughput_mbps", 0.0)),
            "mean_time_ms": float(data["summary"].get("mean_time_ms", 0.0)),
            "max_rss_kb": float(data["summary"].get("max_rss_kb", 0.0)),
            "binary_size_bytes": float(data["summary"].get("binary_size_bytes", 0.0)),
        }
        for name, data in compilers.items()
    }

    max_throughput = max((v["throughput_mbps"] for v in values.values()), default=0.0)
    min_time = min((v["mean_time_ms"] for v in values.values()), default=0.0)
    min_rss = min((v["max_rss_kb"] for v in values.values()), default=0.0)
    min_binary = min((v["binary_size_bytes"] for v in values.values()), default=0.0)

    scores: Dict[str, List[float]] = {}
    for name, metric in values.items():
        throughput_score = metric["throughput_mbps"] / max_throughput if max_throughput else 0.0
        time_score = (min_time / metric["mean_time_ms"]) if metric["mean_time_ms"] else 0.0
        rss_score = (min_rss / metric["max_rss_kb"]) if metric["max_rss_kb"] else 0.0
        binary_score = (
            min_binary / metric["binary_size_bytes"] if metric["binary_size_bytes"] else 0.0
        )
        scores[name] = [throughput_score, time_score, rss_score, binary_score]
    return scores


def plot_summary_chart(comparison: Dict[str, Any], output_path: Path) -> None:
    summary = comparison.get("summary", {})
    labels = [label for _, label, _ in SUMMARY_METRICS]
    gcc_vals = [summary.get(metric, {}).get("gcc", 0.0) for metric, _, _ in SUMMARY_METRICS]
    clang_vals = [summary.get(metric, {}).get("clang", 0.0) for metric, _, _ in SUMMARY_METRICS]

    gcc_vals = [
        v / (1024 * 1024) if metric == "binary_size_bytes" else v / 1024 if metric == "max_rss_kb" else v
        for (metric, _, _), v in zip(SUMMARY_METRICS, gcc_vals)
    ]
    clang_vals = [
        v / (1024 * 1024) if metric == "binary_size_bytes" else v / 1024 if metric == "max_rss_kb" else v
        for (metric, _, _), v in zip(SUMMARY_METRICS, clang_vals)
    ]

    positions = list(range(len(labels)))
    width = 0.35

    fig, ax = plt.subplots(figsize=(10, 5))
    ax.bar([p - width / 2 for p in positions], gcc_vals, width, label="GCC")
    ax.bar([p + width / 2 for p in positions], clang_vals, width, label="Clang")
    ax.set_xticks(positions)
    ax.set_xticklabels(labels, rotation=15, ha="right")
    ax.set_title("Compiler Summary Metrics")
    ax.legend()
    ax.grid(axis="y", alpha=0.3)
    plt.tight_layout()
    plt.savefig(output_path, format="svg", dpi=150)
    plt.close()


def plot_benchmark_chart(
    compilers: Dict[str, Any],
    metric_key: str,
    title: str,
    output_path: Path,
    limit: int = 12,
) -> None:
    gcc_metrics = {m["name"]: m for m in compilers.get("gcc", {}).get("benchmarks", [])}
    clang_metrics = {m["name"]: m for m in compilers.get("clang", {}).get("benchmarks", [])}

    names = [name for name in gcc_metrics.keys() if name in clang_metrics]
    names.sort(key=lambda n: gcc_metrics[n].get(metric_key, 0.0), reverse=True)
    if limit:
        names = names[:limit]

    labels = [short_benchmark_name(name) for name in names]
    gcc_vals = [float(gcc_metrics[name].get(metric_key, 0.0)) for name in names]
    clang_vals = [float(clang_metrics[name].get(metric_key, 0.0)) for name in names]

    positions = list(range(len(labels)))
    width = 0.35

    fig, ax = plt.subplots(figsize=(12, 6))
    ax.bar([p - width / 2 for p in positions], gcc_vals, width, label="GCC")
    ax.bar([p + width / 2 for p in positions], clang_vals, width, label="Clang")
    ax.set_xticks(positions)
    ax.set_xticklabels(labels, rotation=30, ha="right")
    ax.set_title(title)
    ax.legend()
    ax.grid(axis="y", alpha=0.3)
    plt.tight_layout()
    plt.savefig(output_path, format="svg", dpi=150)
    plt.close()


def plot_radar_chart(compilers: Dict[str, Any], output_path: Path) -> None:
    scores = compute_radar_scores(compilers)
    labels = ["Throughput", "Latency", "Memory", "Binary Size"]
    angles = [n / len(labels) * 2 * math.pi for n in range(len(labels))]
    angles.append(angles[0])

    fig = plt.figure(figsize=(6, 6))
    ax = plt.subplot(111, polar=True)

    for compiler, values in scores.items():
        values = values + values[:1]
        ax.plot(angles, values, label=compiler.upper())
        ax.fill(angles, values, alpha=0.15)

    ax.set_thetagrids([a * 180 / math.pi for a in angles[:-1]], labels)
    ax.set_ylim(0, 1.0)
    ax.set_title("Compiler Radar (Normalized)")
    ax.legend(loc="upper right", bbox_to_anchor=(1.2, 1.1))
    plt.tight_layout()
    plt.savefig(output_path, format="svg", dpi=150)
    plt.close()


def generate_markdown_report(
    report: Dict[str, Any],
    charts: Dict[str, Optional[Path]],
    output_path: Path,
) -> str:
    metadata = report.get("metadata", {})
    comparison = report.get("comparison", {})
    compilers = report.get("compilers", {})

    lines = [
        "# FastQTools Compiler Benchmark Report",
        "",
        f"**Generated:** {metadata.get('timestamp', '')}",
        f"**Git Commit:** {metadata.get('git_commit', 'unknown')} ({metadata.get('git_branch', 'unknown')})",
        f"**CPU:** {metadata.get('cpu_model', 'unknown')} ({metadata.get('core_count', 0)} cores)",
        f"**OS:** {metadata.get('os_version', 'unknown')}",
        "",
        "## Compilers",
        "",
    ]

    for name, data in compilers.items():
        lines.append(f"- **{name.upper()}**: {data.get('compiler_version', 'unknown')}")
    lines.append("")

    lines.append("## Summary")
    lines.append("")
    lines.append(generate_summary_table(comparison))
    lines.append("")

    if charts:
        lines.append("## Charts")
        lines.append("")
        for title, chart_path in charts.items():
            if chart_path:
                rel_path = resolve_relative_path(chart_path, output_path.parent)
                lines.append(f"![{title}]({rel_path})")
                lines.append("")
        lines.append("")

    lines.append("## Per-benchmark Comparison")
    lines.append("")
    lines.append(generate_benchmark_table(comparison))
    lines.append("")

    content = "\n".join(lines)
    output_path.write_text(content, encoding="utf-8")
    return content


def generate_html_report(
    report: Dict[str, Any],
    charts: Dict[str, Optional[Path]],
    output_path: Path,
) -> str:
    metadata = report.get("metadata", {})
    comparison = report.get("comparison", {})
    compilers = report.get("compilers", {})

    summary_table = generate_summary_table(comparison)
    benchmark_table = generate_benchmark_table(comparison)

    compiler_list = "".join(
        f"<li><strong>{name.upper()}</strong>: {data.get('compiler_version', 'unknown')}</li>"
        for name, data in compilers.items()
    )

    chart_html = ""
    for title, chart_path in charts.items():
        if chart_path:
            rel_path = resolve_relative_path(chart_path, output_path.parent)
            chart_html += f"<figure><img src=\"{rel_path}\" alt=\"{title}\"><figcaption>{title}</figcaption></figure>"

    html = f"""<!DOCTYPE html>
<html>
<head>
<meta charset=\"utf-8\">
<title>FastQTools Compiler Benchmark Report</title>
<style>
body {{ font-family: Arial, sans-serif; margin: 24px; color: #1a1a1a; }}
code {{ background: #f5f5f5; padding: 2px 4px; }}
table {{ border-collapse: collapse; width: 100%; margin-bottom: 24px; }}
th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
th {{ background: #f0f0f0; }}
figure {{ margin: 16px 0; }}
figcaption {{ font-size: 0.9em; color: #555; }}
</style>
</head>
<body>
<h1>FastQTools Compiler Benchmark Report</h1>
<p><strong>Generated:</strong> {metadata.get('timestamp', '')}</p>
<p><strong>Git Commit:</strong> {metadata.get('git_commit', 'unknown')} ({metadata.get('git_branch', 'unknown')})</p>
<p><strong>CPU:</strong> {metadata.get('cpu_model', 'unknown')} ({metadata.get('core_count', 0)} cores)</p>
<p><strong>OS:</strong> {metadata.get('os_version', 'unknown')}</p>

<h2>Compilers</h2>
<ul>
{compiler_list}
</ul>

<h2>Summary</h2>
{markdown_table_to_html(summary_table)}

<h2>Charts</h2>
{chart_html}

<h2>Per-benchmark Comparison</h2>
{markdown_table_to_html(benchmark_table)}
</body>
</html>"""

    output_path.write_text(html, encoding="utf-8")
    return html


def markdown_table_to_html(table: str) -> str:
    lines = [line for line in table.splitlines() if line.strip()]
    if len(lines) < 2:
        return ""
    headers = [h.strip() for h in lines[0].strip("|").split("|")]
    rows = []
    for line in lines[2:]:
        rows.append([c.strip() for c in line.strip("|").split("|")])

    thead = "".join(f"<th>{h}</th>" for h in headers)
    body = "".join(
        "<tr>" + "".join(f"<td>{cell}</td>" for cell in row) + "</tr>" for row in rows
    )
    return f"<table><thead><tr>{thead}</tr></thead><tbody>{body}</tbody></table>"


def resolve_relative_path(path: Path, base_dir: Path) -> str:
    try:
        return path.relative_to(base_dir).as_posix()
    except ValueError:
        return Path(os.path.relpath(path, base_dir)).as_posix()


def generate_charts(report: Dict[str, Any], charts_dir: Path, enable: bool) -> Dict[str, Optional[Path]]:
    if not enable or not HAS_MATPLOTLIB:
        if enable and not HAS_MATPLOTLIB:
            print("Warning: matplotlib not available, skipping charts", file=sys.stderr)
        return {}

    ensure_dir(charts_dir)
    comparison = report.get("comparison", {})
    compilers = report.get("compilers", {})

    summary_path = charts_dir / "summary-bar.svg"
    plot_summary_chart(comparison, summary_path)

    throughput_path = charts_dir / "benchmark-throughput.svg"
    plot_benchmark_chart(compilers, "throughput_mbps", "Benchmark Throughput", throughput_path)

    latency_path = charts_dir / "benchmark-latency.svg"
    plot_benchmark_chart(compilers, "mean_time_ms", "Benchmark Mean Time (ms)", latency_path)

    radar_path = charts_dir / "compiler-radar.svg"
    plot_radar_chart(compilers, radar_path)

    return {
        "Summary Metrics": summary_path,
        "Benchmark Throughput": throughput_path,
        "Benchmark Mean Time": latency_path,
        "Compiler Radar": radar_path,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate compiler benchmark visualization report")
    parser.add_argument("--input", "-i", type=Path, help="Compiler benchmark JSON input")
    parser.add_argument("--output", "-o", type=Path, help="Output report path")
    parser.add_argument("--format", "-f", choices=["markdown", "html", "summary"], default="markdown")
    parser.add_argument("--no-charts", action="store_true", help="Skip chart generation")

    args = parser.parse_args()

    project_root = Path(__file__).resolve().parents[2]
    compiler_dir = project_root / "docs" / "benchmark-reports" / "compiler"

    input_path = args.input or (compiler_dir / "latest.json")
    if not input_path.exists():
        raise FileNotFoundError(f"Compiler benchmark JSON not found: {input_path}")

    report = load_report(input_path)
    charts_dir = compiler_dir / "charts"

    charts = generate_charts(report, charts_dir, enable=not args.no_charts)

    if args.format == "summary":
        summary = generate_summary_table(report.get("comparison", {}))
        print(summary)
        return

    if args.output:
        output_path = args.output
    else:
        output_path = compiler_dir / ("latest.html" if args.format == "html" else "latest.md")

    if args.format == "html":
        generate_html_report(report, charts, output_path)
    else:
        generate_markdown_report(report, charts, output_path)

    print(f"Visualization report generated: {output_path}")


if __name__ == "__main__":
    main()
