# =============================================================================
# FastQTools Production Build
# =============================================================================
# 使用 GCC 15 + libstdc++ 进行生产构建
# 这是最新、性能最好的配置
# =============================================================================

# -----------------------------------------------------------------------------
# 构建阶段
# -----------------------------------------------------------------------------
FROM gcc:15.2-bookworm AS build

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

# 安装构建依赖
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ninja-build \
    pkg-config \
    git \
    wget \
    ca-certificates \
    tar \
    xz-utils \
    gnupg \
    lsb-release \
    software-properties-common \
    python3 \
    python3-pip \
    ccache \
    libdeflate-dev \
    && rm -rf /var/lib/apt/lists/*

# CMake 4.x (最新版本)
RUN CMAKE_VERSION=4.0.2 && \
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

# Conan 2.x (最新版本)
RUN pip3 install --no-cache-dir --break-system-packages conan==2.14.0

# 设置编译器
ENV CC=gcc
ENV CXX=g++
ENV CONAN_HOME=/root/.conan2
ENV CCACHE_DIR=/root/.ccache
ENV PATH=/usr/lib/ccache:$PATH

WORKDIR /workspace
COPY . .

# 安装依赖并构建
RUN conan profile detect --force
RUN conan install config/dependencies/ --build=missing -s build_type=Release -of=build/gcc-release

RUN cmake -B build/gcc-release -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=build/gcc-release/conan_toolchain.cmake \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DENABLE_COVERAGE=OFF \
    -DBUILD_TESTING=OFF

RUN ninja -C build/gcc-release

# -----------------------------------------------------------------------------
# 生产阶段
# -----------------------------------------------------------------------------
FROM debian:bookworm-slim AS production

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    libtbb12 \
    zlib1g \
    libbz2-1.0 \
    liblzma5 \
    libdeflate0 \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# 非 root 用户
ARG USERNAME=fastqtools
ARG USER_UID=1000
ARG USER_GID=1000

RUN groupadd -g $USER_GID $USERNAME \
    && useradd -m -s /bin/bash -u $USER_UID -g $USER_GID $USERNAME

WORKDIR /app

# 复制构建产物
COPY --from=build /workspace/build/gcc-release/FastQTools /usr/local/bin/
RUN ln -sf /usr/local/bin/FastQTools /usr/local/bin/fastqtools

# 数据目录
RUN mkdir -p /app/data /app/output && \
    chown -R $USERNAME:$USERNAME /app

USER $USERNAME

ENV PATH=/usr/local/bin:$PATH
ENV FASTQTOOLS_DATA_DIR=/app/data
ENV FASTQTOOLS_OUTPUT_DIR=/app/output

HEALTHCHECK --interval=30s --timeout=5s --start-period=5s --retries=3 \
    CMD fastqtools --help || exit 1

CMD ["fastqtools", "--help"]
