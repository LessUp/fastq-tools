#include <gtest/gtest.h>
#include <oneapi/tbb/global_control.h>

namespace fq::test {

/// 在所有测试结束后显式终止 TBB 调度器
/// 解决 oneTBB + ThreadSanitizer 下进程退出时线程清理挂起的问题
static void finalizeTbbScheduler() {
    tbb::task_scheduler_handle handle{tbb::attach{}};
    tbb::finalize(handle, std::nothrow);
}

}  // namespace fq::test

/// 自定义 gtest main：替代 GTest::gtest_main
/// 在 RUN_ALL_TESTS() 后终止 TBB 调度器，避免 TSan 下进程退出挂起
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    const int result = RUN_ALL_TESTS();
    fq::test::finalizeTbbScheduler();
    return result;
}
