#pragma once
/**
 * @file fq.h
 * @brief FastQTools 公共 API Façade，聚合所有对外接口
 */

#include "fqtools/error/error.h"
#include "fqtools/io/fastq_io.h"
#include "fqtools/io/fastq_reader.h"
#include "fqtools/io/fastq_writer.h"
#include "fqtools/io/interfaces.h"
#include "fqtools/processing/interfaces.h"
#include "fqtools/processing/mutators.h"
#include "fqtools/processing/predicates.h"
#include "fqtools/processing/processing_options.h"
#include "fqtools/processing/processing_pipeline_interface.h"
#include "fqtools/statistics/interfaces.h"
#include "fqtools/statistics/statistics_writer.h"
