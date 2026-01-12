#!/usr/bin/env python3
"""
Benchmark 报告生成器

从 JSON 结果文件生成 Markdown 报告和可视化图表。
"""

import json
import sys
from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Any

try:
    import matplotlib.pyplot as plt
    import matplotlib.dates as mdates
    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False


@dataclass
class BenchmarkResult:
    """单个基准测试结果"""
    name: str
    category: str
    iterations: int
    mean_time_ns: float
    std_dev_ns: float
    min_time_ns: float = 0.0
    max_time_ns: float = 0.0
    throughput_mbps: float = 0.0
    throughput_reads_per_sec: float = 0.0
    peak_memory_bytes: int = 0
    thread_count: int = 1
    input_size: int = 0


@dataclass
class SystemMetadata:
    """系统元数据"""
    timestamp: str
    git_commit: str
    git_branch: str
    cpu_model: str
    core_count: int
    memory_bytes: int
    os_version: str
    compiler_version: str


@dataclass
class BenchmarkReport:
    """完整的基准测试报告"""
    metadata: SystemMetadata
    results: List[BenchmarkResult] = field(default_factory=list)


class ReportGenerator:
    """报告生成器"""
    
    def __init__(self, output_dir: Path = Path("benchmark_results/reports")):
        self.output_dir = output_dir
        self.charts_dir = output_dir / "charts"
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.charts_dir.mkdir(parents=True, exist_ok=True)
    
    def load_result(self, path: Path) -> BenchmarkReport:
        """加载单个 JSON 结果文件"""
        with open(path, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        metadata = SystemMetadata(
            timestamp=data['metadata'].get('timestamp', ''),
            git_commit=data['metadata'].get('gitCommit', ''),
            git_branch=data['metadata'].get('gitBranch', ''),
            cpu_model=data['metadata'].get('cpuModel', ''),
            core_count=data['metadata'].get('coreCount', 0),
            memory_bytes=data['metadata'].get('memoryBytes', 0),
            os_version=data['metadata'].get('osVersion', ''),
            compiler_version=data['metadata'].get('compilerVersion', '')
        )
        
        results = []
        for r in data.get('results', []):
            results.append(BenchmarkResult(
                name=r.get('name', ''),
                category=r.get('category', ''),
                iterations=r.get('iterations', 0),
                mean_time_ns=r.get('meanTimeNs', 0.0),
                std_dev_ns=r.get('stdDevNs', 0.0),
                min_time_ns=r.get('minTimeNs', 0.0),
                max_time_ns=r.get('maxTimeNs', 0.0),
                throughput_mbps=r.get('throughputMBps', 0.0),
                throughput_reads_per_sec=r.get('throughputReadsPerSec', 0.0),
                peak_memory_bytes=r.get('peakMemoryBytes', 0),
                thread_count=r.get('threadCount', 1),
                input_size=r.get('inputSize', 0)
            ))
        
        return BenchmarkReport(metadata=metadata, results=results)
    
    def load_google_benchmark_json(self, path: Path) -> BenchmarkReport:
        """加载 Google Benchmark 输出的 JSON 文件"""
        with open(path, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        # 从 context 提取系统信息
        context = data.get('context', {})
        metadata = SystemMetadata(
            timestamp=context.get('date', datetime.now().isoformat()),
            git_commit='unknown',
            git_branch='unknown',
            cpu_model=context.get('cpu_model', 'unknown'),
            core_count=context.get('num_cpus', 0),
            memory_bytes=0,
            os_version=context.get('library_build_type', 'unknown'),
            compiler_version='unknown'
        )
        
        results = []
        for bench in data.get('benchmarks', []):
            name = bench.get('name', '')
            # 解析类别
            category = 'unknown'
            if 'Reader' in name or 'Writer' in name:
                category = 'io'
            elif 'Filter' in name:
                category = 'filter'
            elif 'Stat' in name:
                category = 'stat'
            
            results.append(BenchmarkResult(
                name=name,
                category=category,
                iterations=bench.get('iterations', 0),
                mean_time_ns=bench.get('real_time', 0.0) * 1e6,  # ms to ns
                std_dev_ns=0.0,
                throughput_mbps=bench.get('bytes_per_second', 0.0) / 1e6 if bench.get('bytes_per_second') else 0.0,
                throughput_reads_per_sec=bench.get('items_per_second', 0.0),
            ))
        
        return BenchmarkReport(metadata=metadata, results=results)
    
    def generate_markdown_report(self, report: BenchmarkReport, output_path: Optional[Path] = None) -> str:
        """生成 Markdown 格式报告"""
        if output_path is None:
            output_path = self.output_dir / "latest.md"
        
        lines = [
            "# FastQTools Performance Report",
            "",
            f"**Generated:** {report.metadata.timestamp}",
            f"**Git Commit:** {report.metadata.git_commit} ({report.metadata.git_branch})",
            "",
            "## System Information",
            "",
            f"| Property | Value |",
            f"|----------|-------|",
            f"| CPU | {report.metadata.cpu_model} |",
            f"| Cores | {report.metadata.core_count} |",
            f"| Memory | {self._format_bytes(report.metadata.memory_bytes)} |",
            f"| OS | {report.metadata.os_version} |",
            f"| Compiler | {report.metadata.compiler_version} |",
            "",
            "## Benchmark Results",
            "",
        ]
        
        # 按类别分组
        categories = {}
        for r in report.results:
            if r.category not in categories:
                categories[r.category] = []
            categories[r.category].append(r)
        
        for category, results in sorted(categories.items()):
            lines.append(f"### {category.upper()} Benchmarks")
            lines.append("")
            lines.append("| Benchmark | Time (ms) | Throughput | Iterations |")
            lines.append("|-----------|-----------|------------|------------|")
            
            for r in results:
                time_ms = r.mean_time_ns / 1e6
                throughput = f"{r.throughput_mbps:.2f} MB/s" if r.throughput_mbps > 0 else "-"
                lines.append(f"| {r.name} | {time_ms:.2f} | {throughput} | {r.iterations} |")
            
            lines.append("")
        
        content = "\n".join(lines)
        
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        return content
    
    def generate_summary_table(self, report: BenchmarkReport) -> str:
        """生成简洁的性能摘要表格（用于 README）"""
        lines = [
            "| Operation | Throughput | Time |",
            "|-----------|------------|------|",
        ]
        
        # 选择关键指标
        key_benchmarks = ['BM_FastQReader_Medium', 'BM_FastQWriter_Medium', 
                         'BM_Filter_Combined', 'BM_Stat_Full']
        
        for r in report.results:
            if any(key in r.name for key in key_benchmarks):
                time_ms = r.mean_time_ns / 1e6
                throughput = f"{r.throughput_mbps:.1f} MB/s" if r.throughput_mbps > 0 else "-"
                # 简化名称
                name = r.name.replace('BM_', '').replace('_', ' ')
                lines.append(f"| {name} | {throughput} | {time_ms:.1f} ms |")
        
        return "\n".join(lines)
    
    def generate_readme_section(self, report: BenchmarkReport) -> str:
        """生成可嵌入 README 的性能部分"""
        summary = self.generate_summary_table(report)
        
        section = f"""## Performance

Benchmark results on {report.metadata.cpu_model} ({report.metadata.core_count} cores):

{summary}

*Tested with 100K reads, 150bp read length. See [full benchmark report](docs/performance/benchmark-report.md) for details.*
"""
        return section
    
    def generate_throughput_chart(self, reports: List[BenchmarkReport], 
                                   output_path: Optional[Path] = None) -> Optional[Path]:
        """生成吞吐量趋势图"""
        if not HAS_MATPLOTLIB:
            print("Warning: matplotlib not available, skipping chart generation")
            return None
        
        if output_path is None:
            output_path = self.charts_dir / "throughput-trend.svg"
        
        # 提取数据
        dates = []
        throughputs = {}
        
        for report in reports:
            try:
                date = datetime.fromisoformat(report.metadata.timestamp.replace('Z', '+00:00'))
            except:
                continue
            
            dates.append(date)
            
            for r in report.results:
                if r.throughput_mbps > 0:
                    if r.name not in throughputs:
                        throughputs[r.name] = []
                    throughputs[r.name].append((date, r.throughput_mbps))
        
        if not dates or not throughputs:
            return None
        
        fig, ax = plt.subplots(figsize=(10, 6))
        
        for name, data in throughputs.items():
            if len(data) > 1:
                x = [d[0] for d in data]
                y = [d[1] for d in data]
                ax.plot(x, y, marker='o', label=name[:30])
        
        ax.set_xlabel('Date')
        ax.set_ylabel('Throughput (MB/s)')
        ax.set_title('FastQTools Performance Trend')
        ax.legend(loc='best', fontsize='small')
        ax.grid(True, alpha=0.3)
        
        fig.autofmt_xdate()
        plt.tight_layout()
        plt.savefig(output_path, format='svg', dpi=150)
        plt.close()
        
        return output_path
    
    def generate_comparison_chart(self, report: BenchmarkReport,
                                   output_path: Optional[Path] = None) -> Optional[Path]:
        """生成配置对比图"""
        if not HAS_MATPLOTLIB:
            return None
        
        if output_path is None:
            output_path = self.charts_dir / "comparison.svg"
        
        # 按类别分组
        categories = {}
        for r in report.results:
            if r.category not in categories:
                categories[r.category] = []
            categories[r.category].append(r)
        
        if not categories:
            return None
        
        fig, axes = plt.subplots(1, len(categories), figsize=(5 * len(categories), 5))
        if len(categories) == 1:
            axes = [axes]
        
        for ax, (category, results) in zip(axes, sorted(categories.items())):
            names = [r.name.split('/')[-1][:20] for r in results]
            times = [r.mean_time_ns / 1e6 for r in results]
            
            bars = ax.barh(names, times, color='steelblue')
            ax.set_xlabel('Time (ms)')
            ax.set_title(f'{category.upper()} Benchmarks')
            ax.grid(True, axis='x', alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(output_path, format='svg', dpi=150)
        plt.close()
        
        return output_path
    
    def update_readme(self, readme_path: Path, report: BenchmarkReport) -> bool:
        """更新 README 中的性能部分"""
        if not readme_path.exists():
            return False
        
        with open(readme_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 查找性能部分的标记
        start_marker = "<!-- BENCHMARK_START -->"
        end_marker = "<!-- BENCHMARK_END -->"
        
        new_section = f"""{start_marker}
{self.generate_readme_section(report)}
{end_marker}"""
        
        if start_marker in content and end_marker in content:
            # 替换现有部分
            import re
            pattern = f"{re.escape(start_marker)}.*?{re.escape(end_marker)}"
            content = re.sub(pattern, new_section, content, flags=re.DOTALL)
        else:
            # 在文件末尾添加
            content += f"\n\n{new_section}\n"
        
        with open(readme_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        return True
    
    def _format_bytes(self, bytes_val: int) -> str:
        """格式化字节数"""
        if bytes_val == 0:
            return "Unknown"
        for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
            if bytes_val < 1024:
                return f"{bytes_val:.1f} {unit}"
            bytes_val /= 1024
        return f"{bytes_val:.1f} PB"


def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="Generate benchmark reports")
    parser.add_argument("input", type=Path, nargs='?', help="Input JSON file")
    parser.add_argument("-o", "--output", type=Path, help="Output directory")
    parser.add_argument("--format", choices=['markdown', 'summary', 'readme'], 
                        default='markdown', help="Output format")
    parser.add_argument("--google-benchmark", action="store_true",
                        help="Input is Google Benchmark JSON format")
    parser.add_argument("--update-readme", type=Path, help="Update README file")
    parser.add_argument("--charts", action="store_true", help="Generate charts")
    
    args = parser.parse_args()
    
    output_dir = args.output or Path("benchmark_results/reports")
    generator = ReportGenerator(output_dir)
    
    if args.input:
        if args.google_benchmark:
            report = generator.load_google_benchmark_json(args.input)
        else:
            report = generator.load_result(args.input)
        
        if args.format == 'markdown':
            content = generator.generate_markdown_report(report)
            print(f"Generated report: {output_dir / 'latest.md'}")
        elif args.format == 'summary':
            print(generator.generate_summary_table(report))
        elif args.format == 'readme':
            print(generator.generate_readme_section(report))
        
        if args.charts and HAS_MATPLOTLIB:
            chart_path = generator.generate_comparison_chart(report)
            if chart_path:
                print(f"Generated chart: {chart_path}")
        
        if args.update_readme:
            if generator.update_readme(args.update_readme, report):
                print(f"Updated README: {args.update_readme}")
            else:
                print(f"Failed to update README: {args.update_readme}")
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
