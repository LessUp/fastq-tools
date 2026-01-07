# 构建阶段
FROM gcc:15.2-bookworm AS build

# 安装构建依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    ninja-build \
    pkg-config \
    git \
    wget \
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

RUN wget -q https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 21 && \
    apt-get update && \
    apt-get install -y \
    clang-21 \
    lld-21 \
    llvm-21 && \
    rm -f llvm.sh && \
    rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-21 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-21 100

# 安装Conan (使用 --break-system-packages 绕过 PEP 668 限制)
RUN pip3 install --no-cache-dir --break-system-packages conan==2.19.0

# 设置工作目录
WORKDIR /workspace

# 复制源代码
COPY . .

# 配置构建环境
ENV CC=clang
ENV CXX=clang++
ENV CONAN_HOME=/root/.conan2
ENV CCACHE_DIR=/root/.ccache
ENV PATH=/usr/lib/ccache:$PATH

# 安装依赖
RUN conan profile detect --force
RUN conan install config/dependencies/ --build=missing -s build_type=Release

# 构建项目
RUN cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_COVERAGE=OFF \
    -DBUILD_TESTING=OFF

RUN ninja -C build

# 生产阶段
FROM debian:12-slim AS production

# 安装运行时依赖
RUN apt-get update && apt-get install -y \
    ca-certificates \
    libtbb12 \
    zlib1g \
    libbz2-1.0 \
    liblzma5 \
    libdeflate0 \
    && rm -rf /var/lib/apt/lists/*

# 创建非root用户
ARG USERNAME=fastqtools
ARG USER_UID=1000
ARG USER_GID=1000

RUN groupadd -g $USER_GID $USERNAME \
    && useradd -m -s /bin/bash -u $USER_UID -g $USER_GID $USERNAME

# 设置工作目录
WORKDIR /app

# 从构建阶段复制二进制文件
COPY --from=build /workspace/build/FastQTools /usr/local/bin/
COPY --from=build /workspace/build/lib* /usr/local/lib/ 2>/dev/null || true

# 创建符号链接
RUN ln -sf /usr/local/bin/FastQTools /usr/local/bin/fastqtools

# 创建数据目录
RUN mkdir -p /app/data /app/output && \
    chown -R $USERNAME:$USERNAME /app

# 切换到非root用户
USER $USERNAME

# 设置环境变量
ENV PATH=/usr/local/bin:$PATH
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
ENV FASTQTOOLS_DATA_DIR=/app/data
ENV FASTQTOOLS_OUTPUT_DIR=/app/output

# 暴露端口（如果需要网络服务）
EXPOSE 8080

# 健康检查
HEALTHCHECK --interval=30s --timeout=5s --start-period=5s --retries=3 \
    CMD fastqtools --help || exit 1

CMD ["fastqtools", "--help"]