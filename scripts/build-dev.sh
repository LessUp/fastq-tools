#!/bin/bash
# =============================================================================
# FastQTools 开发构建脚本
# =============================================================================
# 用法:
#   ./scripts/build-dev.sh              # 默认 GCC Debug
#   ./scripts/build-dev.sh gcc          # GCC Debug
#   ./scripts/build-dev.sh gcc release  # GCC Release
#   ./scripts/build-dev.sh clang        # Clang Debug
#   ./scripts/build-dev.sh clang asan   # Clang AddressSanitizer
#   ./scripts/build-dev.sh clang tsan   # Clang ThreadSanitizer
# =============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

cd "$PROJECT_ROOT"

# 默认参数
COMPILER="${1:-gcc}"
BUILD_TYPE="${2:-debug}"

# 构建 preset 名称
case "$COMPILER" in
    gcc)
        case "$BUILD_TYPE" in
            debug)          PRESET="gcc-debug" ;;
            release)        PRESET="gcc-release" ;;
            relwithdebinfo) PRESET="gcc-relwithdebinfo" ;;
            coverage)       PRESET="coverage" ;;
            *)              echo "Unknown build type: $BUILD_TYPE"; exit 1 ;;
        esac
        CONAN_PROFILE="config/conan/profile-gcc"
        ;;
    clang)
        case "$BUILD_TYPE" in
            debug)   PRESET="clang-debug" ;;
            release) PRESET="clang-release" ;;
            asan)    PRESET="clang-asan" ;;
            tsan)    PRESET="clang-tsan" ;;
            *)       echo "Unknown build type: $BUILD_TYPE"; exit 1 ;;
        esac
        CONAN_PROFILE="config/conan/profile-clang"
        ;;
    *)
        echo "Unknown compiler: $COMPILER"
        echo "Usage: $0 [gcc|clang] [debug|release|asan|tsan|coverage]"
        exit 1
        ;;
esac

echo "=============================================="
echo "FastQTools Build"
echo "=============================================="
echo "Compiler: $COMPILER"
echo "Preset:   $PRESET"
echo "Profile:  $CONAN_PROFILE"
echo "=============================================="

# 安装 Conan 依赖
echo ""
echo "[1/3] Installing Conan dependencies..."
if [ -f "$CONAN_PROFILE" ]; then
    conan install config/dependencies/ \
        -pr:h "$CONAN_PROFILE" \
        --build=missing \
        -of="build/$PRESET"
else
    echo "Warning: Profile not found, using auto-detected profile"
    conan install config/dependencies/ \
        --build=missing \
        -of="build/$PRESET"
fi

# CMake 配置
echo ""
echo "[2/3] Configuring with CMake..."
cmake --preset "$PRESET"

# 构建
echo ""
echo "[3/3] Building..."
cmake --build --preset "$PRESET" -j "$(nproc)"

echo ""
echo "=============================================="
echo "Build complete!"
echo "Binary: build/$PRESET/FastQTools"
echo "=============================================="
