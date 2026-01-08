#!/bin/bash

# FastQTools 开发容器设置脚本
# 在VSCode Dev Container启动时运行

set -euo pipefail

# 颜色定义
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly NC='\033[0m' # No Color

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 设置工作目录
cd /workspace

print_info "=== FastQTools 开发环境设置 ==="

# 检查并初始化Conan
print_info "配置Conan..."
if ! conan profile list | grep -q "default"; then
    conan profile detect --force
fi

# 创建或更新Conan配置
print_info "配置Conan设置..."
conan config set general.default_profile=x86_64-linux-gnu-gcc14-release
conan config set general.revisions_enabled=True
conan config set tools.env.virtualenv.auto_use=True

# 配置编译器缓存
print_info "配置编译器缓存..."
ccache --max-size=5G
ccache --show-stats

# 检查并创建必要的目录
print_info "创建必要目录..."
mkdir -p build
mkdir -p .vscode
mkdir -p logs

# 设置Git配置（如果需要）
if [ -n "${GIT_USER_NAME:-}" ] && [ -n "${GIT_USER_EMAIL:-}" ]; then
    print_info "配置Git用户信息..."
    git config --global user.name "${GIT_USER_NAME}"
    git config --global user.email "${GIT_USER_EMAIL}"
fi

# 生成compile_commands.json链接（如果存在）
if [ -f "build/compile_commands.json" ]; then
    print_info "创建compile_commands.json链接..."
    ln -sf build/compile_commands.json .
fi

# 安装预提交钩子（如果存在）
if [ -f "scripts/pre-commit.sh" ]; then
    print_info "设置预提交钩子..."
    chmod +x scripts/pre-commit.sh
    if [ ! -f ".git/hooks/pre-commit" ]; then
        ln -sf ../../scripts/pre-commit.sh .git/hooks/pre-commit
    fi
fi

# 显示环境信息
print_info "环境信息:"
echo "  工作目录: $(pwd)"
echo "  用户: $(whoami)"
echo "  编译器: $(which clang++) ($(clang++ --version | head -n1))"
echo "  CMake: $(cmake --version | head -n1)"
echo "  Ninja: $(ninja --version)"
echo "  Conan: $(conan --version | head -n1)"

# 验证构建系统
print_info "验证构建系统..."
if command -v cmake &> /dev/null && command -v ninja &> /dev/null && command -v conan &> /dev/null; then
    print_success "构建系统验证通过"
else
    print_error "构建系统验证失败"
    exit 1
fi

print_success "开发环境设置完成！"
echo
echo "可用命令:"
echo "  ./scripts/build.sh --help        # 查看构建参数"
echo "  ./scripts/test.sh --help         # 运行/过滤测试"
echo "  ./scripts/lint.sh --help         # 代码格式与静态检查"
echo "  ./scripts/install_deps.sh        # 安装系统依赖"
echo
echo "更多约定: scripts/README.md"
