# Dockerfile for FastQTools
# This Dockerfile creates a consistent build and runtime environment.

# ---- Builder Stage ----
# Uses Ubuntu 22.04 as the base for the build environment.
FROM ubuntu:22.04 AS builder

ARG HTTP_PROXY
ARG HTTPS_PROXY
ARG ALL_PROXY
ARG NO_PROXY

ENV HTTP_PROXY=${HTTP_PROXY}
ENV HTTPS_PROXY=${HTTPS_PROXY}
ENV ALL_PROXY=${ALL_PROXY}
ENV NO_PROXY=${NO_PROXY}
ENV http_proxy=${HTTP_PROXY}
ENV https_proxy=${HTTPS_PROXY}
ENV all_proxy=${ALL_PROXY}
ENV no_proxy=${NO_PROXY}

RUN set -eux; \
    if [ -n "${HTTP_PROXY:-}" ] || [ -n "${HTTPS_PROXY:-}" ]; then \
        : > /etc/apt/apt.conf.d/99proxy; \
        if [ -n "${HTTP_PROXY:-}" ]; then echo "Acquire::http::Proxy \"${HTTP_PROXY}\";" >> /etc/apt/apt.conf.d/99proxy; fi; \
        if [ -n "${HTTPS_PROXY:-}" ]; then echo "Acquire::https::Proxy \"${HTTPS_PROXY}\";" >> /etc/apt/apt.conf.d/99proxy; fi; \
    fi

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
RUN pip3 install conan==2.24.0

ENV CC=/usr/bin/gcc
ENV CXX=/usr/bin/g++

# 4. Copy project source code
WORKDIR /app
COPY . .

# 5. Run Conan to install project dependencies
# We use the system's default GCC (which should be >= 15.1 on modern systems)
# and the installed Clang 19. Here we configure Conan to use GCC.
RUN conan profile detect --force && \
    conan install config/dependencies/ --output-folder=build/conan-release --build=missing -s build_type=Release -s compiler.cppstd=20

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