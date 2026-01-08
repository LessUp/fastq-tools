#!/bin/bash
# scripts/install_deps.sh
#
# This script installs dependencies for FastQTools.
# It supports both development and runtime dependency installation.
# Designed for Ubuntu-based systems.

set -euo pipefail

# Color codes for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly NC='\033[0m' # No Color

# Default installation mode
INSTALL_MODE="dev"

# Parse command line arguments
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  --dev, --development    Install development dependencies (default)"
    echo "  --runtime              Install only runtime dependencies"
    echo "  --all                  Install both development and runtime dependencies"
    echo "  -h, --help             Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                     # Install development dependencies"
    echo "  $0 --runtime           # Install only runtime dependencies"
    echo "  $0 --all               # Install all dependencies"
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --dev|--development)
            INSTALL_MODE="dev"
            shift
            ;;
        --runtime)
            INSTALL_MODE="runtime"
            shift
            ;;
        --all)
            INSTALL_MODE="all"
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option $1${NC}"
            usage
            exit 1
            ;;
    esac
done

echo -e "${GREEN}>>> Starting FastQTools dependency installation (mode: ${INSTALL_MODE})...${NC}"

# Function to install runtime dependencies
install_runtime_deps() {
    echo -e "${BLUE}>>> Installing runtime dependencies...${NC}"
    
    # Runtime libraries needed by FastQTools
    RUNTIME_PACKAGES="
        ca-certificates
        libtbb12
        zlib1g
        libbz2-1.0
        liblzma5
        libdeflate0
    "
    
    if ! sudo apt-get install -y $RUNTIME_PACKAGES; then
        echo -e "${RED}Error: Failed to install runtime packages${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}>>> Runtime dependencies installed successfully${NC}"
}

# Function to install development dependencies
install_dev_deps() {
    echo -e "${BLUE}>>> Installing development dependencies...${NC}"
    
    # Essential build tools
    ESSENTIAL_PACKAGES="
        build-essential
        wget
        gnupg
        software-properties-common
        python3-pip
        ninja-build
        pkg-config
        git
        ccache
        tar
        xz-utils
    "
    
    if ! sudo apt-get install -y $ESSENTIAL_PACKAGES; then
        echo -e "${RED}Error: Failed to install essential packages${NC}"
        exit 1
    fi
    
    # Development libraries (headers and dev packages)
    DEV_PACKAGES="
        libtbb-dev
        zlib1g-dev
        libbz2-dev
        liblzma-dev
        libdeflate-dev
    "
    
    if ! sudo apt-get install -y $DEV_PACKAGES; then
        echo -e "${RED}Error: Failed to install development packages${NC}"
        exit 1
    fi
    
    # Install CMake 4.2.1 (Kitware prebuilt)
    echo -e "${BLUE}>>> Installing CMake 4.2.1...${NC}"
    CMAKE_VERSION=4.2.1
    ARCH="$(dpkg --print-architecture)"
    case "$ARCH" in
        amd64) CMAKE_ARCH="x86_64" ;;
        arm64) CMAKE_ARCH="aarch64" ;;
        *)
            echo -e "${RED}Error: Unsupported architecture for CMake prebuilt: $ARCH${NC}"
            exit 1
            ;;
    esac

    if ! wget -q "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-${CMAKE_ARCH}.tar.gz" -O /tmp/cmake.tar.gz; then
        echo -e "${RED}Error: Failed to download CMake ${CMAKE_VERSION}${NC}"
        exit 1
    fi

    if ! sudo mkdir -p /opt; then
        echo -e "${RED}Error: Failed to create /opt for CMake installation${NC}"
        exit 1
    fi

    if ! sudo tar -C /opt -xzf /tmp/cmake.tar.gz; then
        echo -e "${RED}Error: Failed to extract CMake archive${NC}"
        exit 1
    fi

    sudo ln -sf "/opt/cmake-${CMAKE_VERSION}-linux-${CMAKE_ARCH}/bin/cmake" /usr/local/bin/cmake
    sudo ln -sf "/opt/cmake-${CMAKE_VERSION}-linux-${CMAKE_ARCH}/bin/ctest" /usr/local/bin/ctest
    sudo ln -sf "/opt/cmake-${CMAKE_VERSION}-linux-${CMAKE_ARCH}/bin/cpack" /usr/local/bin/cpack
    rm -f /tmp/cmake.tar.gz

    # Install Clang/LLVM version 21 for development
    echo -e "${BLUE}>>> Installing Clang/LLVM version 21...${NC}"
    if [ ! -f "llvm.sh" ]; then
        if ! wget https://apt.llvm.org/llvm.sh; then
            echo -e "${RED}Error: Failed to download LLVM installer${NC}"
            exit 1
        fi
    fi
    
    chmod +x llvm.sh
    if ! sudo ./llvm.sh 21; then
        echo -e "${RED}Error: Failed to install LLVM 21${NC}"
        exit 1
    fi
    
    if ! sudo apt-get install -y clang-21 clangd-21 clang-tidy-21 clang-format-21 libclang-21-dev lld-21 llvm-21; then
        echo -e "${RED}Error: Failed to install LLVM tools${NC}"
        exit 1
    fi
    
    # Set default clang to version 21
    echo -e "${BLUE}>>> Setting default clang to version 21...${NC}"
    sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-21 100
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-21 100
    sudo update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-21 100
    sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-21 100
    sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-21 100
    
    # Install Conan for dependency management
    echo -e "${BLUE}>>> Installing Conan...${NC}"
    
    # Check if pipx is available, if not install it
    if ! command -v pipx &> /dev/null; then
        echo -e "${BLUE}>>> Installing pipx for Python package management...${NC}"
        if ! sudo apt-get install -y pipx; then
            echo -e "${RED}Error: Failed to install pipx${NC}"
            exit 1
        fi
    fi
    
    # Install Conan using pipx (recommended for Ubuntu 24.04+)
    if ! pipx install conan==2.24.0; then
        echo -e "${YELLOW}Warning: pipx install failed, trying pip with --break-system-packages...${NC}"
        if ! pip3 install --break-system-packages conan==2.24.0; then
            echo -e "${RED}Error: Failed to install Conan${NC}"
            exit 1
        fi
    fi
    
    # Ensure pipx binaries are in PATH
    if ! echo "$PATH" | grep -q "$HOME/.local/bin"; then
        echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
        export PATH="$HOME/.local/bin:$PATH"
    fi

    # Optional development tools
    echo -e "${BLUE}>>> Installing optional development tools...${NC}"
    OPTIONAL_DEV_PACKAGES="
        gdb
        valgrind
        lcov
        doxygen
        graphviz
    "
    
    if ! sudo apt-get install -y $OPTIONAL_DEV_PACKAGES; then
        echo -e "${YELLOW}Warning: Some optional development tools failed to install${NC}"
    fi
    
    # Clean up
    rm -f llvm.sh
    
    echo -e "${GREEN}>>> Development dependencies installed successfully${NC}"
}

