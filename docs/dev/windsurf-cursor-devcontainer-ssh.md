# Windsurf/Cursor 使用 Devcontainer 的 SSH 方案（2026-01-09）

## 背景与问题

- VS Code 支持 Dev Containers 的“Reopen in Container”，可以在容器内完成构建、调试与工具链配置。
- Windsurf/Cursor 在部分工作流里（尤其是“先 Remote-SSH 到主机，再在该主机上 Reopen in Container”的嵌套模式）不如 VS Code 顺畅。
- Windsurf 官方也指出：若需要在 devcontainer 中继续通过 SSH 使用其 Remote-SSH 能力，建议在 devcontainer 内自行启动 `sshd`。

目标：

- 保持 VS Code Dev Containers 体验不变。
- 让 Windsurf/Cursor 能通过 SSH 直接连到 devcontainer 容器，获得同一套工具链与环境。

## 你提供方案的批判性分析

### 方案 A：在终端里 `sshd -D` + tmux/保持前台

- 优点：实现快、容易验证。
- 缺点：
  - 需要人为保持一个前台进程（或额外维护 tmux/session）。
  - 容器重启后容易遗忘恢复。
  - 把“开发环境可用性”绑定到某个终端会话，稳定性差。

### 方案 B：引入 supervisor/systemd 管理 sshd

- 优点：更接近传统服务器守护进程管理。
- 缺点：
  - devcontainer 镜像通常不以 systemd 为目标；引入 supervisor 会增加维护成本。
  - 额外的 init/监督体系容易与 Dev Containers 的启动/attach 机制发生耦合问题。

### 方案 C（选型）：devcontainer lifecycle hooks + sshd 自行 daemonize

- 优点：
  - 不引入 supervisor。
  - 利用 `postCreateCommand` 做一次性配置，`postStartCommand` 保证每次启动/attach 都能拉起 sshd。
  - 不依赖手工保持前台会话。
- 关键注意点（常见坑）：
  - devcontainer 常把宿主机 `~/.ssh` 只读挂载进容器，导致容器内不能写 `authorized_keys`。
  - 即便 `authorized_keys` 存在，若属主/权限不符合 sshd 要求，`sshd` 会忽略它，表现为一直 `Permission denied (publickey)`。

因此本仓库落地时额外做了：

- `sshd` 仅使用容器内可写的 `AuthorizedKeysFile /home/developer/.ssh_authorized_keys`（权限固定为 0600）。
- 在首次创建时，自动从挂载的 `/home/developer/.ssh/authorized_keys` 或 `id_*.pub` 复制填充该文件，降低手工配置成本。

## 仓库落地实现（本次变更）

- `docker/Dockerfile.dev`
  - 安装 `openssh-server`/`openssh-client`/`procps`。
- `.devcontainer/setup-sshd.sh`
  - 幂等配置 `sshd_config`（优先写入 `sshd_config.d`）。
  - 设置：端口 `2222`、禁用密码登录、禁用 root 登录、限制用户 `developer`。
  - 生成 host keys、准备 `/var/run/sshd`。
  - 处理 `authorized_keys` fallback。
- `.devcontainer/start-sshd.sh`
  - 每次启动时检测 `sshd` 是否已运行，未运行则启动（daemonize）。
- `.devcontainer/devcontainer.json` / `.devcontainer/devcontainer.simple.json`
  - 在原有 `postCreateCommand` 末尾追加 `setup-sshd.sh`。
  - 在 `postStartCommand` / `postAttachCommand` 末尾追加 `start-sshd.sh`。
  - `devcontainer.simple.json` 额外通过 `runArgs` 发布端口：`127.0.0.1:2222 -> 2222`，便于本机直接 SSH 到容器。
- `docker/docker-compose.yml`
  - 为 dev 服务增加端口映射：`2222`（默认仅绑定本机回环）。
  - 支持环境变量：
    - `FASTQTOOLS_SSH_BIND`（默认 `127.0.0.1`）
    - `FASTQTOOLS_SSH_PORT`（默认 `2222`）

## 使用方式

### 场景 1：本机开发（Windsurf/Cursor 直接连本机容器）

1. 用 VS Code Dev Containers 正常打开一次（或使用 devcontainer CLI）以创建并启动容器。
2. 确认 dev 容器在跑，并且 `postStartCommand` 已执行（会自动启动 sshd）。
3. 在宿主机测试：

```bash
ssh -p 2222 developer@127.0.0.1
```

4. 在 Windsurf/Cursor：使用其 Remote-SSH 功能连接到：

- Host: `127.0.0.1`
- Port: `2222`
- User: `developer`

连上后打开目录：`/workspace`。

补充说明：

- 如果你使用 `devcontainer.json`（compose 模式），可通过 `FASTQTOOLS_SSH_BIND` / `FASTQTOOLS_SSH_PORT` 调整宿主机端口映射。
- 如果你使用 `devcontainer.simple.json`（非 compose 模式），端口发布由 `runArgs` 指定；如需改端口，请直接修改该文件中的 `-p 127.0.0.1:2222:2222`。

### 场景 2：远程主机开发（Windsurf/Cursor 直连远程主机上的容器）

1. 在远程主机上创建/启动 devcontainer（可通过 VS Code、devcontainer CLI 或你已有的自动化方式）。
2. 若需要从外网直连容器（请先确认防火墙与安全策略）：

- 设置 `FASTQTOOLS_SSH_BIND=0.0.0.0`
- 如端口冲突可设置 `FASTQTOOLS_SSH_PORT=xxxx`

3. 在本地 Windsurf/Cursor：Remote-SSH 直接连接到 `remote-host:FASTQTOOLS_SSH_PORT`（用户 `developer`）。

## 排错

- `Connection refused`
  - 容器未启动或未执行 `postStartCommand`。
  - 端口映射未生效（确认 compose 端口映射与绑定地址）。
- `Permission denied (publickey)`
  - 检查宿主机是否有可用 key。
  - 进入容器确认 `/home/developer/.ssh_authorized_keys` 是否存在且权限为 0600。
- 端口占用
  - 通过 `FASTQTOOLS_SSH_PORT` 改端口映射。

## 安全建议

- 默认只绑定 `127.0.0.1`，避免意外暴露。
- 如需对外开放：务必配合防火墙/安全组，仅允许可信来源访问。
- 已禁用密码登录，推荐仅使用公钥认证。
