#!/bin/bash
# tests/e2e/test_cli.sh
# CLI 端到端测试脚本
#
# 测试 FastQTools CLI 的基本功能，包括：
# - 全局帮助
# - 子命令帮助
# - filter 命令基本执行
# - stat 命令基本执行
# - trim-quality 行为回归

set -euo pipefail

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# 默认可执行文件路径
FASTQTOOLS="${FASTQTOOLS:-$PROJECT_ROOT/build/clang-release/FastQTools}"

# 测试数据目录
DATA_DIR="$PROJECT_ROOT/tools/data"

# 临时输出目录（限制在仓库内，避免写入系统临时目录）
TMP_ROOT="$PROJECT_ROOT/tests/e2e/.tmp"
mkdir -p "$TMP_ROOT"
TMP_DIR="$(mktemp -d "$TMP_ROOT/test_cli.XXXXXX")"
cleanup() {
    rm -rf -- "$TMP_DIR"
}
trap cleanup EXIT

# 样本生成到独立 tmp 目录，避免与 e2e_python_cli 并发写同一文件
SAMPLE_FASTQ="$TMP_DIR/sample_10k_len100.fastq"
echo "Generating sample data via gen_fastq.py..."
python3 "$DATA_DIR/gen_fastq.py" -o "$SAMPLE_FASTQ" -n 10000 --min-len 100 --max-len 100 --seed 42

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
    fail "Global help does not display available commands"
fi

# Test 2: 无子命令
echo "Test 2: no subcommand"
set +euo pipefail
$FASTQTOOLS > "$TMP_DIR/no-subcommand.txt" 2>&1
STATUS=$?
set -euo pipefail
if [[ $STATUS -eq 2 ]] && grep -q "Available commands" "$TMP_DIR/no-subcommand.txt"; then
    pass "No subcommand prints help and exits non-zero"
else
    fail "No subcommand did not print help with non-zero exit"
fi

# Test 3: 未知子命令
echo "Test 3: unknown subcommand"
set +euo pipefail
$FASTQTOOLS unknown-command > "$TMP_DIR/unknown-subcommand.txt" 2>&1
STATUS=$?
set -euo pipefail
if [[ $STATUS -eq 2 ]] && grep -q "Unknown subcommand" "$TMP_DIR/unknown-subcommand.txt"; then
    pass "Unknown subcommand reports an error and exits non-zero"
else
    fail "Unknown subcommand did not report an error with non-zero exit"
fi

# Test 4: filter 子命令帮助
echo "Test 4: filter --help"
if $FASTQTOOLS filter --help 2>&1 | grep -q "input"; then
    pass "filter --help displays input option"
else
    fail "filter --help does not display input option"
fi

# Test 5: stat 子命令帮助
echo "Test 5: stat --help"
if $FASTQTOOLS stat --help 2>&1 | grep -q "input"; then
    pass "stat --help displays input option"
else
    fail "stat --help does not display input option"
fi

# Test 6: --log-level 与子命令帮助组合
echo "Test 6: --log-level with subcommand help"
set +euo pipefail
$FASTQTOOLS --log-level=debug stat --help > "$TMP_DIR/log-level-help.txt" 2>&1
STATUS=$?
set -euo pipefail
if [[ $STATUS -eq 0 ]] && grep -q "Generate statistics for a FASTQ file" "$TMP_DIR/log-level-help.txt"; then
    pass "--log-level works with subcommand help"
else
    fail "--log-level does not work with subcommand help"
fi

# Test 7: --quiet 模式
echo "Test 7: --quiet mode"
OUTPUT=$($FASTQTOOLS --quiet filter --help 2>&1)
if echo "$OUTPUT" | grep -q "FastQTools - A toolkit"; then
    fail "--quiet mode still prints banner"
else
    pass "--quiet mode suppresses banner"
fi

# Test 8: filter 缺少必需参数
echo "Test 8: filter missing required args"
NEGATIVE_INPUT="$TMP_DIR/negative-input.fastq"
cat > "$NEGATIVE_INPUT" <<'EOF'
@negative-read
ACGT
+
!!!!
EOF
set +euo pipefail
$FASTQTOOLS --quiet filter --input "$NEGATIVE_INPUT" > "$TMP_DIR/filter-missing-output.txt" 2>&1
STATUS_OUTPUT=$?
$FASTQTOOLS --quiet filter --output "$TMP_DIR/filter.fastq" > "$TMP_DIR/filter-missing-input.txt" 2>&1
STATUS_INPUT=$?
set -euo pipefail
if [[ $STATUS_OUTPUT -eq 2 ]] && [[ $STATUS_INPUT -eq 2 ]] &&
   grep -q "both --input and --output options are required" "$TMP_DIR/filter-missing-output.txt" &&
   grep -q "both --input and --output options are required" "$TMP_DIR/filter-missing-input.txt"; then
    pass "filter rejects missing input/output arguments"
