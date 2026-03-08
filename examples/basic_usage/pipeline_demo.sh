#!/bin/bash

# FastQTools 完整工作流示例
# 演示典型的 FASTQ 数据质控流程：统计 → 修剪 → 过滤 → 再统计

set -euo pipefail

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 默认参数
THREADS=${FASTQTOOLS_THREADS:-4}
BATCH_SIZE=${FASTQTOOLS_BATCH_SIZE:-10000}
OUTPUT_DIR="./results"

# 质控参数
TRIM_QUALITY=${FASTQTOOLS_TRIM_QUALITY:-20}
TRIM_MODE=${FASTQTOOLS_TRIM_MODE:-both}
MIN_QUALITY=${FASTQTOOLS_MIN_QUALITY:-20}
MIN_LENGTH=${FASTQTOOLS_MIN_LENGTH:-50}
MAX_N_RATIO=${FASTQTOOLS_MAX_N_RATIO:-0.1}

print_usage() {
    echo "FastQTools 完整工作流示例"
    echo ""
    echo "用法: $0 <input.fastq[.gz]> [output_prefix]"
    echo ""
    echo "参数:"
    echo "  input.fastq[.gz]    输入的 FastQ 文件（支持压缩格式）"
    echo "  output_prefix       输出文件前缀（可选，默认使用输入文件名）"
    echo ""
    echo "工作流步骤:"
    echo "  1. 原始数据统计（stat）"
    echo "  2. 质量修剪（trim）"
    echo "  3. 读段过滤（filter）"
    echo "  4. 清洁数据统计（stat）"
    echo ""
    echo "质控参数（通过环境变量设置）:"
    echo "  FASTQTOOLS_TRIM_QUALITY  修剪质量阈值（默认: 20）"
    echo "  FASTQTOOLS_TRIM_MODE     修剪模式: both/five/three（默认: both）"
    echo "  FASTQTOOLS_MIN_QUALITY   最低平均质量（默认: 20）"
    echo "  FASTQTOOLS_MIN_LENGTH    最短读长（默认: 50）"
    echo "  FASTQTOOLS_MAX_N_RATIO   最大 N 碱基比例（默认: 0.1）"
    echo ""
    echo "通用选项:"
    echo "  FASTQTOOLS_THREADS       线程数（默认: 4）"
    echo "  FASTQTOOLS_BATCH_SIZE    批处理大小（默认: 10000）"
    echo ""
    echo "示例:"
    echo "  $0 sample.fastq.gz"
    echo "  $0 data.fq my_sample"
    echo "  FASTQTOOLS_TRIM_QUALITY=25 FASTQTOOLS_MIN_QUALITY=25 $0 input.fq.gz"
}

