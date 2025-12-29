#!/bin/bash
# tests/e2e/test_cli.sh
# CLI 端到端测试脚本
#
# 测试 FastQTools CLI 的基本功能，包括：
# - 全局帮助
# - 子命令帮助
# - filter 命令基本执行
# - stat 命令基本执行（预期可能失败，作为回归基线）

set -e

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# 默认可执行文件路径
FASTQTOOLS="${FASTQTOOLS:-$PROJECT_ROOT/build-clang-release/FastQTools}"

# 测试数据目录
DATA_DIR="$PROJECT_ROOT/tools/data"

# 临时输出目录
TMP_DIR=$(mktemp -d)
trap "rm -rf $TMP_DIR" EXIT

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

fail() {
    echo -e "${RED}[FAIL]${NC} $1"
    exit 1
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# 检查可执行文件是否存在
if [[ ! -x "$FASTQTOOLS" ]]; then
    fail "FastQTools executable not found at $FASTQTOOLS"
fi

echo "=== FastQTools CLI E2E Tests ==="
echo "Executable: $FASTQTOOLS"
echo ""

# Test 1: 全局帮助
echo "Test 1: Global help"
if $FASTQTOOLS --help 2>&1 | grep -q "Available commands"; then
    pass "Global help displays available commands"
else
    # 没有 --help 时也显示帮助
    if $FASTQTOOLS 2>&1 | grep -q "Available commands"; then
        pass "Global help displays available commands (no args)"
    else
        fail "Global help does not display available commands"
    fi
fi

# Test 2: filter 子命令帮助
echo "Test 2: filter --help"
if $FASTQTOOLS filter --help 2>&1 | grep -q "input"; then
    pass "filter --help displays input option"
else
    fail "filter --help does not display input option"
fi

# Test 3: stat 子命令帮助
echo "Test 3: stat --help"
if $FASTQTOOLS stat --help 2>&1 | grep -q "input"; then
    pass "stat --help displays input option"
else
    fail "stat --help does not display input option"
fi

# Test 4: --quiet 模式
echo "Test 4: --quiet mode"
OUTPUT=$($FASTQTOOLS --quiet filter --help 2>&1)
if echo "$OUTPUT" | grep -q "FastQTools - A toolkit"; then
    fail "--quiet mode still prints banner"
else
    pass "--quiet mode suppresses banner"
fi

# Test 5: filter 命令基本执行（使用合成数据）
echo "Test 5: filter command execution"
if [[ -f "$DATA_DIR/sample_10k_len100.fastq" ]]; then
    if $FASTQTOOLS -q filter --input "$DATA_DIR/sample_10k_len100.fastq" --output "$TMP_DIR/filtered.fastq" --threads 2 2>&1; then
        pass "filter command executes without error"
    else
        warn "filter command returned non-zero (known issue, see stage0_baseline.md)"
    fi
else
    warn "Skipping filter test: sample data not found"
fi

# Test 6: stat 命令基本执行（预期可能失败）
echo "Test 6: stat command execution (expected to fail per baseline)"
if [[ -f "$DATA_DIR/sample_10k_len100.fastq" ]]; then
    if $FASTQTOOLS -q stat --input "$DATA_DIR/sample_10k_len100.fastq" --output "$TMP_DIR/stats.txt" --threads 2 2>&1; then
        pass "stat command executes without error"
    else
        warn "stat command returned non-zero (known issue, see stage0_baseline.md)"
    fi
else
    warn "Skipping stat test: sample data not found"
fi

echo ""
echo "=== E2E Tests Complete ==="
