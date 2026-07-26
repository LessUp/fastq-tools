/**
 * @file test_command_registry.cpp
 * @brief CommandRegistry 单元测试
 * @details 覆盖命令注册、查找、执行、帮助输出
 */

#include <memory>
#include <sstream>
#include <string>

#include "command_registry.h"

#include "commands/command_interface.h"
#include <gtest/gtest.h>

using namespace fq::cli;

namespace {

/// 测试用桩命令
class StubCommand : public CommandInterface {
public:
    StubCommand(std::string name, std::string desc, int exitCode = 0)
        : name_(std::move(name)), desc_(std::move(desc)), exitCode_(exitCode) {}

    auto execute(int /*argc*/, char* /*argv*/[]) -> int override {
        ++callCount_;
        return exitCode_;
    }

    [[nodiscard]] auto getName() const -> std::string override {
        return name_;
    }
    [[nodiscard]] auto getDescription() const -> std::string override {
        return desc_;
    }

    int callCount_ = 0;

private:
    std::string name_;
    std::string desc_;
    int exitCode_;
};

}  // namespace

// ============================================================================
// CommandRegistry 测试
// ============================================================================

TEST(CommandRegistryTest, RegisterAndFindCommand) {
    CommandRegistry registry;
    registry.registerCommand("stub", std::make_unique<StubCommand>("stub", "stub desc"));

    auto* cmd = registry.findCommand("stub");
    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->getName(), "stub");
    EXPECT_EQ(cmd->getDescription(), "stub desc");
}

TEST(CommandRegistryTest, FindUnregisteredCommandReturnsNull) {
    CommandRegistry registry;
    EXPECT_EQ(registry.findCommand("nonexistent"), nullptr);
}

TEST(CommandRegistryTest, HasCommandReturnsCorrectStatus) {
    CommandRegistry registry;
    registry.registerCommand("stub", std::make_unique<StubCommand>("stub", "desc"));

    EXPECT_TRUE(registry.hasCommand("stub"));
    EXPECT_FALSE(registry.hasCommand("nonexistent"));
}

TEST(CommandRegistryTest, ExecuteCommandInvokesHandler) {
    CommandRegistry registry;
    auto stub = std::make_unique<StubCommand>("stub", "desc", 42);
    auto* rawStub = stub.get();
    registry.registerCommand("stub", std::move(stub));

    char* argv[] = {const_cast<char*>("prog")};
    int rc = registry.executeCommand("stub", 1, argv);
    EXPECT_EQ(rc, 42);
    EXPECT_EQ(rawStub->callCount_, 1);
}

TEST(CommandRegistryTest, ExecuteUnknownCommandReturnsNegative) {
    CommandRegistry registry;
    char* argv[] = {const_cast<char*>("prog")};
    int rc = registry.executeCommand("nonexistent", 1, argv);
    EXPECT_LT(rc, 0);
}

TEST(CommandRegistryTest, PrintHelpListsCommands) {
    CommandRegistry registry;
    registry.registerCommand("stub", std::make_unique<StubCommand>("stub", "stub description"));

    std::ostringstream oss;
    registry.printHelp(oss);
    std::string help = oss.str();

    EXPECT_NE(help.find("stub"), std::string::npos);
    EXPECT_NE(help.find("stub description"), std::string::npos);
    EXPECT_NE(help.find("Usage:"), std::string::npos);
}

TEST(CommandRegistryTest, CreateDefaultRegistryHasStatAndFilter) {
    auto registry = createDefaultCommandRegistry();
    EXPECT_TRUE(registry->hasCommand("stat"));
    EXPECT_TRUE(registry->hasCommand("filter"));
}
