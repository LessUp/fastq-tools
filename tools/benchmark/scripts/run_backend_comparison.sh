#!/bin/bash

set -euo pipefail

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <benchmark_pipeline> [output_dir]" >&2
    exit 2
fi

BENCHMARK_BIN="$1"
OUTPUT_DIR="${2:-build/benchmark-results/backends}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

mkdir -p "$OUTPUT_DIR"

CASES=(
    "BM_Backend_SequentialCpu/100000/1/real_time"
    "BM_Backend_OneTbbCpu/100000/2/real_time"
    "BM_Backend_OneTbbCpu/100000/4/real_time"
    "BM_Backend_OneTbbCpu/100000/8/real_time"
    "BM_Backend_TaskflowCpu/100000/2/real_time"
    "BM_Backend_TaskflowCpu/100000/4/real_time"
    "BM_Backend_TaskflowCpu/100000/8/real_time"
    "BM_Backend_SequentialReadWrite/100000/1/real_time"
    "BM_Backend_OneTbbReadWrite/100000/2/real_time"
    "BM_Backend_OneTbbReadWrite/100000/4/real_time"
    "BM_Backend_OneTbbReadWrite/100000/8/real_time"
    "BM_Backend_TaskflowReadWrite/100000/2/real_time"
    "BM_Backend_TaskflowReadWrite/100000/4/real_time"
    "BM_Backend_TaskflowReadWrite/100000/8/real_time"
)

for benchmark_case in "${CASES[@]}"; do
    file_stem="${benchmark_case//\//_}"
    echo "Running ${benchmark_case}"
    /usr/bin/time -f '%M' -o "${OUTPUT_DIR}/${file_stem}.rss_kb" \
        "$BENCHMARK_BIN" \
        --benchmark_filter="^${benchmark_case}$" \
        --benchmark_min_time=0.05s \
        --benchmark_repetitions=7 \
        --benchmark_out="${OUTPUT_DIR}/${file_stem}.json" \
        --benchmark_out_format=json \
        >/dev/null
done

python3 "${SCRIPT_DIR}/summarize_backend_comparison.py" "$OUTPUT_DIR"
