#!/usr/bin/env python3
"""
性能徽章生成器

生成 shields.io 兼容的性能徽章。
"""

import json
import sys
from pathlib import Path
from typing import Optional
from urllib.parse import quote

from report_generator import ReportGenerator


def generate_badge_url(label: str, message: str, color: str = "blue") -> str:
    """生成 shields.io 徽章 URL"""
    label_encoded = quote(label, safe='')
    message_encoded = quote(message, safe='')
    return f"https://img.shields.io/badge/{label_encoded}-{message_encoded}-{color}"


def generate_badge_markdown(label: str, message: str, color: str = "blue", 
                            link: Optional[str] = None) -> str:
    """生成 Markdown 格式的徽章"""
    url = generate_badge_url(label, message, color)
    if link:
        return f"[![{label}]({url})]({link})"
    return f"![{label}]({url})"


def generate_performance_badge(throughput_mbps: float) -> str:
    """根据吞吐量生成性能徽章"""
    if throughput_mbps >= 500:
        color = "brightgreen"
        rating = "excellent"
    elif throughput_mbps >= 200:
        color = "green"
        rating = "good"
    elif throughput_mbps >= 100:
        color = "yellow"
        rating = "fair"
    else:
        color = "red"
        rating = "needs improvement"
    
    message = f"{throughput_mbps:.0f} MB/s ({rating})"
    return generate_badge_markdown("performance", message, color, 
                                   "docs/performance/benchmark-report.md")


def generate_badge_json(label: str, message: str, color: str = "blue") -> dict:
    """生成 shields.io endpoint JSON 格式"""
    return {
        "schemaVersion": 1,
        "label": label,
        "message": message,
        "color": color
    }


def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="Generate performance badges")
    parser.add_argument("input", type=Path, nargs='?', help="Input JSON file")
    parser.add_argument("--output", "-o", type=Path, help="Output badge JSON file")
    parser.add_argument("--format", choices=['url', 'markdown', 'json'], 
                        default='markdown', help="Output format")
    parser.add_argument("--google-benchmark", action="store_true",
                        help="Input is Google Benchmark JSON format")
    
    args = parser.parse_args()
    
    if not args.input:
        # 尝试找到最新的结果
        results_dir = Path("benchmark_results/results")
        if results_dir.exists():
            files = sorted(results_dir.glob("*.json"), reverse=True)
            if files:
                args.input = files[0]
    
    if not args.input or not args.input.exists():
        print("Error: No benchmark results found", file=sys.stderr)
        sys.exit(1)
    
    generator = ReportGenerator()
    
    if args.google_benchmark:
        report = generator.load_google_benchmark_json(args.input)
    else:
        report = generator.load_result(args.input)
    
    # 计算平均吞吐量
    throughputs = [r.throughput_mbps for r in report.results if r.throughput_mbps > 0]
    avg_throughput = sum(throughputs) / len(throughputs) if throughputs else 0
    
    if args.format == 'json':
        # 生成 shields.io endpoint JSON
        if avg_throughput >= 500:
            color = "brightgreen"
        elif avg_throughput >= 200:
            color = "green"
        elif avg_throughput >= 100:
            color = "yellow"
        else:
            color = "red"
        
        badge = generate_badge_json("performance", f"{avg_throughput:.0f} MB/s", color)
        
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(badge, f, indent=2)
            print(f"Badge JSON saved to: {args.output}")
        else:
            print(json.dumps(badge, indent=2))
    
    elif args.format == 'url':
        print(generate_badge_url("performance", f"{avg_throughput:.0f} MB/s"))
    
    else:  # markdown
        print(generate_performance_badge(avg_throughput))


if __name__ == "__main__":
    main()
