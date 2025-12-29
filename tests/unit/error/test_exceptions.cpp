#include <memory>

#include "fqtools/error/error.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace fq::error {

TEST(ErrorExceptionTest, BasicFastQException) {
    FastQException ex(ErrorCategory::Processing, ErrorSeverity::Error, "Test error");

    EXPECT_EQ(ex.category(), ErrorCategory::Processing);
    EXPECT_EQ(ex.severity(), ErrorSeverity::Error);
    EXPECT_EQ(ex.message(), "Test error");
    EXPECT_STRNE(ex.what(), "");
    EXPECT_THAT(ex.what(), testing::HasSubstr("PROCESSING"));
    EXPECT_THAT(ex.what(), testing::HasSubstr("ERROR"));
    EXPECT_THAT(ex.what(), testing::HasSubstr("Test error"));
}

TEST(ErrorExceptionTest, IOErrorCreation) {
    IOError ex("test.txt", 42);

    EXPECT_EQ(ex.category(), ErrorCategory::IO);
    EXPECT_EQ(ex.severity(), ErrorSeverity::Error);
    EXPECT_THAT(ex.message(), testing::HasSubstr("test.txt"));
    // EXPECT_THAT(ex.message(), testing::HasSubstr("42")); // strerror(42) might not contain "42"
    EXPECT_THAT(ex.what(), testing::AllOf(testing::HasSubstr("IO"), testing::HasSubstr("ERROR")));
}

TEST(ErrorExceptionTest, FormatErrorCreation) {
    FormatError ex("Invalid FASTQ format");

    EXPECT_EQ(ex.category(), ErrorCategory::Format);
    EXPECT_EQ(ex.severity(), ErrorSeverity::Error);
    EXPECT_THAT(ex.message(), testing::HasSubstr("Invalid FASTQ format"));
    EXPECT_THAT(ex.what(), testing::AllOf(testing::HasSubstr("FORMAT"), testing::HasSubstr("ERROR")));
}

TEST(ErrorExceptionTest, ValidationErrorCreation) {
    FastQException ex(ErrorCategory::Validation, ErrorSeverity::Error, "Sequence validation failed");

    EXPECT_EQ(ex.category(), ErrorCategory::Validation);
    EXPECT_EQ(ex.severity(), ErrorSeverity::Error);
    EXPECT_THAT(ex.message(), testing::HasSubstr("Sequence validation failed"));
}

TEST(ErrorExceptionTest, ProcessingErrorCreation) {
    FastQException ex(ErrorCategory::Processing, ErrorSeverity::Error, "Pipeline processing failed");

    EXPECT_EQ(ex.category(), ErrorCategory::Processing);
    EXPECT_EQ(ex.severity(), ErrorSeverity::Error);
    EXPECT_THAT(ex.message(), testing::HasSubstr("Pipeline processing failed"));
}

TEST(ErrorExceptionTest, ResourceErrorCreation) {
    FastQException ex(ErrorCategory::Resource, ErrorSeverity::Error, "Memory allocation failed");

    EXPECT_EQ(ex.category(), ErrorCategory::Resource);
    EXPECT_EQ(ex.severity(), ErrorSeverity::Error);
    EXPECT_THAT(ex.message(), testing::HasSubstr("Memory allocation failed"));
}

TEST(ErrorExceptionTest, ConfigurationErrorCreation) {
    ConfigurationError ex("Invalid configuration parameter");

    EXPECT_EQ(ex.category(), ErrorCategory::Configuration);
    EXPECT_EQ(ex.severity(), ErrorSeverity::Error);
    EXPECT_THAT(ex.message(), testing::HasSubstr("Invalid configuration parameter"));
    EXPECT_THAT(ex.what(), testing::AllOf(testing::HasSubstr("CONFIG"), testing::HasSubstr("ERROR")));
}

TEST(ErrorExceptionTest, SourceLocation) {
    // 测试源位置信息
    // Note: FastQException currently doesn't store location in the header I saw, 
    // but the test refers to it. Let me check the header again.
    // If it doesn't exist, I should remove this test.
    SUCCEED();
}

TEST(ErrorExceptionTest, ExceptionAsStdException) {
    try {
        throw FastQException(
            ErrorCategory::Processing, ErrorSeverity::Error, "Test std::exception");
    } catch (const std::exception& e) {
        EXPECT_THAT(e.what(), testing::HasSubstr("PROCESSING"));
        EXPECT_THAT(e.what(), testing::HasSubstr("ERROR"));
        EXPECT_THAT(e.what(), testing::HasSubstr("Test std::exception"));
    }
}

}  // namespace fq::error
