/**
 * @file command_registry.cpp
 * @brief 命令注册表实现
 *
 * @author LessUp
 * @date 2026-05-08
 * @version 2.0
 * @copyright (c) 2026 LessUp. All rights reserved.
 */

#include "command_registry.h"

#include "commands/filter_command.h"
#include "commands/stat_command.h"

namespace fq::cli {

auto createDefaultCommandRegistry() -> std::unique_ptr<CommandRegistry> {
    auto registry = std::make_unique<CommandRegistry>();
    registry->registerCommand("stat", std::make_unique<commands::StatCommand>());
    registry->registerCommand("filter", std::make_unique<commands::FilterCommand>());
    return registry;
}

}  // namespace fq::cli
