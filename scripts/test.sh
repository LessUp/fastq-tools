#!/bin/bash

# FastQTools 测试运行脚本（与 scripts/build.sh 保持一致）
# 使用 ctest --test-dir 在统一的构建目录中运行测试

set -euo pipefail

# 颜色定义
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly BLUE='\033[0;34m'
readonly YELLOW='\033[1;33m'
readonly NC='\033[0m' # No Color

# 默认值
COMPILER="clang"         # gcc | clang
BUILD_TYPE="Debug"       # Debug | Release | RelWithDebInfo | Coverage
VERBOSE=false
COVERAGE=false
FILTER=""
CTEST_JOBS=""            # 例如 -j 8
REPEAT=1
COVERAGE_DIR=""
HTML_DIR=""

print_usage() {
    echo "FastQTools Test Runner"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -c, --compiler NAME    Compiler: gcc | clang (default: clang)"
    echo "  -t, --type TYPE        Build type: Debug|Release|RelWithDebInfo|Coverage (default: Debug)"
    echo "  -v, --verbose          Enable verbose output"
    echo "  -C, --coverage         Build with coverage instrumentation, run tests, and generate lcov/html reports"
    echo "  -f, --filter PATTERN   Run only tests matching pattern"
    echo "  -j, --jobs N           Run tests with N parallel jobs (ctest -j N)"
    echo "  -r, --repeat COUNT     Repeat tests COUNT times (default: 1)"
    echo "  -h, --help             Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                     # Run all tests with clang Debug"
    echo "  $0 -c gcc -t Release   # Run tests with gcc Release"
    echo "  $0 -f \"*timer*\"        # Run only timer-related tests"
    echo "  $0 -C                  # Run tests and generate coverage (lcov + html)"
    echo "  $0 -r 5                # Run tests 5 times"
}

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--compiler)
            COMPILER="$2"
            shift 2
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -C|--coverage)
            COVERAGE=true
            shift
            ;;
        -f|--filter)
            FILTER="$2"
            shift 2
            ;;
        -j|--jobs)
            CTEST_JOBS="-j $2"
            shift 2
            ;;
        -r|--repeat)
            REPEAT="$2"
            shift 2
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

echo -e "${BLUE}FastQTools Test Runner${NC}"
echo -e "${BLUE}=====================${NC}"
echo ""

# 计算构建目录名称
BT_LOWER=$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')
BUILD_DIR="build-${COMPILER}-${BT_LOWER}"

# 若不存在则先构建
if [[ ! -d "$BUILD_DIR" ]]; then
    echo -e "${YELLOW}Build directory not found. Building project first...${NC}"
    if [[ "$COVERAGE" == true || "$BUILD_TYPE" == "Coverage" ]]; then
        ./scripts/build.sh "$COMPILER" Coverage --coverage
        BUILD_DIR="build-${COMPILER}-coverage"
        COVERAGE_DIR="${BUILD_DIR}/coverage"
        HTML_DIR="${COVERAGE_DIR}/html"
    else
        ./scripts/build.sh "$COMPILER" "$BUILD_TYPE"
    fi
fi

# 覆盖率输出目录默认值
if [[ "$COVERAGE" == true && -z "$COVERAGE_DIR" ]]; then
    COVERAGE_DIR="${BUILD_DIR}/coverage"
    HTML_DIR="${COVERAGE_DIR}/html"
fi

# 构建测试参数
TEST_ARGS=""
if [[ "$VERBOSE" == true ]]; then
    TEST_ARGS="$TEST_ARGS --verbose"
fi

if [[ -n "$FILTER" ]]; then
    TEST_ARGS="$TEST_ARGS -R \"$FILTER\""
fi

# 运行测试
echo -e "${BLUE}Running tests in: $BUILD_DIR${NC}"
echo -e "${BLUE}Test arguments: $TEST_ARGS ${CTEST_JOBS}${NC}"
echo ""

for ((i=1; i<=REPEAT; i++)); do
    if [[ $REPEAT -gt 1 ]]; then
        echo -e "${BLUE}Test run $i of $REPEAT${NC}"
    fi
    
    if eval "ctest --test-dir \"$BUILD_DIR\" $CTEST_JOBS $TEST_ARGS"; then
        echo -e "${GREEN}Tests passed!${NC}"
    else
        echo -e "${RED}Tests failed!${NC}"
        exit 1
    fi
    
    if [[ $i -lt $REPEAT ]]; then
        echo ""
    fi
done

