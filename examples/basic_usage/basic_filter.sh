#!/bin/bash

# FastQTools 基本过滤示例
# 演示如何对 FastQ 文件进行质量过滤（按最低平均质量、读长范围、N 碱基比例）

set -euo pipefail

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 默认参数
THREADS=${FASTQTOOLS_THREADS:-4}
BATCH_SIZE=${FASTQTOOLS_BATCH_SIZE:-10000}
OUTPUT_DIR="./results"

print_usage() {
    echo "FastQTools 基本过滤示例"
    echo ""
    echo "用法: $0 <input.fastq[.gz]> [output.fastq[.gz]]"
    echo ""
    echo "参数:"
    echo "  input.fastq[.gz]     输入的 FastQ 文件（支持压缩格式）"
    echo "  output.fastq[.gz]    输出文件路径（可选，默认写入 results/ 目录）"
    echo ""
    echo "过滤选项（通过环境变量设置）:"
    echo "  FASTQTOOLS_MIN_QUALITY   最低平均质量（默认: 20）"
    echo "  FASTQTOOLS_MIN_LENGTH    最短读长（默认: 50）"
    echo "  FASTQTOOLS_MAX_LENGTH    最长读长（默认: 不限）"
    echo "  FASTQTOOLS_MAX_N_RATIO   最大 N 碱基比例（默认: 0.1）"
    echo ""
    echo "通用选项（通过环境变量设置）:"
    echo "  FASTQTOOLS_THREADS       线程数（默认: 4）"
    echo "  FASTQTOOLS_BATCH_SIZE    批处理大小（默认: 10000）"
    echo ""
    echo "示例:"
    echo "  $0 sample.fastq.gz"
    echo "  FASTQTOOLS_MIN_QUALITY=30 $0 data.fq filtered.fq"
    echo "  FASTQTOOLS_MIN_LENGTH=100 FASTQTOOLS_MAX_N_RATIO=0.05 $0 input.fq.gz"
}

# 检查参数
if [[ $# -lt 1 ]]; then
    print_usage
    exit 1
fi

INPUT_FILE="$1"
BASENAME=$(basename "$INPUT_FILE" | sed 's/\.[^.]*$//' | sed 's/\.[^.]*$//')
DEFAULT_OUTPUT="$OUTPUT_DIR/${BASENAME}.filtered.fastq"
OUTPUT_FILE="${2:-$DEFAULT_OUTPUT}"

# 过滤参数
MIN_QUALITY=${FASTQTOOLS_MIN_QUALITY:-20}
MIN_LENGTH=${FASTQTOOLS_MIN_LENGTH:-50}
MAX_LENGTH=${FASTQTOOLS_MAX_LENGTH:-}
MAX_N_RATIO=${FASTQTOOLS_MAX_N_RATIO:-0.1}

# 检查输入文件
if [[ ! -f "$INPUT_FILE" ]]; then
    echo -e "${RED}错误: 输入文件不存在: $INPUT_FILE${NC}"
    exit 1
fi

# 检查 FastQTools 是否可用
FASTQTOOLS_BIN="${FASTQTOOLS:-}"
if [[ -z "${FASTQTOOLS_BIN}" ]]; then
    if command -v FastQTools &> /dev/null; then
        FASTQTOOLS_BIN="FastQTools"
    else
        for candidate in \
            "../../build/gcc-release/FastQTools" \
            "../../build/clang-release/FastQTools" \
            "../../build/gcc-debug/FastQTools" \
            "../../build/clang-debug/FastQTools"; do
            if [[ -x "$candidate" ]]; then
                FASTQTOOLS_BIN="$candidate"
                break
            fi
        done
    fi
fi

if [[ -z "${FASTQTOOLS_BIN}" ]]; then
    echo -e "${RED}错误: FastQTools 未找到，请先构建（../../scripts/core/build）或设置 FASTQTOOLS=/path/to/FastQTools${NC}"
    exit 1
fi

# 创建输出目录
mkdir -p "$(dirname "$OUTPUT_FILE")"

echo -e "${BLUE}FastQTools 基本过滤${NC}"
echo -e "${BLUE}====================${NC}"
echo ""
echo -e "${GREEN}输入文件:${NC}       $INPUT_FILE"
echo -e "${GREEN}输出文件:${NC}       $OUTPUT_FILE"
echo -e "${GREEN}线程数:${NC}         $THREADS"
echo -e "${GREEN}最低平均质量:${NC}   $MIN_QUALITY"
echo -e "${GREEN}最短读长:${NC}       $MIN_LENGTH"
if [[ -n "$MAX_LENGTH" ]]; then
    echo -e "${GREEN}最长读长:${NC}       $MAX_LENGTH"
fi
echo -e "${GREEN}最大 N 比例:${NC}    $MAX_N_RATIO"
echo ""

# 构建过滤参数
FILTER_ARGS=(
    -i "$INPUT_FILE"
    -o "$OUTPUT_FILE"
    --threads "$THREADS"
    --batch-size "$BATCH_SIZE"
    --min-quality "$MIN_QUALITY"
    --min-length "$MIN_LENGTH"
    --max-n-ratio "$MAX_N_RATIO"
)

if [[ -n "$MAX_LENGTH" ]]; then
    FILTER_ARGS+=(--max-length "$MAX_LENGTH")
fi

# 执行过滤
echo -e "${BLUE}开始过滤...${NC}"

start_time=$(date +%s)

if "$FASTQTOOLS_BIN" filter "${FILTER_ARGS[@]}"; then

    end_time=$(date +%s)
    duration=$((end_time - start_time))

    echo ""
    echo -e "${GREEN}✅ 过滤完成！${NC}"
    echo -e "${GREEN}耗时: ${duration} 秒${NC}"
    echo -e "${GREEN}输出文件: $OUTPUT_FILE${NC}"

    # 显示输出文件大小
    if [[ -f "$OUTPUT_FILE" ]]; then
        OUT_SIZE=$(du -h "$OUTPUT_FILE" | cut -f1)
        IN_SIZE=$(du -h "$INPUT_FILE" | cut -f1)
        echo ""
        echo -e "${BLUE}文件大小对比:${NC}"
        echo "  输入: $IN_SIZE"
        echo "  输出: $OUT_SIZE"
    fi
else
    echo -e "${RED}❌ 过滤失败${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}🎉 过滤完成！${NC}"
echo ""
echo -e "${YELLOW}下一步建议:${NC}"
echo "1. 查看输出文件: head -8 $OUTPUT_FILE"
echo "2. 对输出文件统计: ./basic_stats.sh $OUTPUT_FILE"
echo "3. 尝试质量修剪: ./quality_trim.sh $INPUT_FILE"
