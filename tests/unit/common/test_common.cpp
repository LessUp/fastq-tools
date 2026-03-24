#include "fqtools/common/common.h"

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>
namespace fq::common {

// --- Timer Tests ---
TEST(CommonModuleTest, Timer_BasicFunctionality) {
    Timer timer("test_timer");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto elapsed = timer.elapsed();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    EXPECT_GE(elapsedMs, 90);
    EXPECT_LE(elapsedMs, 200);

    // Test that report() doesn't crash
    EXPECT_NO_THROW(timer.report());
}

// --- StringUtils Tests ---
TEST(CommonModuleTest, StringUtils_SplitBasic) {
    auto result = split("a,b,c", ',');
    std::vector<std::string> expected = {"a", "b", "c"};
    ASSERT_EQ(result, expected);
}

TEST(CommonModuleTest, StringUtils_SplitWithEmptyParts) {
    // Note: The new implementation might handle this differently.
    // Assuming it skips empty parts based on the previous test logic.
    auto result = split("a,,c", ',');
    std::vector<std::string> expected = {"a", "c"};
    ASSERT_EQ(result, expected);
}

TEST(CommonModuleTest, StringUtils_TrimBasic) {
    EXPECT_EQ(trim("  hello  "), "hello");
    EXPECT_EQ(trim("\t\nhello\r\n"), "hello");
    EXPECT_EQ(trim("hello"), "hello");
}

TEST(CommonModuleTest, StringUtils_JoinBasic) {
    std::vector<std::string> parts = {"a", "b", "c"};
    EXPECT_EQ(join(parts, ","), "a,b,c");
}

// --- IDGenerator Tests ---
TEST(CommonModuleTest, IDGenerator_IncrementsMonotonically) {
    IDGenerator::reset();
    const auto first = IDGenerator::nextId();
    const auto second = IDGenerator::nextId();

    EXPECT_EQ(first, 1);
    EXPECT_EQ(second, 2);
}

TEST(CommonModuleTest, IDGenerator_ResetRestoresInitialValue) {
    static_cast<void>(IDGenerator::nextId());
    IDGenerator::reset();

    EXPECT_EQ(IDGenerator::nextId(), 1);
}

}  // namespace fq::common
