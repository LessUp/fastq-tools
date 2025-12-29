#!/usr/bin/env python3
"""
生成合成 FASTQ 数据，用于基线测试与回归基准。

默认生成 10_000 条 reads，读长分布在 80~150bp，质量分数偏移为 33。
可通过参数自定义条数、读长、输出文件等。
"""

import argparse
import random
import string
from pathlib import Path
from typing import Tuple

BASES = ["A", "C", "G", "T", "N"]


def random_read(seq_len: int) -> Tuple[str, str]:
    seq = "".join(random.choices(BASES, k=seq_len))
    # 让质量分布稍微分层：大部分在高质量，少量低质量噪声
    qualities = []
    for _ in range(seq_len):
        q = 38 if random.random() > 0.1 else 25  # 高质量占 90%
        qualities.append(chr(q + 33))
    return seq, "".join(qualities)


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate synthetic FASTQ data.")
    parser.add_argument("-o", "--output", type=Path, default=Path("sample_10k.fastq"))
    parser.add_argument("-n", "--reads", type=int, default=10_000)
    parser.add_argument("--min-len", type=int, default=80)
    parser.add_argument("--max-len", type=int, default=150)
    parser.add_argument("--seed", type=int, default=42)
    args = parser.parse_args()

    random.seed(args.seed)

    with args.output.open("w", encoding="ascii") as f:
        for idx in range(args.reads):
            seq_len = random.randint(args.min_len, args.max_len)
            seq, qual = random_read(seq_len)
            name = f"@read_{idx}"
            f.write(f"{name}\n{seq}\n+\n{qual}\n")

    print(f"Generated {args.reads} reads to {args.output}")


if __name__ == "__main__":
    main()
