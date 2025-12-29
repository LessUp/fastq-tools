#include <memory>

#include "fqtools/error/error.h"
#include "fqtools/common/common.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace fq::error {

class ErrorHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前重置全局处理器状态
        // 注意：在实际项目中，可能需要更好的状态管理
    }

    void TearDown() override {
        // 清理
    }
};

TEST_F(ErrorHandlerTest, RegisterSingleHandler) {
    ErrorHandler handler;

    bool wasCalled = false;
    ErrorHandler::HandlerFunc testHandler = [&wasCalled]([[maybe_unused]] const FastQException& ex) {
        wasCalled = true;
        return true;
    };

    handler.registerHandler(ErrorCategory::IO, testHandler);

    IOError ex("test.txt", 42);
    bool result = handler.handleError(ex);

    EXPECT_TRUE(result);
    EXPECT_TRUE(wasCalled);
}

TEST_F(ErrorHandlerTest, HandlerNotCalledForWrongCategory) {
    ErrorHandler handler;

    bool wasCalled = false;
    ErrorHandler::HandlerFunc testHandler = [&wasCalled]([[maybe_unused]] const FastQException& ex) {
        wasCalled = true;
        return true;
    };

    handler.registerHandler(ErrorCategory::IO, testHandler);

    // 创建不同类别的异常
    FormatError ex("test format error");
    bool result = handler.handleError(ex);

    EXPECT_FALSE(result);
    EXPECT_FALSE(wasCalled);
}

TEST_F(ErrorHandlerTest, MultipleHandlersSameCategory) {
    ErrorHandler handler;

    int callCount = 0;
    ErrorHandler::HandlerFunc handler1 = [&callCount]([[maybe_unused]] const FastQException& ex) {
        callCount++;
        return false;  // 继续下一个处理器
    };

    ErrorHandler::HandlerFunc handler2 = [&callCount]([[maybe_unused]] const FastQException& ex) {
        callCount++;
        return true;  // 处理并停止
    };

    ErrorHandler::HandlerFunc handler3 = [&callCount]([[maybe_unused]] const FastQException& ex) {
        callCount++;
        return true;
    };

    handler.registerHandler(ErrorCategory::Processing, handler1);
    handler.registerHandler(ErrorCategory::Processing, handler2);
    handler.registerHandler(ErrorCategory::Processing, handler3);

    // Using FastQException directly as placeholder for ProcessingError if not defined separately
    FastQException ex(ErrorCategory::Processing, ErrorSeverity::Error, "test processing error");
    bool result = handler.handleError(ex);

    EXPECT_TRUE(result);
    EXPECT_EQ(callCount, 2);  // handler3不应该被调用
}

TEST_F(ErrorHandlerTest, HandlerChainStopsOnFirstTrue) {
    ErrorHandler handler;

    int callCount = 0;
    ErrorHandler::HandlerFunc handler1 = [&callCount]([[maybe_unused]] const FastQException& ex) {
        callCount++;
        return true;  // 处理并停止
    };

    ErrorHandler::HandlerFunc handler2 = [&callCount]([[maybe_unused]] const FastQException& ex) {
        callCount++;
        return true;
    };

    handler.registerHandler(ErrorCategory::Validation, handler1);
    handler.registerHandler(ErrorCategory::Validation, handler2);

    FastQException ex(ErrorCategory::Validation, ErrorSeverity::Error, "test validation error");
    bool result = handler.handleError(ex);

    EXPECT_TRUE(result);
    EXPECT_EQ(callCount, 1);  // 只有handler1被调用
}

TEST_F(ErrorHandlerTest, AllHandlersReturnFalse) {
    ErrorHandler handler;

    int callCount = 0;
    ErrorHandler::HandlerFunc handler1 = [&callCount]([[maybe_unused]] const FastQException& ex) {
        callCount++;
        return false;
    };

    ErrorHandler::HandlerFunc handler2 = [&callCount]([[maybe_unused]] const FastQException& ex) {
        callCount++;
        return false;
    };

    handler.registerHandler(ErrorCategory::Resource, handler1);
    handler.registerHandler(ErrorCategory::Resource, handler2);

    FastQException ex(ErrorCategory::Resource, ErrorSeverity::Error, "test resource error");
    bool result = handler.handleError(ex);

    EXPECT_FALSE(result);
    EXPECT_EQ(callCount, 2);  // 两个处理器都被调用
}

TEST_F(ErrorHandlerTest, NoHandlersRegistered) {
    ErrorHandler handler;

    IOError ex("test.txt", 42);
    bool result = handler.handleError(ex);

    EXPECT_FALSE(result);
}

TEST_F(ErrorHandlerTest, HandlerReceivesCorrectException) {
    ErrorHandler handler;

    FastQException* capturedException = nullptr;
    ErrorHandler::HandlerFunc testHandler = [&capturedException](const FastQException& ex) {
        capturedException = const_cast<FastQException*>(&ex);
        return true;
    };

    handler.registerHandler(ErrorCategory::Configuration, testHandler);

    ConfigurationError ex("test config error");
    bool result = handler.handleError(ex);

    EXPECT_TRUE(result);
    ASSERT_NE(capturedException, nullptr);
    EXPECT_EQ(capturedException->category(), ErrorCategory::Configuration);
    EXPECT_EQ(capturedException->severity(), ErrorSeverity::Error);
    EXPECT_THAT(capturedException->message(), testing::HasSubstr("test config error"));
}

TEST_F(ErrorHandlerTest, HandlerWithExceptionDetails) {
    ErrorHandler handler;

    std::string capturedMessage;
    ErrorCategory capturedCategory;
    ErrorSeverity capturedSeverity;

    ErrorHandler::HandlerFunc detailedHandler =
        [&capturedMessage, &capturedCategory, &capturedSeverity](const FastQException& ex) {
            capturedMessage = ex.message();
            capturedCategory = ex.category();
            capturedSeverity = ex.severity();
            return true;
        };

    handler.registerHandler(ErrorCategory::Format, detailedHandler);

    FormatError ex("format error details");
    bool result = handler.handleError(ex);

    EXPECT_TRUE(result);
    EXPECT_EQ(capturedCategory, ErrorCategory::Format);
    EXPECT_EQ(capturedSeverity, ErrorSeverity::Error);
    EXPECT_EQ(capturedMessage, "format error details");
}

TEST_F(ErrorHandlerTest, MultipleCategoriesIndependent) {
    ErrorHandler handler;

    bool ioCalled = false;
    bool formatCalled = false;

    ErrorHandler::HandlerFunc ioHandler = [&ioCalled]([[maybe_unused]] const FastQException& ex) {
        ioCalled = true;
        return true;
    };

    ErrorHandler::HandlerFunc formatHandler = [&formatCalled]([[maybe_unused]] const FastQException& ex) {
        formatCalled = true;
        return true;
    };

    handler.registerHandler(ErrorCategory::IO, ioHandler);
    handler.registerHandler(ErrorCategory::Format, formatHandler);

    // 测试IO异常
    IOError ioEx("io_test.txt", 123);
    bool ioResult = handler.handleError(ioEx);
    EXPECT_TRUE(ioResult);
    EXPECT_TRUE(ioCalled);
    EXPECT_FALSE(formatCalled);

    // 重置标志
    ioCalled = false;

    // 测试Format异常
    FormatError formatEx("format_test");
    bool formatResult = handler.handleError(formatEx);
    EXPECT_TRUE(formatResult);
    EXPECT_FALSE(ioCalled);
    EXPECT_TRUE(formatCalled);
}

}  // namespace fq::error
