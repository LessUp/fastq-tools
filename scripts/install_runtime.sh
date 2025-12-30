#!/bin/bash
# scripts/install_runtime.sh
#
# Lightweight script to install only runtime dependencies for FastQTools.
# This script is optimized for production deployments and end users.

set -euo pipefail

# Color codes for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly NC='\033[0m' # No Color

echo -e "${GREEN}>>> Installing FastQTools runtime dependencies...${NC}"

# Check if running on Ubuntu/Debian
if ! grep -qE "(Ubuntu|Debian)" /etc/os-release 2>/dev/null; then
    echo -e "${YELLOW}Warning: This script is designed for Ubuntu/Debian systems.${NC}"
    echo -e "${YELLOW}Your system may not be fully compatible.${NC}"
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Update package lists
echo -e "${GREEN}>>> Updating package lists...${NC}"
if ! sudo apt-get update; then
    echo -e "${RED}Error: Failed to update package lists${NC}"
    exit 1
fi

# Install runtime dependencies
echo -e "${GREEN}>>> Installing runtime libraries...${NC}"
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

# Verify installation
echo -e "${GREEN}>>> Verifying runtime dependencies...${NC}"
runtime_ok=true

# Check if runtime libraries are available
declare -A lib_checks=(
    ["libtbb.so.12"]="Intel TBB"
    ["libz.so.1"]="zlib"
    ["libbz2.so.1.0"]="bzip2"
    ["liblzma.so.5"]="LZMA"
    ["libdeflate.so.0"]="libdeflate"
)

for lib in "${!lib_checks[@]}"; do
    if ldconfig -p | grep -q "$lib"; then
        echo -e "${GREEN}✓ ${lib_checks[$lib]} ($lib) found${NC}"
    else
        echo -e "${RED}✗ ${lib_checks[$lib]} ($lib) not found${NC}"
        runtime_ok=false
    fi
done

if [ "$runtime_ok" = true ]; then
    echo -e "${GREEN}>>> Runtime dependencies installed successfully!${NC}"
    echo -e "${GREEN}Your system is ready to run FastQTools.${NC}"
else
    echo -e "${RED}>>> Some runtime dependencies are missing.${NC}"
    echo -e "${YELLOW}FastQTools may not work correctly.${NC}"
    exit 1
fi

echo -e "${GREEN}>>> Installation complete!${NC}"
echo -e "${GREEN}You can now install and run FastQTools binaries.${NC}"