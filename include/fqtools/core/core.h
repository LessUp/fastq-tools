/**
 * @file core.h
 * @brief 核心功能组件定义。
 * @details 提供FastQ数据处理的基础类和工具，包括ID管理、序列和质量验证等功能。
 *
 * @author FastQTools Team
 * @date 2024
 * @version 1.0
 *
 * @copyright Copyright (c) 2024 FastQTools
 * @license MIT License
 */

#pragma once

// 传统头文件使用，尚未模块化
// export module fq.core;  // 当前尚不支持此模块导出

#include "fqtools/common/common.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "fqtools/error/error.h"

// import fq.common;
// import fq.error;

namespace fq::core {
// 基础ID类型
using ReadID = std::uint64_t;
using BatchID = std::uint32_t;

// 质量评分类型
enum class QScoreType {
    Unknown = 0,
    Sanger = 1,
    Illumina13 = 2,
    Illumina15 = 3,
    Illumina18 = 4,
    MGI = 5,
    NovaSeqQ4 = 6,
    MGIQ4 = 7
};

// 测序代数
enum class SequencingGeneration { Second = 2, Third = 3 };

/**
 * @class WithID
 * @brief 提供ID管理功能的基础接口。
 */
class WithID {
public:
    virtual ~WithID() = default;

    [[nodiscard]] auto id() const noexcept -> ReadID {
        return id_;
    }

protected:
    void setId(ReadID id) noexcept {
        id_ = id;
    }

    WithID() : id_(fq::common::IDGenerator::nextId()) {}
    explicit WithID(ReadID id) : id_(id) {}

private:
    ReadID id_;
};

// 可克隆接口
template <typename T>
class Cloneable {
public:
    virtual ~Cloneable() = default;
    [[nodiscard]] virtual auto clone() const -> std::unique_ptr<T> = 0;
};

// 可序列化接口
class Serializable {
public:
    virtual ~Serializable() = default;
    virtual void serialize(std::ostream& os) const = 0;
    virtual void deserialize(std::istream& is) = 0;
};

// 可验证接口
class Validatable {
public:
    virtual ~Validatable() = default;
    [[nodiscard]] virtual auto isValid() const noexcept -> bool = 0;
    [[nodiscard]] virtual auto validationErrors() const -> std::vector<std::string> {
        return {};
    }
};

// 内存使用统计接口
class MemoryTrackable {
public:
    virtual ~MemoryTrackable() = default;
    [[nodiscard]] virtual auto memoryUsage() const noexcept -> std::size_t = 0;
};

// 统计信息接口
class Statisticable {
public:
    virtual ~Statisticable() = default;
    [[nodiscard]] virtual auto getStatistics() const
        -> std::unordered_map<std::string, std::uint64_t> = 0;
    virtual void resetStatistics() = 0;
};

// 可配置接口
class Configurable {
public:
    virtual ~Configurable() = default;
    virtual void configure(const std::unordered_map<std::string, std::string>& config) = 0;
    [[nodiscard]] virtual auto getConfigSchema() const -> std::vector<std::string> {
        return {};
    }
};

// 质量分数工具
class QualityScore {
public:
    static constexpr int kMinQuality = 0;
    static constexpr int kMaxQuality = 93;
    static constexpr char kMinAscii = '!';
    static constexpr char kMaxAscii = '~';

    // Sanger质量分数转换
    static auto sangerToQuality(char asciiChar) -> int {
        return static_cast<int>(asciiChar - '!');
    }

    static auto qualityToSanger(int quality) -> char {
        if (quality < kMinQuality || quality > kMaxQuality) {
            throw fq::error::FastQException(fq::error::ErrorCategory::Validation,
                                            fq::error::ErrorSeverity::Error,
                                            std::string("Quality score out of range: ") +
                                                std::to_string(quality));
        }
        return static_cast<char>('!' + quality);
    }

    // 计算平均质量分数
    template <std::ranges::range R>
    static auto calculateAverageQuality(R&& qualityString) -> double {
        if (std::ranges::empty(qualityString)) {
            return 0.0;
        }

        double sum = 0.0;
        std::size_t count = 0;

        for (char c : qualityString) {
            sum += sangerToQuality(c);
            ++count;
        }

        return sum / static_cast<double>(count);
    }

