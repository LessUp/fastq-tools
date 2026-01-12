#!/usr/bin/env bash
set -euo pipefail

if ! command -v sshd >/dev/null 2>&1; then
  exit 0
fi

AUTHORIZED_KEYS_FALLBACK="/home/developer/.ssh_authorized_keys"

CONF_MAIN="/etc/ssh/sshd_config"
CONF_D="/etc/ssh/sshd_config.d"
CONF_SNIPPET="${CONF_D}/fastqtools-devcontainer.conf"

sudo mkdir -p /var/run/sshd
sudo ssh-keygen -A

if sudo test -d "${CONF_D}" && sudo grep -Fq "Include /etc/ssh/sshd_config.d/*.conf" "${CONF_MAIN}"; then
  sudo tee "${CONF_SNIPPET}" >/dev/null <<'EOF'
Port 2222
PasswordAuthentication no
KbdInteractiveAuthentication no
PubkeyAuthentication yes
PermitRootLogin no
AuthorizedKeysFile /home/developer/.ssh_authorized_keys
AllowUsers developer
EOF
else
  MARK_START="# FASTQTOOLS_DEVCONTAINER_SSHD_CONFIG_START"
  MARK_END="# FASTQTOOLS_DEVCONTAINER_SSHD_CONFIG_END"

  sudo sed -i "/^${MARK_START}$/,/^${MARK_END}$/d" "${CONF_MAIN}"

  sudo tee -a "${CONF_MAIN}" >/dev/null <<EOF
${MARK_START}
Port 2222
PasswordAuthentication no
KbdInteractiveAuthentication no
PubkeyAuthentication yes
PermitRootLogin no
AuthorizedKeysFile /home/developer/.ssh_authorized_keys
AllowUsers developer
${MARK_END}
EOF
fi

if sudo test -f "${AUTHORIZED_KEYS_FALLBACK}"; then
  exit 0
fi

if test -f /home/developer/.ssh/authorized_keys; then
  sudo install -o developer -g developer -m 0600 /home/developer/.ssh/authorized_keys "${AUTHORIZED_KEYS_FALLBACK}"
  exit 0
fi

PUBKEY=""
for candidate in /home/developer/.ssh/id_ed25519.pub /home/developer/.ssh/id_rsa.pub /home/developer/.ssh/id_ecdsa.pub /home/developer/.ssh/id_dsa.pub; do
  if test -f "${candidate}"; then
    PUBKEY="${candidate}"
    break
  fi
done

if test -n "${PUBKEY}"; then
  sudo install -o developer -g developer -m 0600 "${PUBKEY}" "${AUTHORIZED_KEYS_FALLBACK}"
else
  sudo install -o developer -g developer -m 0600 /dev/null "${AUTHORIZED_KEYS_FALLBACK}"
fi
