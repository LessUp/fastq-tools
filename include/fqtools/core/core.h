/**
 * @file core.h
 * @brief 核心工具类定义。
 * @details 提供 FastQ 数据处理的基础工具，包括质量分数转换和序列验证等功能。
 *
 * @note 此头文件经过精简，移除了所有未被项目使用的抽象接口
 *       （WithID, Cloneable, Serializable, Validatable, MemoryTrackable,
 *       Statisticable, Configurable, PerformanceMetrics）。
 *       如需这些接口，可在具体模块中按需定义。
 *
 * @author FastQTools Team
 * @date 2024
 * @version 2.0
 *
 * @copyright Copyright (c) 2024 FastQTools
 * @license MIT License
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fq::core {

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
            throw std::out_of_range("Quality score out of range: " + std::to_string(quality));
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

}  // namespace fq::core