    // 验证质量字符串
    template <std::ranges::range R>
    static auto isValidQualityString(R&& qualityString) -> bool {
        return std::ranges::all_of(qualityString,
                                   [](char c) { return c >= kMinAscii && c <= kMaxAscii; });
    }
};

/**
 * @class SequenceUtils
 * @brief 提供DNA/RNA序列验证与处理的工具函数。
 *
 * 提供GC含量计算、反向互补、序列复杂度计算等功能。
 */
class SequenceUtils {
public:
    // 核酸字符集
    static constexpr std::string_view kValidDnaChars = "ACGTNacgtn";
    static constexpr std::string_view kValidRnaChars = "ACGUNacgun";

    // 验证DNA序列
    template <std::ranges::range R>
    static auto isValidDna(R&& sequence) -> bool {
        return std::ranges::all_of(
            sequence, [](char c) { return kValidDnaChars.find(c) != std::string_view::npos; });
    }

    // 验证RNA序列
    template <std::ranges::range R>
    static auto isValidRna(R&& sequence) -> bool {
        return std::ranges::all_of(
            sequence, [](char c) { return kValidRnaChars.find(c) != std::string_view::npos; });
    }

    // 计算GC含量
    template <std::ranges::range R>
    static auto calculateGcContent(R&& sequence) -> double {
        if (std::ranges::empty(sequence)) {
            return 0.0;
        }

        std::size_t gcCount = 0;
        std::size_t totalCount = 0;

        for (char c : sequence) {
            if (c == 'G' || c == 'C' || c == 'g' || c == 'c') {
                ++gcCount;
            }
            if (c != 'N' && c != 'n') {
                ++totalCount;
            }
        }

        return totalCount > 0
            ? static_cast<double>(gcCount) / static_cast<double>(totalCount) * 100.0
            : 0.0;
    }

    // 反向互补
    static auto reverseComplement(std::string_view sequence) -> std::string {
        std::string result;
        result.reserve(sequence.size());

        for (auto it = sequence.rbegin(); it != sequence.rend(); ++it) {
            char c = *it;
            switch (c) {
                case 'A':
                case 'a':
                    result += (c == 'A') ? 'T' : 't';
                    break;
                case 'T':
                case 't':
                    result += (c == 'T') ? 'A' : 'a';
                    break;
                case 'G':
                case 'g':
                    result += (c == 'G') ? 'C' : 'c';
                    break;
                case 'C':
                case 'c':
                    result += (c == 'C') ? 'G' : 'g';
                    break;
                case 'N':
                case 'n':
                    result += c;
                    break;
                default:
                    result += c;
                    break;
            }
        }

        return result;
    }

    // 序列复杂度计算（基于Shannon熵）
    template <std::ranges::range R>
    static auto calculateComplexity(R&& sequence) -> double {
        if (std::ranges::empty(sequence)) {
            return 0.0;
        }

        std::unordered_map<char, std::size_t> counts;
        std::size_t total = 0;

        for (char c : sequence) {
            ++counts[std::toupper(c)];
            ++total;
        }

        double entropy = 0.0;
        for (const auto& [base, count] : counts) {
            if (count > 0) {
                double p = static_cast<double>(count) / static_cast<double>(total);
                entropy -= p * std::log2(p);
            }
        }

        return entropy;
    }
};

// 性能统计工具
class PerformanceMetrics {
public:
    struct Metrics {
        std::chrono::nanoseconds processingTime{0};
        std::size_t itemsProcessed = 0;
        std::size_t bytesProcessed = 0;
        std::size_t peakMemoryUsage = 0;

        [[nodiscard]] auto itemsPerSecond() const -> double {
            if (processingTime.count() == 0)
                return 0.0;
            return static_cast<double>(itemsProcessed) /
                (static_cast<double>(processingTime.count()) / 1e9);
        }

        [[nodiscard]] auto bytesPerSecond() const -> double {
            if (processingTime.count() == 0)
                return 0.0;
            return static_cast<double>(bytesProcessed) /
                (static_cast<double>(processingTime.count()) / 1e9);
        }

        [[nodiscard]] auto megabytesPerSecond() const -> double {
            return bytesPerSecond() / (1024.0 * 1024.0);
        }
    };

    void startTiming() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }

    void stopTiming() {
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics_.processingTime += endTime - startTime_;
    }

    void addItemsProcessed(std::size_t count) {
        metrics_.itemsProcessed += count;
    }

    void addBytesProcessed(std::size_t bytes) {
        metrics_.bytesProcessed += bytes;
    }

    void updatePeakMemory(std::size_t currentMemory) {
        metrics_.peakMemoryUsage = std::max(metrics_.peakMemoryUsage, currentMemory);
    }

    [[nodiscard]] auto getMetrics() const -> const Metrics& {
        return metrics_;
    }

    void reset() {
        metrics_ = Metrics{};
    }

private:
    Metrics metrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
};
}  // namespace fq::core