# 生成覆盖率报告
if [[ "$COVERAGE" == true ]]; then
    echo ""
    echo -e "${BLUE}Generating coverage report...${NC}"

    if ! command -v lcov &> /dev/null; then
        echo -e "${YELLOW}lcov not found. Coverage report not generated.${NC}"
        exit 1
    fi

    if ! command -v genhtml &> /dev/null; then
        echo -e "${YELLOW}genhtml not found. Coverage HTML not generated.${NC}"
        exit 1
    fi

    mkdir -p "${COVERAGE_DIR}"

    # 选择 gcov 工具（优先 wrapper 以兼容 llvm-cov）
    GCOV_TOOL=""
    WRAPPER_TOOL="$(dirname "$0")/gcov_wrapper.sh"
    if [ -x "$WRAPPER_TOOL" ]; then
        GCOV_TOOL="$WRAPPER_TOOL"
    else
        if [[ "$COMPILER" == "clang" ]]; then
            if command -v llvm-cov &> /dev/null; then
                GCOV_TOOL="$(mktemp)"
                echo -e "#!/usr/bin/env bash\nexec llvm-cov gcov \"\$@\"" > "$GCOV_TOOL"
                chmod +x "$GCOV_TOOL"
            fi
        else
            if command -v gcov &> /dev/null; then
                GCOV_TOOL="$(command -v gcov)"
            fi
        fi
    fi

    echo "Capturing coverage data..."
    LCOV_CAPTURE_CMD=(lcov --capture \
        --directory "${BUILD_DIR}" \
        --output-file "${COVERAGE_DIR}/coverage.info" \
        --rc lcov_branch_coverage=1 \
        --rc geninfo_unexecuted_blocks=1 \
        --ignore-errors mismatch \
        --ignore-errors version \
        --ignore-errors inconsistent \
        --ignore-errors empty)

    if [[ -n "${GCOV_TOOL}" ]]; then
        LCOV_CAPTURE_CMD+=(--gcov-tool "${GCOV_TOOL}")
    fi

    "${LCOV_CAPTURE_CMD[@]}"

    echo "Filtering coverage data..."
    lcov --remove "${COVERAGE_DIR}/coverage.info" \
        '/usr/*' \
        '*/miniconda3/*' \
        '*/.conan2/*' \
        '*/tests/*' \
        '*/test_*' \
        '*/build-*/*' \
        --output-file "${COVERAGE_DIR}/coverage-filtered.info" \
        --rc lcov_branch_coverage=1 \
        --ignore-errors empty \
        --ignore-errors inconsistent \
        --ignore-errors version \
        --ignore-errors unused

    echo "Generating HTML coverage report..."
    genhtml "${COVERAGE_DIR}/coverage-filtered.info" \
        --output-directory "${HTML_DIR}" \
        --branch-coverage \
        --function-coverage \
        --title "FastQTools Coverage Report" \
        --ignore-errors empty \
        --ignore-errors inconsistent \
        --ignore-errors version

    # 覆盖率摘要
    if command -v python3 &> /dev/null; then
        echo ""
        echo -e "${BLUE}Coverage Summary:${NC}"
        INFO_FILE="${COVERAGE_DIR}/coverage-filtered.info"
        python3 - "$INFO_FILE" << 'EOF'
import re
import sys

def extract(info_file):
    with open(info_file, 'r') as f:
        content = f.read()
    def num(tag):
        m = re.search(rf"{tag}:(\d+)", content)
        return int(m.group(1)) if m else 0
    return {
        'lines': (num("LH"), num("LF")),
        'functions': (num("FH"), num("FNF")),
        'branches': (num("BRH"), num("BRF")),
    }

def pct(hit, total):
    return 0 if total == 0 else (hit / total) * 100

data = extract(sys.argv[1] if len(sys.argv) > 1 else "coverage-filtered.info")
lh, lf = data['lines']
fh, ff = data['functions']
bh, bf = data['branches']
print(f"  Lines:      {lh:6d} / {lf:6d} ({pct(lh, lf):5.1f}%)")
print(f"  Functions:  {fh:6d} / {ff:6d} ({pct(fh, ff):5.1f}%)")
print(f"  Branches:   {bh:6d} / {bf:6d} ({pct(bh, bf):5.1f}%)")
EOF
    fi

    echo ""
    echo -e "${GREEN}Coverage report generated:${NC}"
    echo "  LCOV: ${COVERAGE_DIR}/coverage-filtered.info"
    echo "  HTML: ${HTML_DIR}/index.html"
fi

echo ""
echo -e "${GREEN}Test execution completed!${NC}"
