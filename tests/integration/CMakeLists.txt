# ---------------------------------------------------------------------------
# 集成测试
# 验证多模块间交互（文件 I/O、多线程、Pipeline 链路等）
# ---------------------------------------------------------------------------

add_fq_test(test_integration_pipeline "integration" 120
    test_pipeline_integration.cpp
)

# --- 聚合目标 ---
add_custom_target(integration_tests
    DEPENDS
        test_integration_pipeline
)
