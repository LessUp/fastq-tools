#pragma once
/**
 * @file fq.h
 * @brief FastQTools 公共 API Façade
 * @details 一站式入口，聚合所有对外公共接口
 */

#include "fqtools/cli/app_info.h"
#include "fqtools/common/common.h"
#include "fqtools/processing/processing_pipeline_interface.h"
#include "fqtools/processing/read_mutator_interface.h"
#include "fqtools/processing/read_predicate_interface.h"
#include "fqtools/processing/mutators/quality_trimmer.h"
#include "fqtools/processing/predicates/min_quality_predicate.h"
#include "fqtools/statistics/statistic_calculator_interface.h"
