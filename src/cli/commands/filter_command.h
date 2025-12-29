/**
 * @file filter_command.h
 * @brief 定义了 'filter' 子命令。
 * @author LessUp
 * @version 1.0
 * @date 2025-07-31
 * @copyright Copyright (c) 2025 LessUp
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "commands/command_interface.h"
#include <fqtools/fq.h>

namespace fq::cli::commands {

/**
 * @brief 实现了用于过滤和修剪 FastQ 文件的 'filter' 命令。
 */
class FilterCommand : public fq::cli::CommandInterface {
public:
    /**
     * @brief 构造函数
     * @details 创建过滤器命令实例，初始化内部配置
     *
     * @post 过滤器命令实例被创建并初始化
     */
    FilterCommand();

    /**
     * @brief 析构函数
     * @details 清理过滤器命令实例的资源
     *
     * @post 所有资源被正确释放
     */
    ~FilterCommand();

    /**
     * @brief 执行过滤命令
     * @details 解析命令行参数并执行 FastQ 文件过滤操作
     *
     * @param argc 参数数量
     * @param argv 参数数组
     * @return 执行成功返回0，失败返回非0值
     */
    auto execute(int argc, char* argv[]) -> int override;

    /**
     * @brief 获取命令名称
     * @return 返回命令名称字符串 "filter"
     */
    [[nodiscard]] auto getName() const -> std::string override;

    /**
     * @brief 获取命令描述
     * @return 返回命令功能的描述字符串
     */
    [[nodiscard]] auto getDescription() const -> std::string override;

private:
    struct Config;
    std::unique_ptr<Config> config_;
    // Depend on the INTERFACE
    std::unique_ptr<fq::processing::ProcessingPipelineInterface> pipeline_;
};

}  // namespace fq::cli::commands
