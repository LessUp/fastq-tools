# DevContainer Development Environment

This document explains FastQTools development environment setup in DevContainer, including VS Code native support and Windsurf/Cursor SSH connection solutions.

---

## Applicable Scenarios

- Windows 11/10 + Docker Desktop (WSL2 backend)
- VS Code + Dev Containers extension
- Windsurf / Cursor (via SSH connection)

---

## Environment Preparation

### 1. Repository Must be Opened in WSL2

Container bind mounts are more stable and performant under WSL2 Linux filesystem:

```bash
# Clone repository in WSL2
mkdir -p ~/lessup
cd ~/lessup
git clone git@github.com:<org>/<repo>.git fastq-tools
```

In VS Code, open `~/lessup/fastq-tools` through **Remote - WSL**, then execute **Reopen in Container**.

### 2. SSH Agent Configuration

Container accesses private repositories through SSH Agent forwarding (without copying private keys into container):

```bash
# Start agent and load key in WSL
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519

# Verify
ssh-add -l
ssh -T git@github.com
```

### 3. Git Identity Configuration

Container automatically mounts `~/.gitconfig` from host WSL (read-only):

```bash
git config --global user.name "Your Name"
git config --global user.email "you@example.com"
```

---

## VS Code DevContainer

Two configuration options available:

1. **Full configuration** (`.devcontainer/devcontainer.json`): Uses docker-compose
2. **Simple configuration** (`.devcontainer/devcontainer.simple.json`): Single Dockerfile

Both provide: Clang/LLVM 21, CMake, Ninja, Conan, VS Code C++ extensions.

### Daily Development

```bash
# Execute inside container
./scripts/core/build
./scripts/core/test
./scripts/core/lint
```

---

## Windsurf / Cursor SSH Solution

Windsurf/Cursor doesn't fully support "Reopen in Container". The solution is to start `sshd` inside DevContainer and connect directly via SSH.

### Implementation

Uses DevContainer lifecycle hooks + sshd self-daemonize:

- `postCreateCommand`: Run `setup-sshd.sh` for one-time configuration
- `postStartCommand`: Run `start-sshd.sh` to launch sshd on each start

**sshd configuration**: Port `2222`, password login disabled, root login disabled, restricted to user `developer`.

### Connection Steps

#### Local Development

1. Start container with VS Code Dev Containers (or `devcontainer` CLI)
2. Confirm `postStartCommand` has executed (sshd starts automatically)
3. Test connection:

```bash
ssh -p 2222 developer@127.0.0.1
```

4. Use Remote-SSH in Windsurf/Cursor:
   - Host: `127.0.0.1`
   - Port: `2222`
   - User: `developer`
   - Working directory: `/workspace`

#### Remote Host

1. Start DevContainer on remote host
2. Set `FASTQTOOLS_SSH_BIND=0.0.0.0` (note firewall)
3. Connect directly from local Windsurf/Cursor to `remote-host:2222`

### Port Configuration

Customize via environment variables:

- `FASTQTOOLS_SSH_BIND` (default `127.0.0.1`)
- `FASTQTOOLS_SSH_PORT` (default `2222`)

---

## Quick Command Reference

```bash
# Manually start container
docker start fastqtools-dev

# Manually start sshd
docker exec fastqtools-dev bash /workspace/.devcontainer/scripts/start-sshd.sh

# SSH connection
ssh -p 2222 developer@localhost
```

---

## Frequently Asked Questions

### SSH Agent Not Detected

```bash
# Execute in WSL
eval "$(ssh-agent -s)" && ssh-add

# Then in VS Code Rebuild and Reopen in Container
```

### Git Permission denied (publickey)

```bash
ssh-add -l          # Confirm key is loaded
ssh -T git@github.com  # Test connection
```

### SSH Connection refused

- Container not started or `postStartCommand` not executed
- Check port mapping: `docker port fastqtools-dev`

### SSH Permission denied (publickey)

- Check if `/home/developer/.ssh_authorized_keys` exists in container and permissions are `0600`

### Slow Performance

- Confirm repository is located in WSL2 Linux filesystem (not under Windows drive)
- Cache uses named volumes or user directory

---

## Security Recommendations

- sshd only binds to `127.0.0.1` by default
- When exposing externally, use with firewall
- Password login disabled, public key authentication only
