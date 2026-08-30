#!/bin/bash
# tools/benchmark/scripts/run_baseline.sh
# 性能基准脚本
#
# 对 FastQTools 的 filter 命令进行基准测试，记录吞吐量和耗时。
# 结果输出到 stdout，可重定向保存。

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

FASTQTOOLS="${FASTQTOOLS:-$PROJECT_ROOT/build/clang-release/FastQTools}"
GENERATED_DIR="$PROJECT_ROOT/tests/data/generated"
DATAGEN="$PROJECT_ROOT/scripts/datagen/gen_fastq.py"
TMP_DIR=$(mktemp -d)
trap "rm -rf $TMP_DIR" EXIT

# 检查可执行文件
if [[ ! -x "$FASTQTOOLS" ]]; then
    echo "Error: FastQTools not found at $FASTQTOOLS"
    exit 1
fi

# 生成更大的测试数据（如果不存在；生成数据不入库，统一放 tests/data/generated）
BENCH_DATA="$GENERATED_DIR/sample_100k_len100.fastq"
if [[ ! -f "$BENCH_DATA" ]]; then
    echo "Generating benchmark data (100k reads, 100bp)..."
    mkdir -p "$GENERATED_DIR"
    python3 "$DATAGEN" -o "$BENCH_DATA" -n 100000 --min-len 100 --max-len 100 --seed 42
fi

FILE_SIZE=$(stat -c %s "$BENCH_DATA" 2>/dev/null || stat -f %z "$BENCH_DATA")
FILE_SIZE_MB=$(echo "scale=2; $FILE_SIZE / 1048576" | bc)

echo "=== FastQTools Performance Baseline ==="
echo "Date: $(date -Iseconds)"
echo "Executable: $FASTQTOOLS"
echo "Input: $BENCH_DATA"
echo "File size: ${FILE_SIZE_MB} MB"
echo ""

run_benchmark() {
    local name="$1"
    local threads="$2"
    local extra_args="$3"
    
    echo "--- Benchmark: $name (threads=$threads) ---"
    
    local start_time=$(date +%s.%N)
    $FASTQTOOLS -q filter \
        --input "$BENCH_DATA" \
        --output "$TMP_DIR/out.fastq" \
        --threads "$threads" \
        $extra_args 2>&1 || true
    local end_time=$(date +%s.%N)
    
    local elapsed=$(echo "$end_time - $start_time" | bc)
    local throughput=$(echo "scale=2; $FILE_SIZE_MB / $elapsed" | bc)
    
    echo "  Elapsed: ${elapsed}s"
    echo "  Throughput: ${throughput} MB/s"
    echo ""
}

# 基准场景
run_benchmark "baseline-1-thread" 1 ""
run_benchmark "baseline-4-threads" 4 ""
run_benchmark "with-min-quality-1-thread" 1 "--min-quality 30"
run_benchmark "with-min-quality-4-threads" 4 "--min-quality 30"
run_benchmark "with-filters-4-threads" 4 "--min-quality 30 --min-length 80 --max-n-ratio 0.1"

echo "=== Benchmark Complete ==="