else
    fail "filter did not reject missing input/output arguments"
fi

# Test 9: stat 缺少必需参数
echo "Test 9: stat missing required args"
set +euo pipefail
$FASTQTOOLS --quiet stat --input "$NEGATIVE_INPUT" > "$TMP_DIR/stat-missing-output.txt" 2>&1
STATUS_OUTPUT=$?
$FASTQTOOLS --quiet stat --output "$TMP_DIR/stat.txt" > "$TMP_DIR/stat-missing-input.txt" 2>&1
STATUS_INPUT=$?
set -euo pipefail
if [[ $STATUS_OUTPUT -eq 2 ]] && [[ $STATUS_INPUT -eq 2 ]] &&
   grep -q "both --input and --output options are required" "$TMP_DIR/stat-missing-output.txt" &&
   grep -q "both --input and --output options are required" "$TMP_DIR/stat-missing-input.txt"; then
    pass "stat rejects missing input/output arguments"
else
    fail "stat did not reject missing input/output arguments"
fi

# Test 10: filter 命令基本执行

echo "Test 10: filter command execution"
if [[ -f "$DATA_DIR/sample_10k_len100.fastq" ]]; then
    if $FASTQTOOLS -q filter --input "$DATA_DIR/sample_10k_len100.fastq" --output "$TMP_DIR/filtered.fastq" --threads 2 2>&1; then
        pass "filter command executes without error"
        [[ -s "$TMP_DIR/filtered.fastq" ]] || fail "filter command did not produce output"
    else
        fail "filter command returned non-zero"
    fi
else
    warn "Skipping filter test: sample data not found"
fi

# Test 11: stat 命令基本执行

echo "Test 11: stat command execution"
if [[ -f "$DATA_DIR/sample_10k_len100.fastq" ]]; then
    if $FASTQTOOLS -q stat --input "$DATA_DIR/sample_10k_len100.fastq" --output "$TMP_DIR/stats.txt" --threads 2 2>&1; then
        pass "stat command executes without error"
        grep -q $'^#ReadNum\t10000$' "$TMP_DIR/stats.txt" || fail "stat output missing read count"
    else
        fail "stat command returned non-zero"
    fi
else
    warn "Skipping stat test: sample data not found"
fi

# Test 12: 错误类别使用稳定退出码
echo "Test 12: stable error exit codes"
MALFORMED_FASTQ="$TMP_DIR/malformed.fastq"
cat > "$MALFORMED_FASTQ" <<'EOF'
@malformed-read
ACGT
-
!!!!
EOF
set +euo pipefail
$FASTQTOOLS -q filter --input "$MALFORMED_FASTQ" --output "$TMP_DIR/malformed-output.fastq" > "$TMP_DIR/format-error.txt" 2>&1
STATUS_FORMAT=$?
$FASTQTOOLS -q filter --input "$TMP_DIR/does-not-exist.fastq" --output "$TMP_DIR/io-output.fastq" > "$TMP_DIR/io-error.txt" 2>&1
STATUS_IO=$?
set -euo pipefail
if [[ $STATUS_FORMAT -eq 3 ]] && [[ $STATUS_IO -eq 4 ]]; then
    pass "format and I/O failures use distinct stable exit codes"
else
    fail "format/I/O exit code mapping changed (format=$STATUS_FORMAT io=$STATUS_IO)"
fi

# Test 13: trim-quality 实际裁剪生效

echo "Test 13: trim-quality changes output"
TRIM_INPUT="$TMP_DIR/trim-input.fastq"
cat > "$TRIM_INPUT" <<'EOF'
@trim-read
ACGT
+
!!II
EOF
if $FASTQTOOLS -q filter --input "$TRIM_INPUT" --output "$TMP_DIR/trimmed.fastq" --trim-quality 20 2>&1; then
    if grep -q '^GT$' "$TMP_DIR/trimmed.fastq"; then
        pass "trim-quality trims low-quality ends"
    else
        fail "trim-quality did not trim as expected"
    fi
else
    fail "trim-quality command returned non-zero"
fi

echo ""
echo "=== E2E Tests Complete ==="
