#pragma once
/**
 * @file fq.h
 * @brief FastQTools 公共 API Façade
 * @details 一站式入口，聚合所有对外公共接口
 */

// 公共工具
#include "fqtools/common/common.h"
#include "fqtools/config/config.h"
#include "fqtools/error/error.h"

// I/O
#include "fqtools/io/fastq_io.h"
#include "fqtools/io/fastq_reader.h"
#include "fqtools/io/fastq_writer.h"
#include "fqtools/io/reader_interface.h"
#include "fqtools/io/writer_interface.h"

// 处理管道
#include "fqtools/processing/mutators.h"
#include "fqtools/processing/predicates.h"
#include "fqtools/processing/processing_options.h"
#include "fqtools/processing/processing_pipeline_interface.h"
#include "fqtools/processing/read_mutator_interface.h"
#include "fqtools/processing/read_predicate_interface.h"

// 统计
#include "fqtools/statistics/statistic_calculator_interface.h"
#include "fqtools/statistics/statistic_interface.h"
#include "fqtools/statistics/statistics_writer.h"