# 检查参数
if [[ $# -lt 1 ]]; then
    print_usage
    exit 1
fi

INPUT_FILE="$1"
PREFIX="${2:-$(basename "$INPUT_FILE" | sed 's/\.[^.]*$//' | sed 's/\.[^.]*$//')}"

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
mkdir -p "$OUTPUT_DIR"

# 中间与最终文件路径
RAW_STAT="$OUTPUT_DIR/${PREFIX}.raw.stat.txt"
TRIMMED_FILE="$OUTPUT_DIR/${PREFIX}.trimmed.fastq"
CLEAN_FILE="$OUTPUT_DIR/${PREFIX}.clean.fastq"
CLEAN_STAT="$OUTPUT_DIR/${PREFIX}.clean.stat.txt"

echo -e "${CYAN}╔══════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║    FastQTools 完整质控工作流              ║${NC}"
echo -e "${CYAN}╚══════════════════════════════════════════╝${NC}"
echo ""
echo -e "${GREEN}输入文件:${NC}       $INPUT_FILE"
echo -e "${GREEN}输出前缀:${NC}       $PREFIX"
echo -e "${GREEN}线程数:${NC}         $THREADS"
echo ""
echo -e "${GREEN}质控参数:${NC}"
echo "  修剪质量阈值:   $TRIM_QUALITY"
echo "  修剪模式:       $TRIM_MODE"
echo "  最低平均质量:   $MIN_QUALITY"
echo "  最短读长:       $MIN_LENGTH"
echo "  最大 N 比例:    $MAX_N_RATIO"
echo ""

total_start=$(date +%s)

# ──────────────────────────────────────────
# 步骤 1: 原始数据统计
# ──────────────────────────────────────────
echo -e "${BLUE}[1/4] 原始数据统计...${NC}"

if "$FASTQTOOLS_BIN" stat \
    -i "$INPUT_FILE" \
    -o "$RAW_STAT" \
    --threads "$THREADS" \
    --batch-size "$BATCH_SIZE"; then
    echo -e "${GREEN}  ✅ 原始数据统计完成 → $RAW_STAT${NC}"
else
    echo -e "${RED}  ❌ 原始数据统计失败${NC}"
    exit 1
fi

echo ""

# ──────────────────────────────────────────
# 步骤 2: 质量修剪
# ──────────────────────────────────────────
echo -e "${BLUE}[2/4] 质量修剪（trim-quality=$TRIM_QUALITY, mode=$TRIM_MODE）...${NC}"

if "$FASTQTOOLS_BIN" filter \
    -i "$INPUT_FILE" \
    -o "$TRIMMED_FILE" \
    --threads "$THREADS" \
    --batch-size "$BATCH_SIZE" \
    --trim-quality "$TRIM_QUALITY" \
    --trim-mode "$TRIM_MODE"; then
    echo -e "${GREEN}  ✅ 质量修剪完成 → $TRIMMED_FILE${NC}"
else
    echo -e "${RED}  ❌ 质量修剪失败${NC}"
    exit 1
fi

echo ""

# ──────────────────────────────────────────
# 步骤 3: 读段过滤
# ──────────────────────────────────────────
echo -e "${BLUE}[3/4] 读段过滤（min-quality=$MIN_QUALITY, min-length=$MIN_LENGTH, max-n-ratio=$MAX_N_RATIO）...${NC}"

if "$FASTQTOOLS_BIN" filter \
    -i "$TRIMMED_FILE" \
    -o "$CLEAN_FILE" \
    --threads "$THREADS" \
    --batch-size "$BATCH_SIZE" \
    --min-quality "$MIN_QUALITY" \
    --min-length "$MIN_LENGTH" \
    --max-n-ratio "$MAX_N_RATIO"; then
    echo -e "${GREEN}  ✅ 读段过滤完成 → $CLEAN_FILE${NC}"
else
    echo -e "${RED}  ❌ 读段过滤失败${NC}"
    exit 1
fi

echo ""

# ──────────────────────────────────────────
# 步骤 4: 清洁数据统计
# ──────────────────────────────────────────
echo -e "${BLUE}[4/4] 清洁数据统计...${NC}"

if "$FASTQTOOLS_BIN" stat \
    -i "$CLEAN_FILE" \
    -o "$CLEAN_STAT" \
    --threads "$THREADS" \
    --batch-size "$BATCH_SIZE"; then
    echo -e "${GREEN}  ✅ 清洁数据统计完成 → $CLEAN_STAT${NC}"
else
    echo -e "${RED}  ❌ 清洁数据统计失败${NC}"
    exit 1
fi

total_end=$(date +%s)
total_duration=$((total_end - total_start))

echo ""
echo -e "${CYAN}══════════════════════════════════════════${NC}"
echo -e "${GREEN}🎉 完整工作流完成！总耗时: ${total_duration} 秒${NC}"
echo -e "${CYAN}══════════════════════════════════════════${NC}"
echo ""

# 文件大小汇总
echo -e "${BLUE}文件大小汇总:${NC}"
IN_SIZE=$(du -h "$INPUT_FILE" | cut -f1)
echo "  原始文件:     $IN_SIZE  ($INPUT_FILE)"
if [[ -f "$TRIMMED_FILE" ]]; then
    TRIM_SIZE=$(du -h "$TRIMMED_FILE" | cut -f1)
    echo "  修剪后:       $TRIM_SIZE  ($TRIMMED_FILE)"
fi
if [[ -f "$CLEAN_FILE" ]]; then
    CLEAN_SIZE=$(du -h "$CLEAN_FILE" | cut -f1)
    echo "  过滤后:       $CLEAN_SIZE  ($CLEAN_FILE)"
fi

echo ""
echo -e "${BLUE}生成的文件:${NC}"
echo "  原始统计:     $RAW_STAT"
echo "  修剪中间文件: $TRIMMED_FILE"
echo "  清洁数据:     $CLEAN_FILE"
echo "  清洁统计:     $CLEAN_STAT"

echo ""
echo -e "${YELLOW}下一步建议:${NC}"
echo "1. 对比前后统计: diff $RAW_STAT $CLEAN_STAT"
echo "2. 清理中间文件: rm $TRIMMED_FILE"
echo "3. 查看详细文档: ../../docs/guide/getting-started.md"
