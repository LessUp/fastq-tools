#!/usr/bin/env python3
"""
生成合成 FASTQ 数据，用于基线测试与回归基准。

默认生成 10_000 条 reads，读长分布在 80~150bp，质量分数偏移为 33。
可通过参数自定义条数、读长、输出文件等。

数据特征：
- 碱基仅 ACGT（无 N），避免触发 filter 的 max-n-ratio 过滤
- 质量统一 Q38（高可靠），确保 filter 默认全保留
- 这样生成的样本可作为 e2e/基准测试的"理想输入"
"""

import argparse
import random
from pathlib import Path
from typing import Tuple

BASES = ["A", "C", "G", "T"]


def random_read(seq_len: int) -> Tuple[str, str]:
    seq = "".join(random.choices(BASES, k=seq_len))
    # 统一高质量，避免被默认质量过滤剔除
    qual = "".join(chr(38 + 33) for _ in range(seq_len))
    return seq, qual


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate synthetic FASTQ data.")
    parser.add_argument("-o", "--output", type=Path, default=Path("sample_10k.fastq"))
    parser.add_argument("-n", "--reads", type=int, default=10_000)
    parser.add_argument("--min-len", type=int, default=80)
    parser.add_argument("--max-len", type=int, default=150)
    parser.add_argument("--seed", type=int, default=42)
    args = parser.parse_args()

    random.seed(args.seed)

    # 输出目录可能不存在（如 tests/data/generated 不入库），自动创建
    args.output.parent.mkdir(parents=True, exist_ok=True)

    with args.output.open("w", encoding="ascii") as f:
        for idx in range(args.reads):
            seq_len = random.randint(args.min_len, args.max_len)
            seq, qual = random_read(seq_len)
            name = f"@read_{idx}"
            f.write(f"{name}\n{seq}\n+\n{qual}\n")

    print(f"Generated {args.reads} reads to {args.output}")


if __name__ == "__main__":
    main()
