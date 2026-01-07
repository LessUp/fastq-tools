# Dockerfile for FastQTools
# This Dockerfile creates a consistent build and runtime environment.

# ---- Builder Stage ----
# Uses Ubuntu 22.04 as the base for the build environment.
FROM ubuntu:22.04 AS builder

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# 1. Install base dependencies and GCC
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    wget \
    tar \
    xz-utils \
    gnupg \
    software-properties-common \
    python3-pip \
    ninja-build

RUN CMAKE_VERSION=4.2.1 && \
    ARCH="$(dpkg --print-architecture)" && \
    case "$ARCH" in \
        amd64) CMAKE_ARCH="x86_64" ;; \
        arm64) CMAKE_ARCH="aarch64" ;; \
        *) echo "Unsupported architecture: $ARCH"; exit 1 ;; \
    esac && \
    wget -q "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-${CMAKE_ARCH}.tar.gz" -O /tmp/cmake.tar.gz && \
    mkdir -p /opt && \
    tar -C /opt -xzf /tmp/cmake.tar.gz && \
    ln -sf "/opt/cmake-${CMAKE_VERSION}-linux-${CMAKE_ARCH}/bin/cmake" /usr/local/bin/cmake && \
    ln -sf "/opt/cmake-${CMAKE_VERSION}-linux-${CMAKE_ARCH}/bin/ctest" /usr/local/bin/ctest && \
    ln -sf "/opt/cmake-${CMAKE_VERSION}-linux-${CMAKE_ARCH}/bin/cpack" /usr/local/bin/cpack && \
    rm -f /tmp/cmake.tar.gz

# 2. Install specific Clang version (21)
RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 21 && \
    apt-get install -y clang-21 clang-tidy-21 clang-format-21 lld-21 llvm-21 && \
    rm -f llvm.sh && \
    rm -rf /var/lib/apt/lists/*

# 3. Install specific Conan version
RUN pip3 install conan==2.19.0

# 4. Copy project source code
WORKDIR /app
COPY . .

# 5. Run Conan to install project dependencies
# We use the system's default GCC (which should be >= 15.1 on modern systems)
# and the installed Clang 19. Here we configure Conan to use GCC.
RUN conan profile detect --force && \
    conan install config/dependencies/ --output-folder=build/conan-release --build=missing -s build_type=Release

# 6. Configure and build the project with CMake
RUN cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=build/conan-release/conan_toolchain.cmake \
    -G "Ninja"
RUN cmake --build build

# ---- Final Stage ----
# Uses a slim base image for a smaller footprint.
FROM ubuntu:22.04

# Copy the compiled executable from the builder stage
COPY --from=builder /app/build/FastQTools /usr/local/bin/FastQTools

# Set the entrypoint for the container
ENTRYPOINT ["/usr/local/bin/FastQTools"]
CMD ["--help"]