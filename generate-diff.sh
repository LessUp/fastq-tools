#!/bin/bash
# generate-diff.sh
#
# 一个用于生成 git diff 文件的 Bash 脚本。

# 当命令失败时立即退出
set -euo pipefail

# --- 用户可配置参数 ---
# 注意：路径中不能包含空格。
# 指定要包含的目录或文件 (用空格分隔, 例如: "src lib")。留空则包含所有。
INCLUDE_PATHS=""
# 指定要排除的目录或文件 (用空格分隔, 例如: "*.md" "dist/")。留空则不排除任何。
EXCLUDE_PATHS=""
# --- 配置结束 ---

# --- 函数定义 ---

# 检查当前是否在 git 仓库中
function check_git_repo() {
    if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
        echo "错误：当前目录不是一个 git 仓库。请在您的 git 项目目录中运行此脚本。"
        exit 1
    fi
}

# 获取项目根目录并创建 diff 输出目录
function get_output_directory() {
    # 使用 git 命令获取项目根目录
    local project_root
    if ! project_root=$(git rev-parse --show-toplevel); then
        echo "错误：无法确定 git 项目根目录。"
        exit 1
    fi
    
    # 定义并创建输出目录
    local output_dir="$project_root/_output/diffs"
    if [ ! -d "$output_dir" ]; then
        mkdir -p "$output_dir"
        echo "已创建输出目录: $output_dir" >&2
    fi
    # 将结果返回给调用者
    echo "$output_dir"
}

resolve_short_rev() {
    local rev="$1"
    local short
    if ! short=$(git rev-parse --short "$rev" 2>/dev/null); then
        echo "错误: 无效的引用: $rev" >&2
        exit 1
    fi
    echo "$short"
}

print_usage() {
    echo "用法: $0 [-m a|b|c|d] [-s START] [-e END] [-o OUTPUT_DIR] [-c CONTEXT] [-I INCLUDE_PATHS] [-X EXCLUDE_PATHS] [-n]"
    echo "  -m  模式: a(工作区未提交) b(START..HEAD) c(START..END) d(START..工作区)"
    echo "  -s  起始引用 (b/c/d 需要)"
    echo "  -e  结束引用 (c 需要)"
    echo "  -o  输出目录 (默认: 项目根 _output/diffs)"
    echo "  -c  上下文行数 (默认: 10)"
    echo "  -I  包含路径 (空格分隔; 注意: 不支持带空格的路径)"
    echo "  -X  排除路径 (空格分隔; 注意: 不支持带空格的路径)"
    echo "  -n  非交互模式 (缺少必要参数将报错退出)"
}

cleanup_on_error() {
    if [ -n "${OUTPUT_FILE:-}" ] && [ -f "${OUTPUT_FILE:-}" ]; then
        rm -f -- "$OUTPUT_FILE"
    fi
    echo "生成 diff 失败。请检查输入参数或 commit 引用是否正确。" >&2
}

# --- 主逻辑 ---

check_git_repo

MODE=""
START_REV=""
END_REV=""
OUTPUT_DIR=""
CONTEXT_LINES=10
NON_INTERACTIVE=false

while [ $# -gt 0 ]; do
    case "$1" in
        -m)
            MODE="${2:-}"; shift 2 ;;
        -s)
            START_REV="${2:-}"; shift 2 ;;
        -e)
            END_REV="${2:-}"; shift 2 ;;
        -o)
            OUTPUT_DIR="${2:-}"; shift 2 ;;
        -c)
            CONTEXT_LINES="${2:-10}"; shift 2 ;;
        -I)
            INCLUDE_PATHS="${2:-}"; shift 2 ;;
        -X)
            EXCLUDE_PATHS="${2:-}"; shift 2 ;;
        -n)
            NON_INTERACTIVE=true; shift 1 ;;
        -h|--help)
            print_usage; exit 0 ;;
        *)
            echo "错误: 未知参数 '$1'" >&2
            print_usage
            exit 1
            ;;
    esac
done

# 获取统一的输出目录
DIFFS_DIR=$(get_output_directory)
if [ -n "$OUTPUT_DIR" ]; then
    DIFFS_DIR="$OUTPUT_DIR"
    mkdir -p -- "$DIFFS_DIR"
fi

