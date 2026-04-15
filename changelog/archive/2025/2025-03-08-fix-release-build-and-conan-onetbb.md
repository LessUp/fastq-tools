# fix(build): 修复 release-build.sh Clang 安装失败及 onetbb Conan 构建错误

## 变更

### 修复 1: `lsb_release: command not found`

- **文件**: `scripts/ci/release-build.sh`
- **原因**: `install_deps_debian()` 缺少 `lsb-release`、`gnupg`、`software-properties-common` 包，
  导致后续 `llvm.sh` 安装 Clang 21 时因找不到 `lsb_release` 命令而失败（退出码 127）。
- **修复**: 在 Debian 构建依赖列表中添加这三个包。

### 修复 2: `onetbb/2022.3.0 Invalid: hwloc shared=True`

- **文件**: `config/dependencies/conanfile.py`、`conanfile.py`
- **原因**: onetbb 2022.3.0 要求其依赖 hwloc 以 `shared=True` 模式构建，
  但 Conan 默认不设置此选项，导致二进制包校验失败（Invalid）。
- **修复**: 在两个 conanfile.py 的 `configure()` 方法中添加 `self.options["hwloc"].shared = True`。
