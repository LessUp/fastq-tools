#!/bin/bash
# scripts/lib/common.sh
# 
# 公共函数库 - 为所有 FastQTools 脚本提供统一的工具函数
# 
# 使用方法:
#   source "$(dirname "${BASH_SOURCE[0]}")/lib/common.sh"

# =============================================================================
# 颜色和样式定义
# =============================================================================
readonly COLOR_RED='\033[0;31m'
readonly COLOR_GREEN='\033[0;32m'
readonly COLOR_YELLOW='\033[1;33m'
readonly COLOR_BLUE='\033[0;34m'
readonly COLOR_MAGENTA='\033[0;35m'
readonly COLOR_CYAN='\033[0;36m'
readonly COLOR_BOLD='\033[1m'
readonly COLOR_RESET='\033[0m'

# =============================================================================
# 日志函数
# =============================================================================

# 信息日志
log_info() {
    echo -e "${COLOR_BLUE}[INFO]${COLOR_RESET} $*"
}

# 成功日志
log_success() {
    echo -e "${COLOR_GREEN}[SUCCESS]${COLOR_RESET} $*"
}

# 警告日志
log_warning() {
    echo -e "${COLOR_YELLOW}[WARNING]${COLOR_RESET} $*" >&2
}

# 错误日志
log_error() {
    echo -e "${COLOR_RED}[ERROR]${COLOR_RESET} $*" >&2
}

# 调试日志（需要 VERBOSE=true）
log_debug() {
    if [[ "${VERBOSE:-false}" == "true" ]]; then
        echo -e "${COLOR_CYAN}[DEBUG]${COLOR_RESET} $*" >&2
    fi
}

# 分隔线
log_separator() {
    local char="${1:--}"
    local width="${2:-80}"
    printf '%*s\n' "$width" '' | tr ' ' "$char"
}

# 带标题的分隔线
log_section() {
    local title="$1"
    log_separator "="
    echo -e "${COLOR_BOLD}${title}${COLOR_RESET}"
    log_separator "="
}

# =============================================================================
# 错误处理
# =============================================================================

# 错误处理函数
error_handler() {
    local exit_code=$1
    local line_number=$2
    log_error "Script failed at line ${line_number} with exit code ${exit_code}"
    log_error "Command: ${BASH_COMMAND}"
    exit "$exit_code"
}

# 设置错误处理陷阱
setup_error_trap() {
    set -euo pipefail
    trap 'error_handler $? $LINENO' ERR
}

# =============================================================================
# 路径和环境工具
# =============================================================================

# 获取项目根目录
get_project_root() {
    local script_dir
    script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
    echo "$(dirname "$script_dir")"
}

# 获取构建目录名称
get_build_dir() {
    local compiler="${1:-clang}"
    local build_type="${2:-release}"
    local build_type_lower
    build_type_lower=$(echo "$build_type" | tr '[:upper:]' '[:lower:]')
    echo "build-${compiler}-${build_type_lower}"
}

# 检查命令是否存在
require_command() {
    local cmd="$1"
    local install_hint="${2:-}"
    
    if ! command -v "$cmd" &>/dev/null; then
        log_error "Required command not found: ${cmd}"
        if [[ -n "$install_hint" ]]; then
            log_info "Install hint: ${install_hint}"
        fi
        return 1
    fi
    return 0
}

