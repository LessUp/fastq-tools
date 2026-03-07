#!/usr/bin/env python3
"""
生成 Benchmark 测试数据

支持生成不同大小、不同质量分布的 FASTQ 文件，用于性能基准测试。
"""

import argparse
import gzip
import random
import sys
from pathlib import Path
from typing import Tuple, Optional

BASES = ["A", "C", "G", "T"]
BASES_WITH_N = ["A", "C", "G", "T", "N"]


def random_read(
    seq_len: int,
    n_ratio: float = 0.0,
    quality_mean: int = 30,
    quality_std: int = 5
) -> Tuple[str, str]:
    """生成随机 read 序列和质量分数"""
    bases = BASES_WITH_N if n_ratio > 0 else BASES
    weights = None
    
    if n_ratio > 0:
        # 设置 N 的权重
        n_weight = n_ratio
        other_weight = (1 - n_ratio) / 4
        weights = [other_weight, other_weight, other_weight, other_weight, n_weight]
    
    seq = "".join(random.choices(bases, weights=weights, k=seq_len))
    
    # 生成质量分数（正态分布，限制在 0-41 范围内）
    qualities = []
    for _ in range(seq_len):
        q = int(random.gauss(quality_mean, quality_std))
        q = max(0, min(41, q))  # 限制范围
        qualities.append(chr(q + 33))
    
    return seq, "".join(qualities)


def generate_fastq(
    output_path: Path,
    num_reads: int,
    min_len: int = 100,
    max_len: int = 150,
    n_ratio: float = 0.0,
    quality_mean: int = 30,
    quality_std: int = 5,
    seed: Optional[int] = None,
    compress: bool = False
) -> None:
    """生成 FASTQ 文件"""
    if seed is not None:
        random.seed(seed)
    
    # 确定输出路径
    if compress and not str(output_path).endswith('.gz'):
        output_path = Path(str(output_path) + '.gz')
    
    # 选择打开方式
    open_func = gzip.open if compress else open
    mode = 'wt' if compress else 'w'
    
    with open_func(output_path, mode, encoding='ascii') as f:
        for idx in range(num_reads):
            seq_len = random.randint(min_len, max_len)
            seq, qual = random_read(seq_len, n_ratio, quality_mean, quality_std)
            
            f.write(f"@read_{idx}\n")
            f.write(f"{seq}\n")
            f.write("+\n")
            f.write(f"{qual}\n")
    
    print(f"Generated {num_reads} reads to {output_path}")


def generate_benchmark_dataset(output_dir: Path, seed: int = 42) -> None:
    """生成完整的 benchmark 数据集"""
    output_dir.mkdir(parents=True, exist_ok=True)
    
    datasets = [
        # (name, reads, min_len, max_len, n_ratio, quality_mean, compress)
        ("small_10k", 10000, 100, 150, 0.0, 30, False),
        ("small_10k_compressed", 10000, 100, 150, 0.0, 30, True),
        ("medium_100k", 100000, 100, 150, 0.0, 30, False),
        ("medium_100k_compressed", 100000, 100, 150, 0.0, 30, True),
        ("large_1m", 1000000, 100, 150, 0.0, 30, False),
        ("with_n_10k", 10000, 100, 150, 0.05, 30, False),
        ("low_quality_10k", 10000, 100, 150, 0.0, 20, False),
        ("high_quality_10k", 10000, 100, 150, 0.0, 38, False),
        ("variable_length_10k", 10000, 50, 300, 0.0, 30, False),
    ]
    
    for name, reads, min_len, max_len, n_ratio, quality_mean, compress in datasets:
        ext = ".fastq.gz" if compress else ".fastq"
        output_path = output_dir / f"{name}{ext}"
        
        if output_path.exists():
            print(f"Skipping {output_path} (already exists)")
            continue
        
        generate_fastq(
            output_path=output_path,
            num_reads=reads,
            min_len=min_len,
            max_len=max_len,
            n_ratio=n_ratio,
            quality_mean=quality_mean,
            seed=seed,
            compress=compress
        )


