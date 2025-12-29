# 2025-12-22 脚本精简重构

## 摘要
- 覆盖率流程并入 `scripts/test.sh`，新增 lcov/genhtml 全量报告逻辑；CI 覆盖率改用 test.sh。
- 代码质量脚本统一到 `scripts/lint.sh`，支持 lint/lint-fix/format/format-check 子命令；`format.sh` 废弃。
- `coverage.sh` 废弃，提示改用 `test.sh -C`。
- 同步更新开发文档与编码规范，指向新入口。

## 变更详情
- `scripts/test.sh`：添加覆盖率构建/采集/过滤/HTML 报告与摘要；缺省覆盖率目录自动配置；保留 ctest 参数与重复执行能力。
- `scripts/lint.sh`：改为统一子命令入口，收敛 clang-tidy 与 clang-format（修复/检查）；支持指定 build-dir/style；遍历 src/app/tests。
- 删除 `scripts/coverage.sh`、`scripts/format.sh`；覆盖率/格式化改走 test.sh -C 与 lint.sh 子命令。
- CI `coverage` job 改为调用 `./scripts/test.sh -c clang -t Coverage -C -j 2`。
- 文档 `docs/development/build.md` 与 `docs/development/coding_standards.md` 更新引用。
- 统一脚本命名为 snake_case：`install_deps.sh`（原 install_dependencies.sh）、`package_release.sh`（原 package.sh）、`devcontainer_setup.sh`（原 dev-container-setup.sh）、示例 `basic_stats.sh`（原 basic_stats.sh）；对应文档、DevContainer、示例 README 全部同步。

## 兼容性
- 覆盖率与格式化旧入口不再执行原逻辑，请改用 `test.sh -C` 与 `lint.sh format|format-check`。

## 后续建议
- 根据需要在 README/CONTRIBUTING 添加新的脚本用法示例。
- 若本地常用 gcc 覆盖率，可在 CI matrix 补充 gcc Coverage 组合（当前仅 clang）。
