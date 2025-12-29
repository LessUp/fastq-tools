/**
 * @file statistic_calculator_interface.h
 * @brief Interface and structures for statistic calculation tasks.
 *
 * This file provides interfaces and supporting structures for performing
 * various statistics calculations on FASTQ data. It decouples clients from
 * concrete implementations and facilitates flexible, high-level processing.
 *
 * @author LessUp
 * @date 2023-10-05
 * @version 1.0
 * @copyright (c) 2023 LessUp. All rights reserved.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace fq::statistic {

/**
 * @brief Configuration options for a statistics calculation task.
 * This struct is defined at the interface level to decouple clients
 * from the concrete implementation.
 */
struct StatisticOptions {
    std::string inputFastqPath;
    std::string outputStatPath;
    uint32_t batchSize = 50000;
    uint32_t threadCount = 4;

    size_t readChunkBytes = 1 * 1024 * 1024;
    size_t zlibBufferBytes = 128 * 1024;
    size_t batchCapacityBytes = 4 * 1024 * 1024;
    size_t memoryLimitBytes = 0;
    size_t maxInFlightBatches = 0;
};

/**
 * @brief Abstract interface for a high-level statistic calculation task.
 * This decouples the command-line layer from the statistics implementation.
 */
class StatisticCalculatorInterface {
public:
    virtual ~StatisticCalculatorInterface() = default;

    /**
     * @brief Executes the entire statistics generation process.
     */
    virtual void run() = 0;
};

/**
 * @brief factory function to create an instance of a statistic calculator.
 * @param options The configuration for the calculation task.
 * @return A unique_ptr to an object implementing the StatisticCalculatorInterface interface.
 */
auto createStatisticCalculator(const StatisticOptions& options)
    -> std::unique_ptr<StatisticCalculatorInterface>;

}  // namespace fq::statistic
