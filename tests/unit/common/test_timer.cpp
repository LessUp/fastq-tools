#include "fqtools/common/common.h"

#include <chrono>
#include <thread>

#include "test_helpers.h"

#include <gtest/gtest.h>

namespace fq::test {

class TimerTest : public FastQToolsTest {};

// Tests the basic functionality of the timer measuring an interval.
TEST_F(TimerTest, BasicTiming) {
    fq::common::Timer timer("test_timer");

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto elapsedNs = timer.elapsed();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedNs);

    // Check if the elapsed time is within a reasonable range (e.g., >90ms)
    EXPECT_GT(elapsedMs.count(), 90);
    EXPECT_LT(elapsedMs.count(), 200);  // Allow for system scheduling delays
}

// Tests that the timer name is correctly stored.
TEST_F(TimerTest, NamedTimer) {
    const std::string timerName = "named_test_timer";
    fq::common::Timer timer(timerName);

    // The main test is that this compiles and runs without issue.
    // A more advanced test could involve redirecting stdout and checking
    // the output of timer.report(), but this is sufficient for now.
    SUCCEED();
}

// Tests timing of sequential operations by using two separate timers.
TEST_F(TimerTest, SequentialTiming) {
    long long firstDurationMs = 0;
    long long secondDurationMs = 0;

    {
        fq::common::Timer firstTimer("first_interval");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        firstDurationMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(firstTimer.elapsed()).count();
    }  // first_timer is destroyed here

    {
        fq::common::Timer secondTimer("second_interval");
        std::this_thread::sleep_for(std::chrono::milliseconds(70));
        secondDurationMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(secondTimer.elapsed()).count();
    }  // second_timer is destroyed here

    EXPECT_GT(firstDurationMs, 45);
    EXPECT_LT(firstDurationMs, 100);

    EXPECT_GT(secondDurationMs, 65);
    EXPECT_LT(secondDurationMs, 120);
}

}  // namespace fq::test
