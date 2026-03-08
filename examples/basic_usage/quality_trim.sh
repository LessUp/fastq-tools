#!/bin/bash

# FastQTools 质量修剪示例
# 演示如何对 FastQ 文件进行质量修剪（trim），去除低质量末端碱基

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
    echo "FastQTools 质量修剪示例"
    echo ""
    echo "用法: $0 <input.fastq[.gz]> [output.fastq[.gz]]"
    echo ""
    echo "参数:"
    echo "  input.fastq[.gz]     输入的 FastQ 文件（支持压缩格式）"
    echo "  output.fastq[.gz]    输出文件路径（可选，默认写入 results/ 目录）"
    echo ""
    echo "修剪选项（通过环境变量设置）:"
    echo "  FASTQTOOLS_TRIM_QUALITY  修剪质量阈值（默认: 20）"
    echo "  FASTQTOOLS_TRIM_MODE     修剪模式: both / five / three（默认: both）"
    echo "  FASTQTOOLS_MIN_LENGTH    修剪后最短保留读长（默认: 30）"
    echo ""
    echo "通用选项（通过环境变量设置）:"
    echo "  FASTQTOOLS_THREADS       线程数（默认: 4）"
    echo "  FASTQTOOLS_BATCH_SIZE    批处理大小（默认: 10000）"
    echo ""
    echo "修剪模式说明:"
    echo "  both   — 从 5' 和 3' 两端修剪低质量碱基"
    echo "  five   — 仅从 5' 端修剪"
    echo "  three  — 仅从 3' 端修剪"
    echo ""
    echo "示例:"
    echo "  $0 sample.fastq.gz"
    echo "  FASTQTOOLS_TRIM_QUALITY=25 $0 data.fq trimmed.fq"
    echo "  FASTQTOOLS_TRIM_MODE=three FASTQTOOLS_MIN_LENGTH=50 $0 input.fq.gz"
}

# 检查参数
if [[ $# -lt 1 ]]; then
    print_usage
    exit 1
fi

INPUT_FILE="$1"
BASENAME=$(basename "$INPUT_FILE" | sed 's/\.[^.]*$//' | sed 's/\.[^.]*$//')
DEFAULT_OUTPUT="$OUTPUT_DIR/${BASENAME}.trimmed.fastq"
OUTPUT_FILE="${2:-$DEFAULT_OUTPUT}"

# 修剪参数
TRIM_QUALITY=${FASTQTOOLS_TRIM_QUALITY:-20}
TRIM_MODE=${FASTQTOOLS_TRIM_MODE:-both}
MIN_LENGTH=${FASTQTOOLS_MIN_LENGTH:-30}

# 验证修剪模式
if [[ "$TRIM_MODE" != "both" && "$TRIM_MODE" != "five" && "$TRIM_MODE" != "three" ]]; then
    echo -e "${RED}错误: 无效的修剪模式 '$TRIM_MODE'，请使用 both / five / three${NC}"
    exit 1
fi

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

echo -e "${BLUE}FastQTools 质量修剪${NC}"
echo -e "${BLUE}====================${NC}"
echo ""
echo -e "${GREEN}输入文件:${NC}       $INPUT_FILE"
echo -e "${GREEN}输出文件:${NC}       $OUTPUT_FILE"
echo -e "${GREEN}线程数:${NC}         $THREADS"
echo -e "${GREEN}修剪质量阈值:${NC}   $TRIM_QUALITY"
echo -e "${GREEN}修剪模式:${NC}       $TRIM_MODE"
echo -e "${GREEN}最短保留读长:${NC}   $MIN_LENGTH"
echo ""

# 执行修剪
echo -e "${BLUE}开始质量修剪...${NC}"

start_time=$(date +%s)

if "$FASTQTOOLS_BIN" filter \
    -i "$INPUT_FILE" \
    -o "$OUTPUT_FILE" \
    --threads "$THREADS" \
    --batch-size "$BATCH_SIZE" \
    --trim-quality "$TRIM_QUALITY" \
    --trim-mode "$TRIM_MODE" \
    --min-length "$MIN_LENGTH"; then

    end_time=$(date +%s)
    duration=$((end_time - start_time))

    echo ""
    echo -e "${GREEN}✅ 质量修剪完成！${NC}"
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
    echo -e "${RED}❌ 质量修剪失败${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}🎉 修剪完成！${NC}"
echo ""
echo -e "${YELLOW}下一步建议:${NC}"
echo "1. 对修剪后文件统计: ./basic_stats.sh $OUTPUT_FILE"
echo "2. 进一步过滤: ./basic_filter.sh $OUTPUT_FILE"
echo "3. 完整工作流: ./pipeline_demo.sh $INPUT_FILE"