# 1. Check if running on Ubuntu
if ! grep -q "Ubuntu" /etc/os-release 2>/dev/null; then
    echo -e "${YELLOW}Warning: This script is designed for Ubuntu systems.${NC}"
    echo -e "${YELLOW}Your system may not be fully compatible.${NC}"
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# 2. Update package lists
echo -e "${GREEN}>>> Updating package lists...${NC}"
if ! sudo apt-get update; then
    echo -e "${RED}Error: Failed to update package lists${NC}"
    exit 1
fi

# 3. Install dependencies based on mode
case $INSTALL_MODE in
    "runtime")
        install_runtime_deps
        ;;
    "dev")
        install_runtime_deps
        install_dev_deps
        ;;
    "all")
        install_runtime_deps
        install_dev_deps
        ;;
esac

# 4. Verify installation
echo -e "${GREEN}>>> Verifying installation...${NC}"

# Verify runtime dependencies
echo -e "${BLUE}>>> Verifying runtime libraries...${NC}"

# Check installed packages instead of ldconfig
runtime_packages=("libtbb12" "zlib1g" "libbz2-1.0" "liblzma5" "libdeflate0")
for pkg in "${runtime_packages[@]}"; do
    if dpkg -l "$pkg" 2>/dev/null | grep -q "^ii"; then
        echo -e "${GREEN}✓ $pkg installed${NC}"
    else
        echo -e "${YELLOW}Warning: $pkg not found${NC}"
    fi
done

echo -e "${GREEN}✓ Runtime dependencies verified${NC}"

# Verify development tools if in dev mode
if [[ "$INSTALL_MODE" == "dev" || "$INSTALL_MODE" == "all" ]]; then
    echo -e "${BLUE}>>> Checking development tools...${NC}"
    dev_ok=true
    
    for cmd in gcc g++ clang clang++ cmake ninja conan; do
        if ! command -v $cmd &> /dev/null; then
            echo -e "${RED}Error: $cmd is not available after installation${NC}"
            dev_ok=false
        fi
    done
    
    if [ "$dev_ok" = true ]; then
        echo -e "${GREEN}✓ Development tools verified${NC}"
    else
        echo -e "${RED}✗ Some development tools are missing${NC}"
        exit 1
    fi
fi

echo -e "${GREEN}>>> Installation complete!${NC}"

# Show version information
if [[ "$INSTALL_MODE" == "dev" || "$INSTALL_MODE" == "all" ]]; then
    echo -e "${GREEN}Development Environment:${NC}"
    echo -e "${GREEN}    GCC:       $(gcc --version | head -n1)${NC}"
    echo -e "${GREEN}    Clang:     $(clang --version | head -n1)${NC}"
    echo -e "${GREEN}    CMake:     $(cmake --version | head -n1)${NC}"
    echo -e "${GREEN}    Ninja:     $(ninja --version)${NC}"
    echo -e "${GREEN}    Conan:     $(conan --version)${NC}"
fi

if [[ "$INSTALL_MODE" == "runtime" ]]; then
    echo -e "${GREEN}Runtime Environment: Ready for FastQTools execution${NC}"
fi