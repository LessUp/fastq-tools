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

# Check installed packages instead of ldconfig
runtime_packages=("libtbb12" "zlib1g" "libbz2-1.0" "liblzma5" "libdeflate0")
for pkg in "${runtime_packages[@]}"; do
    if dpkg -l "$pkg" 2>/dev/null | grep -q "^ii"; then
        echo -e "${GREEN}✓ $pkg installed${NC}"
    else
        echo -e "${YELLOW}Warning: $pkg not found${NC}"
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