def validate_fastq(filepath: Path) -> Tuple[bool, str]:
    """验证 FASTQ 文件格式"""
    try:
        open_func = gzip.open if str(filepath).endswith('.gz') else open
        
        with open_func(filepath, 'rt', encoding='ascii') as f:
            line_num = 0
            record_num = 0
            
            while True:
                # 读取 4 行作为一个 record
                header = f.readline()
                if not header:
                    break  # EOF
                
                seq = f.readline()
                plus = f.readline()
                qual = f.readline()
                
                line_num += 4
                record_num += 1
                
                # 验证 header
                if not header.startswith('@'):
                    return False, f"Line {line_num-3}: Header must start with '@'"
                
                # 验证 plus line
                if not plus.startswith('+'):
                    return False, f"Line {line_num-1}: Plus line must start with '+'"
                
                # 验证序列和质量长度匹配
                seq = seq.strip()
                qual = qual.strip()
                if len(seq) != len(qual):
                    return False, f"Record {record_num}: Sequence and quality lengths don't match"
                
                # 验证序列字符
                valid_bases = set('ATGCNatgcn')
                for i, c in enumerate(seq):
                    if c not in valid_bases:
                        return False, f"Record {record_num}: Invalid base '{c}' at position {i}"
        
        return True, f"Valid FASTQ file with {record_num} records"
    
    except Exception as e:
        return False, f"Error reading file: {str(e)}"


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate FASTQ benchmark data",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Generate a single file
  %(prog)s -o test.fastq -n 10000
  
  # Generate compressed file
  %(prog)s -o test.fastq.gz -n 10000 --compress
  
  # Generate with custom parameters
  %(prog)s -o test.fastq -n 50000 --min-len 80 --max-len 200 --n-ratio 0.05
  
  # Generate complete benchmark dataset
  %(prog)s --generate-dataset --output-dir benchmark_results/data
  
  # Validate a FASTQ file
  %(prog)s --validate test.fastq
"""
    )
    
    parser.add_argument("-o", "--output", type=Path, help="Output file path")
    parser.add_argument("-n", "--reads", type=int, default=10000, help="Number of reads")
    parser.add_argument("--min-len", type=int, default=100, help="Minimum read length")
    parser.add_argument("--max-len", type=int, default=150, help="Maximum read length")
    parser.add_argument("--n-ratio", type=float, default=0.0, help="Ratio of N bases (0.0-1.0)")
    parser.add_argument("--quality-mean", type=int, default=30, help="Mean quality score")
    parser.add_argument("--quality-std", type=int, default=5, help="Quality score std dev")
    parser.add_argument("--seed", type=int, default=42, help="Random seed")
    parser.add_argument("--compress", action="store_true", help="Compress output with gzip")
    
    parser.add_argument("--generate-dataset", action="store_true", 
                        help="Generate complete benchmark dataset")
    parser.add_argument("--output-dir", type=Path, default=Path("benchmark_results/data"),
                        help="Output directory for dataset")
    
    parser.add_argument("--validate", type=Path, help="Validate a FASTQ file")
    
    args = parser.parse_args()
    
    if args.validate:
        valid, message = validate_fastq(args.validate)
        print(message)
        sys.exit(0 if valid else 1)
    
    if args.generate_dataset:
        generate_benchmark_dataset(args.output_dir, args.seed)
        return
    
    if not args.output:
        parser.error("--output is required when not using --generate-dataset or --validate")
    
    generate_fastq(
        output_path=args.output,
        num_reads=args.reads,
        min_len=args.min_len,
        max_len=args.max_len,
        n_ratio=args.n_ratio,
        quality_mean=args.quality_mean,
        quality_std=args.quality_std,
        seed=args.seed,
        compress=args.compress
    )


if __name__ == "__main__":
    main()
