#!/usr/bin/env bash
set -euo pipefail

compose() {
  if docker compose version >/dev/null 2>&1; then
    docker compose "$@"
    return
  fi
  if command -v docker-compose >/dev/null 2>&1; then
    docker-compose "$@"
    return
  fi
  echo "ERROR: 未找到 docker compose 或 docker-compose" >&2
  exit 127
}

usage() {
  cat <<'EOF'
用法:
  docker/start_devcontainer.sh [--bind <ip>] [--port <port>] [--authorized-keys <file>] [--no-build]

说明:
  在远程服务器上手动启动 FastQTools dev 容器，并在容器内启动 sshd(2222)，用于 Windsurf/Cursor/Remote-SSH 连接。

参数:
  --bind <ip>               宿主机端口绑定地址（默认: 127.0.0.1；异机访问可用 0.0.0.0）
  --port <port>             宿主机端口（默认: 2222）
  --authorized-keys <file>  要注入到容器的 authorized_keys 文件（默认: $HOME/.ssh/authorized_keys）
  --no-build                跳过 docker compose build
EOF
}

BIND="${FASTQTOOLS_SSH_BIND:-127.0.0.1}"
PORT="${FASTQTOOLS_SSH_PORT:-2222}"
AUTHORIZED_KEYS_FILE="${FASTQTOOLS_AUTHORIZED_KEYS_FILE:-${HOME}/.ssh/authorized_keys}"
NO_BUILD=0

while [ $# -gt 0 ]; do
  case "$1" in
    --bind)
      BIND="$2"; shift 2 ;;
    --port)
      PORT="$2"; shift 2 ;;
    --authorized-keys)
      AUTHORIZED_KEYS_FILE="$2"; shift 2 ;;
    --no-build)
      NO_BUILD=1; shift 1 ;;
    -h|--help)
      usage; exit 0 ;;
    *)
      echo "未知参数: $1" >&2
      usage
      exit 2 ;;
  esac
done

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
COMPOSE_FILE="${REPO_ROOT}/docker/docker-compose.yml"

export FASTQTOOLS_SSH_BIND="${BIND}"
export FASTQTOOLS_SSH_PORT="${PORT}"

if [ "${NO_BUILD}" -eq 0 ]; then
  compose -f "${COMPOSE_FILE}" build dev
fi

compose -f "${COMPOSE_FILE}" up -d dev

if [ -f "${AUTHORIZED_KEYS_FILE}" ]; then
  compose -f "${COMPOSE_FILE}" exec -T -u developer dev bash -lc 'mkdir -p ~/.ssh && chmod 700 ~/.ssh'
  compose -f "${COMPOSE_FILE}" exec -T -u developer dev bash -lc 'cat > ~/.ssh/authorized_keys && chmod 600 ~/.ssh/authorized_keys' <"${AUTHORIZED_KEYS_FILE}"
else
  echo "WARNING: 未找到 authorized_keys 文件: ${AUTHORIZED_KEYS_FILE}（将启动 sshd，但可能无法公钥登录）" >&2
fi

compose -f "${COMPOSE_FILE}" exec -T -u developer dev bash -lc 'bash /workspace/.devcontainer/start-sshd.sh'

echo "OK: devcontainer 已启动"
echo "- SSH bind: ${BIND}:${PORT}"
echo "- 连接示例: ssh -p ${PORT} developer@<服务器IP>"