# 显示菜单
echo ""
echo "请选择要生成的 diff 类型:"
echo "  a. 工作区中未提交的更改 (git diff HEAD)"
echo "  b. 从指定 commit 到 HEAD 的更改"
echo "  c. 两个指定 commit 之间的更改"
echo "  d. 从指定 commit 到当前工作区 (包含未提交的更改)"

choice="$MODE"
if [ -z "$choice" ]; then
    if [ "$NON_INTERACTIVE" = true ]; then
        echo "错误: 非交互模式下必须提供 -m (a/b/c/d)。" >&2
        exit 1
    fi
    read -p "请输入选项 (a/b/c/d): " choice
fi

# 基础命令已优化
GIT_ARGS=(diff "--unified=$CONTEXT_LINES" -w --no-prefix)
OUTPUT_FILE=""
TIMESTAMP=$(date +%Y%m%d-%H%M%S)

# 构造路径过滤参数
# 注意: 这种方式不支持带空格的路径
PATHSPEC_ARGS=()
if [ -n "$INCLUDE_PATHS" ] || [ -n "$EXCLUDE_PATHS" ]; then
    PATHSPEC_ARGS+=(--)
    if [ -n "$INCLUDE_PATHS" ]; then
        for p in $INCLUDE_PATHS; do
            PATHSPEC_ARGS+=("$p")
        done
    fi
    if [ -n "$EXCLUDE_PATHS" ]; then
        for p in $EXCLUDE_PATHS; do
            PATHSPEC_ARGS+=(":(exclude)$p")
        done
    fi
fi

trap cleanup_on_error ERR

RANGE_ARG=""

case $choice in
    a)
        OUTPUT_FILE="$DIFFS_DIR/diff_workspace_uncommitted_${TIMESTAMP}.diff"
        RANGE_ARG="HEAD"
        ;;
    b)
        startHash="$START_REV"
        if [ -z "$startHash" ] && [ "$NON_INTERACTIVE" = false ]; then
            read -p "请输入起始 commit hash: " startHash
        fi
        if [ -z "$startHash" ]; then echo "错误: Hash 不能为空。"; exit 1; fi

        shortStart=$(resolve_short_rev "$startHash")
        OUTPUT_FILE="$DIFFS_DIR/diff_${shortStart}_to_HEAD_${TIMESTAMP}.diff"
        RANGE_ARG="${startHash}..HEAD"
        ;;
    c)
        startHash="$START_REV"
        endHash="$END_REV"
        if ([ -z "$startHash" ] || [ -z "$endHash" ]) && [ "$NON_INTERACTIVE" = false ]; then
            if [ -z "$startHash" ]; then read -p "请输入起始 commit hash: " startHash; fi
            if [ -z "$endHash" ]; then read -p "请输入结束 commit hash: " endHash; fi
        fi
        if [ -z "$startHash" ] || [ -z "$endHash" ]; then echo "错误: Hash 不能为空。"; exit 1; fi

        shortStart=$(resolve_short_rev "$startHash")
        shortEnd=$(resolve_short_rev "$endHash")
        OUTPUT_FILE="$DIFFS_DIR/diff_${shortStart}_${shortEnd}_${TIMESTAMP}.diff"
        RANGE_ARG="${startHash}..${endHash}"
        ;;
    d)
        startHash="$START_REV"
        if [ -z "$startHash" ] && [ "$NON_INTERACTIVE" = false ]; then
            read -p "请输入起始 commit hash: " startHash
        fi
        if [ -z "$startHash" ]; then echo "错误: Hash 不能为空。"; exit 1; fi

        shortStart=$(resolve_short_rev "$startHash")
        OUTPUT_FILE="$DIFFS_DIR/diff_${shortStart}_to_workspace_${TIMESTAMP}.diff"
        RANGE_ARG="$startHash"
        ;;
    *)
        echo "错误: 无效的选项 '$choice'。"
        exit 1
        ;;
esac

echo "正在执行 git 命令: git ${GIT_ARGS[*]} --output=$OUTPUT_FILE $RANGE_ARG"
git "${GIT_ARGS[@]}" "--output=$OUTPUT_FILE" "$RANGE_ARG" "${PATHSPEC_ARGS[@]}"

if [ -s "$OUTPUT_FILE" ]; then
    echo "成功创建 diff 文件: $OUTPUT_FILE"
else
    echo "操作成功，但未检测到差异，已生成空文件: $OUTPUT_FILE"
fi