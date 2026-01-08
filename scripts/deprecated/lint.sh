#!/bin/bash
# scripts/lint.sh
#
# 统一的代码质量脚本：
# - lint       : clang-tidy + clang-format 检查（默认）
# - lint-fix   : clang-tidy 修复 + clang-format 修复
# - format     : 仅 clang-format 修复
# - format-check: 仅 clang-format 检查

set -euo pipefail

# Color codes for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly NC='\033[0m' # No Color

# Defaults
COMMAND="lint"
BUILD_DIR="build-clang-release"
STYLE_FILE=".clang-format"
CLANG_FORMAT_BIN="${CLANG_FORMAT:-clang-format}"
VERBOSE=false

print_usage() {
    echo "FastQTools Lint & Format"
    echo ""
    echo "Usage: $0 [COMMAND] [OPTIONS]"
    echo ""
    echo "Commands:"
    echo "  lint           (default) clang-tidy + clang-format check"
    echo "  lint-fix       clang-tidy fix + clang-format fix"
    echo "  format         clang-format fix only"
    echo "  format-check   clang-format check only"
    echo ""
    echo "Options:"
    echo "  -b, --build-dir DIR   Build dir with compile_commands.json (lint*)"
    echo "  -s, --style FILE      clang-format style file (default: .clang-format)"
    echo "  -v, --verbose         Verbose output"
    echo "  -h, --help            Show this help"
}

# Detect command
if [[ $# -gt 0 ]]; then
    case "$1" in
        lint|lint-fix|format|format-check)
            COMMAND="$1"
            shift
            ;;
    esac
fi

# Parse options
while [[ $# -gt 0 ]]; do
    case $1 in
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -s|--style)
            STYLE_FILE="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
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

log() {
    if [[ "$VERBOSE" == true ]]; then
        echo -e "${BLUE}[INFO]${NC} $1"
    fi
}

ensure_build() {
    if [[ "$COMMAND" == "lint" || "$COMMAND" == "lint-fix" ]]; then
        if [ ! -d "$BUILD_DIR" ]; then
            echo -e "${YELLOW}Build directory '$BUILD_DIR' missing, building...${NC}"
            ./scripts/build.sh clang Release
        fi
        if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
            echo -e "${RED}Error: compile_commands.json not found in $BUILD_DIR${NC}"
            exit 1
        fi
    fi
}

ensure_tools() {
    local format_candidates=("$CLANG_FORMAT_BIN" "clang-format" "clang-format-19")
    CLANG_FORMAT_BIN=""
    for c in "${format_candidates[@]}"; do
        if command -v "$c" &> /dev/null; then
            CLANG_FORMAT_BIN="$c"
            break
        fi
    done
    if [[ -z "$CLANG_FORMAT_BIN" ]]; then
        echo -e "${RED}Error: clang-format is not installed (tried: ${format_candidates[*]})${NC}"
        exit 1
    fi

    if [[ "$COMMAND" == "lint" || "$COMMAND" == "lint-fix" ]]; then
        if ! command -v clang-tidy &> /dev/null; then
            echo -e "${RED}Error: clang-tidy is not installed${NC}"
            exit 1
        fi
    fi
}

collect_sources() {
    local files=()
    local dirs=("src" "include" "tests")
    local exts=("*.cpp" "*.hpp" "*.h" "*.cppm")
    for dir in "${dirs[@]}"; do
        if [[ -d "$dir" ]]; then
            for ext in "${exts[@]}"; do
                while IFS= read -r -d '' f; do
                    files+=("$f")
                done < <(find "$dir" -name "$ext" -type f -print0)
            done
        fi
    done
    if [[ ${#files[@]} -eq 0 ]]; then
        echo -e "${RED}Error: no source files found${NC}"
        exit 1
    fi
    SOURCE_FILES=("${files[@]}")
}

run_clang_tidy() {
    echo -e "${GREEN}>>> clang-tidy (${COMMAND})${NC}"
    if command -v run-clang-tidy &> /dev/null; then
        if [[ "$COMMAND" == "lint-fix" ]]; then
            run-clang-tidy -p "$BUILD_DIR" -fix
        else
            run-clang-tidy -p "$BUILD_DIR"
        fi
    else
        for file in "${SOURCE_FILES[@]}"; do
            if [[ "$COMMAND" == "lint-fix" ]]; then
                clang-tidy -p "$BUILD_DIR" "$file" -fix
            else
                clang-tidy -p "$BUILD_DIR" "$file"
            fi
        done
    fi
}

run_clang_format() {
    echo -e "${GREEN}>>> clang-format (${COMMAND})${NC}"
    local style_arg=()
    if [[ -f "$STYLE_FILE" ]]; then
        style_arg=(--style=file:"$STYLE_FILE")
    fi
    if [[ "$COMMAND" == "format" || "$COMMAND" == "lint-fix" ]]; then
        for f in "${SOURCE_FILES[@]}"; do
            "$CLANG_FORMAT_BIN" "${style_arg[@]}" -i "$f"
        done
    else
        for f in "${SOURCE_FILES[@]}"; do
            "$CLANG_FORMAT_BIN" "${style_arg[@]}" --dry-run --Werror "$f"
        done
    fi
}

echo -e "${GREEN}>>> Running ${COMMAND} on FastQTools codebase...${NC}"
ensure_tools
collect_sources
ensure_build

case "$COMMAND" in
    lint)
        run_clang_tidy
        run_clang_format
        ;;
    lint-fix)
        run_clang_tidy
        run_clang_format
        ;;
    format|format-check)
        run_clang_format
        ;;
esac

echo -e "${GREEN}>>> ${COMMAND} completed!${NC}"