# 检查多个命令
require_commands() {
    local missing=()
    local cmd
    
    for cmd in "$@"; do
        if ! command -v "$cmd" &>/dev/null; then
            missing+=("$cmd")
        fi
    done
    
    if [[ ${#missing[@]} -gt 0 ]]; then
        log_error "Missing required commands: ${missing[*]}"
        return 1
    fi
    return 0
}

# =============================================================================
# 文件和目录工具
# =============================================================================

# 确保目录存在
ensure_directory() {
    local dir="$1"
    if [[ ! -d "$dir" ]]; then
        log_debug "Creating directory: ${dir}"
        mkdir -p "$dir"
    fi
}

# 安全地删除目录
safe_remove_dir() {
    local dir="$1"
    if [[ -d "$dir" ]]; then
        log_debug "Removing directory: ${dir}"
        rm -rf "$dir"
    fi
}

# 检查文件是否存在且可执行
check_executable() {
    local file="$1"
    if [[ ! -x "$file" ]]; then
        log_error "Executable not found or not executable: ${file}"
        return 1
    fi
    return 0
}

# =============================================================================
# 参数解析工具
# =============================================================================

# 显示帮助信息的标准格式
print_usage_header() {
    local script_name="$1"
    local description="$2"
    
    log_section "$script_name"
    echo "$description"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
}

# =============================================================================
# 构建工具
# =============================================================================

# 获取 CPU 核心数
get_cpu_cores() {
    if command -v nproc &>/dev/null; then
        nproc
    elif command -v sysctl &>/dev/null; then
        sysctl -n hw.ncpu
    else
        echo "4"  # 默认值
    fi
}

# 标准化构建类型
normalize_build_type() {
    local build_type="$1"
    case "${build_type,,}" in
        debug|dbg|d)
            echo "Debug"
            ;;
        release|rel|r)
            echo "Release"
            ;;
        relwithdebinfo|relwithdeb|rwdi)
            echo "RelWithDebInfo"
            ;;
        coverage|cov)
            echo "Coverage"
            ;;
        sanitize|san|asan)
            echo "Sanitize"
            ;;
        *)
            log_error "Unknown build type: ${build_type}"
            return 1
            ;;
    esac
}

# 标准化编译器名称
normalize_compiler() {
    local compiler="$1"
    case "${compiler,,}" in
        gcc|gnu)
            echo "gcc"
            ;;
        clang|llvm)
            echo "clang"
            ;;
        *)
            log_error "Unknown compiler: ${compiler}"
            return 1
            ;;
    esac
}

# =============================================================================
# 进度显示
# =============================================================================

# 显示进度步骤
show_step() {
    local current="$1"
    local total="$2"
    local description="$3"
    
    echo ""
    log_info "[${current}/${total}] ${description}"
}

# =============================================================================
# 时间测量
# =============================================================================

# 开始计时
start_timer() {
    TIMER_START=$(date +%s)
}

# 结束计时并显示
end_timer() {
    local timer_end
    timer_end=$(date +%s)
    local duration=$((timer_end - TIMER_START))
    local minutes=$((duration / 60))
    local seconds=$((duration % 60))
    
    if [[ $minutes -gt 0 ]]; then
        log_success "Completed in ${minutes}m ${seconds}s"
    else
        log_success "Completed in ${seconds}s"
    fi
}

# =============================================================================
# 系统检测
# =============================================================================

# 检测操作系统
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

# 检测发行版（仅 Linux）
detect_distro() {
    if [[ -f /etc/os-release ]]; then
        source /etc/os-release
        echo "${ID:-unknown}"
    else
        echo "unknown"
    fi
}

# =============================================================================
# 确认提示
# =============================================================================

# 请求用户确认
confirm() {
    local prompt="${1:-Continue?}"
    local default="${2:-n}"
    
    if [[ "$default" == "y" ]]; then
        read -p "${prompt} [Y/n]: " -n 1 -r
    else
        read -p "${prompt} [y/N]: " -n 1 -r
    fi
    echo
    
    if [[ "$default" == "y" ]]; then
        [[ $REPLY =~ ^[Nn]$ ]] && return 1
    else
        [[ ! $REPLY =~ ^[Yy]$ ]] && return 1
    fi
    return 0
}

# =============================================================================
# 版本比较
# =============================================================================

# 比较版本号（返回 0 如果 v1 >= v2）
version_ge() {
    local v1="$1"
    local v2="$2"
    
    # 使用 sort -V 进行版本比较
    if printf '%s\n' "$v2" "$v1" | sort -V -C; then
        return 0
    else
        return 1
    fi
}

# =============================================================================
# 导出函数
# =============================================================================

# 导出所有公共函数供子 shell 使用
export -f log_info log_success log_warning log_error log_debug
export -f log_separator log_section
export -f get_project_root get_build_dir
export -f require_command require_commands
export -f ensure_directory safe_remove_dir check_executable
export -f get_cpu_cores normalize_build_type normalize_compiler
export -f show_step start_timer end_timer
export -f detect_os detect_distro confirm version_ge
