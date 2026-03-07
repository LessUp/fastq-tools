#!/usr/bin/env python3
"""
性能回归检测器

比较基准测试结果，检测性能回归。
"""

import json
import sys
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Dict, List, Optional, Tuple

from report_generator import BenchmarkReport, BenchmarkResult, ReportGenerator


class Severity(Enum):
    OK = "ok"
    WARNING = "warning"
    CRITICAL = "critical"


@dataclass
class RegressionResult:
    """回归检测结果"""
    benchmark_name: str
    metric_name: str
    baseline_value: float
    current_value: float
    change_percent: float
    severity: Severity
    
    @property
    def is_regression(self) -> bool:
        return self.severity in (Severity.WARNING, Severity.CRITICAL)


@dataclass
class RegressionReport:
    """回归检测报告"""
    baseline_commit: str
    current_commit: str
    results: List[RegressionResult]
    
    @property
    def has_warning(self) -> bool:
        return any(r.severity == Severity.WARNING for r in self.results)
    
    @property
    def has_critical(self) -> bool:
        return any(r.severity == Severity.CRITICAL for r in self.results)
    
    @property
    def has_regression(self) -> bool:
        return self.has_warning or self.has_critical


class RegressionDetector:
    """性能回归检测器"""
    
    def __init__(self, 
                 warning_threshold: float = 0.10,
                 critical_threshold: float = 0.20,
                 custom_thresholds: Optional[Dict[str, Tuple[float, float]]] = None):
        """
        初始化回归检测器
        
        Args:
            warning_threshold: 警告阈值（默认 10%）
            critical_threshold: 严重阈值（默认 20%）
            custom_thresholds: 自定义阈值 {metric_name: (warning, critical)}
        """
        self.warning_threshold = warning_threshold
        self.critical_threshold = critical_threshold
        self.custom_thresholds = custom_thresholds or {}
    
    def get_thresholds(self, metric_name: str) -> Tuple[float, float]:
        """获取指定指标的阈值"""
        if metric_name in self.custom_thresholds:
            return self.custom_thresholds[metric_name]
        return (self.warning_threshold, self.critical_threshold)
    
    def compare_metric(self, 
                       benchmark_name: str,
                       metric_name: str,
                       baseline_value: float, 
                       current_value: float,
                       higher_is_better: bool = False) -> RegressionResult:
        """
        比较单个指标
        
        Args:
            benchmark_name: 基准测试名称
            metric_name: 指标名称
            baseline_value: 基线值
            current_value: 当前值
            higher_is_better: True 表示值越高越好（如吞吐量），False 表示值越低越好（如延迟）
        """
        if baseline_value == 0:
            change_percent = 0.0
        else:
            change_percent = (current_value - baseline_value) / baseline_value
        
        # 如果 higher_is_better，则负变化是回归
        # 如果 lower_is_better，则正变化是回归
        if higher_is_better:
            regression_change = -change_percent  # 吞吐量下降是回归
        else:
            regression_change = change_percent   # 延迟增加是回归
        
        warning_thresh, critical_thresh = self.get_thresholds(metric_name)
        
        if regression_change >= critical_thresh:
            severity = Severity.CRITICAL
        elif regression_change >= warning_thresh:
            severity = Severity.WARNING
        else:
            severity = Severity.OK
        
        return RegressionResult(
            benchmark_name=benchmark_name,
            metric_name=metric_name,
            baseline_value=baseline_value,
            current_value=current_value,
            change_percent=change_percent * 100,  # 转换为百分比
            severity=severity
        )
    
    def compare_results(self, 
                        baseline_result: BenchmarkResult,
                        current_result: BenchmarkResult) -> List[RegressionResult]:
        """比较两个基准测试结果"""
        results = []
        
        # 比较执行时间（越低越好）
        if baseline_result.mean_time_ns > 0:
            results.append(self.compare_metric(
                baseline_result.name,
                "mean_time_ns",
                baseline_result.mean_time_ns,
                current_result.mean_time_ns,
                higher_is_better=False
            ))
        
        # 比较吞吐量（越高越好）
        if baseline_result.throughput_mbps > 0:
            results.append(self.compare_metric(
                baseline_result.name,
                "throughput_mbps",
                baseline_result.throughput_mbps,
                current_result.throughput_mbps,
                higher_is_better=True
            ))
        
        if baseline_result.throughput_reads_per_sec > 0:
            results.append(self.compare_metric(
                baseline_result.name,
                "throughput_reads_per_sec",
                baseline_result.throughput_reads_per_sec,
                current_result.throughput_reads_per_sec,
                higher_is_better=True
            ))
        
        return results
    
    def compare_reports(self, 
                        baseline: BenchmarkReport,
                        current: BenchmarkReport) -> RegressionReport:
        """比较两个完整的基准测试报告"""
        # 建立基线结果索引
        baseline_map = {r.name: r for r in baseline.results}
        
        all_results = []
        
        for current_result in current.results:
            if current_result.name in baseline_map:
                baseline_result = baseline_map[current_result.name]
                results = self.compare_results(baseline_result, current_result)
                all_results.extend(results)
        
        return RegressionReport(
            baseline_commit=baseline.metadata.git_commit,
            current_commit=current.metadata.git_commit,
            results=all_results
        )
    
    def format_report(self, report: RegressionReport, verbose: bool = False) -> str:
        """格式化回归报告"""
        lines = [
            "# Performance Regression Report",
            "",
            f"**Baseline:** {report.baseline_commit}",
            f"**Current:** {report.current_commit}",
            "",
        ]
        
        if report.has_critical:
            lines.append("⛔ **CRITICAL REGRESSIONS DETECTED**")
            lines.append("")
        elif report.has_warning:
            lines.append("⚠️ **Warnings detected**")
            lines.append("")
        else:
            lines.append("✅ **No regressions detected**")
            lines.append("")
        
        # 按严重程度分组
        critical = [r for r in report.results if r.severity == Severity.CRITICAL]
        warnings = [r for r in report.results if r.severity == Severity.WARNING]
        ok = [r for r in report.results if r.severity == Severity.OK]
        
        if critical:
            lines.append("## Critical Regressions")
            lines.append("")
            lines.append("| Benchmark | Metric | Baseline | Current | Change |")
            lines.append("|-----------|--------|----------|---------|--------|")
            for r in critical:
                lines.append(f"| {r.benchmark_name} | {r.metric_name} | {r.baseline_value:.2f} | {r.current_value:.2f} | {r.change_percent:+.1f}% |")
            lines.append("")
        
        if warnings:
            lines.append("## Warnings")
            lines.append("")
            lines.append("| Benchmark | Metric | Baseline | Current | Change |")
            lines.append("|-----------|--------|----------|---------|--------|")
            for r in warnings:
                lines.append(f"| {r.benchmark_name} | {r.metric_name} | {r.baseline_value:.2f} | {r.current_value:.2f} | {r.change_percent:+.1f}% |")
            lines.append("")
        
        if verbose and ok:
            lines.append("## Passed")
            lines.append("")
            lines.append("| Benchmark | Metric | Baseline | Current | Change |")
            lines.append("|-----------|--------|----------|---------|--------|")
            for r in ok:
                lines.append(f"| {r.benchmark_name} | {r.metric_name} | {r.baseline_value:.2f} | {r.current_value:.2f} | {r.change_percent:+.1f}% |")
            lines.append("")
        
        return "\n".join(lines)
    
    def format_ci_output(self, report: RegressionReport) -> str:
        """格式化 CI 输出（GitHub Actions 注解格式）"""
        lines = []
        
        for r in report.results:
            if r.severity == Severity.CRITICAL:
                lines.append(f"::error::Performance regression in {r.benchmark_name}: "
                           f"{r.metric_name} changed by {r.change_percent:+.1f}%")
            elif r.severity == Severity.WARNING:
                lines.append(f"::warning::Performance warning in {r.benchmark_name}: "
                           f"{r.metric_name} changed by {r.change_percent:+.1f}%")
        
        if not report.has_regression:
            lines.append("::notice::All performance checks passed")
        
        return "\n".join(lines)


