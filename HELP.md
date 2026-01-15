# 手动启动容器
docker start fastqtools-dev

# 手动启动 SSHD
docker exec fastqtools-dev bash /workspace/.devcontainer/start-sshd.sh

# 然后就可以 SSH 连接了
ssh -p 2222 developer@localhost