def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="Detect performance regressions")
    parser.add_argument("baseline", type=Path, help="Baseline JSON file")
    parser.add_argument("current", type=Path, help="Current JSON file")
    parser.add_argument("--warning-threshold", type=float, default=0.10,
                        help="Warning threshold (default: 0.10 = 10%%)")
    parser.add_argument("--critical-threshold", type=float, default=0.20,
                        help="Critical threshold (default: 0.20 = 20%%)")
    parser.add_argument("--ci", action="store_true", help="CI mode output")
    parser.add_argument("--verbose", action="store_true", help="Show all results")
    parser.add_argument("--google-benchmark", action="store_true",
                        help="Input is Google Benchmark JSON format")
    
    args = parser.parse_args()
    
    generator = ReportGenerator()
    
    if args.google_benchmark:
        baseline = generator.load_google_benchmark_json(args.baseline)
        current = generator.load_google_benchmark_json(args.current)
    else:
        baseline = generator.load_result(args.baseline)
        current = generator.load_result(args.current)
    
    detector = RegressionDetector(
        warning_threshold=args.warning_threshold,
        critical_threshold=args.critical_threshold
    )
    
    report = detector.compare_reports(baseline, current)
    
    if args.ci:
        print(detector.format_ci_output(report))
    else:
        print(detector.format_report(report, verbose=args.verbose))
    
    # 退出码
    if report.has_critical:
        sys.exit(1)
    elif report.has_warning:
        sys.exit(0)  # 警告不导致失败
    else:
        sys.exit(0)


if __name__ == "__main__":
    main()
