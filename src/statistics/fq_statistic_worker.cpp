#include "statistics/fq_statistic_worker.h"

#include "fqtools/io/fastq_io.h"

#include "statistics/fq_statistic.h"

namespace fq::statistic {

FqStatisticWorker::FqStatisticWorker(int qualOffset) : qualOffset_(qualOffset) {}

auto FqStatisticWorker::calculateStats(const Batch& batch) -> IStatistic::Result {
    FqStatisticResult result;
    if (batch.empty()) {
        return result;
    }

    for (const auto& read : batch) {
        result.readCount++;
        size_t len = read.seq.size();
        result.totalBases += len;

        if (len > result.maxReadLength) {
            result.maxReadLength = len;
            // Ensure vectors are large enough
            if (len > result.posQualityDist.size()) {
                result.posQualityDist.resize(len, std::vector<uint64_t>(kMaxQual, 0));
                result.posBaseDist.resize(len, std::vector<uint64_t>(kMaxBaseNum, 0));
            }
        }

        for (size_t i = 0; i < len; ++i) {
            // Quality stats
            // TODO: Handle different quality systems robustly. Currently assumes simple offset.
            int qVal = static_cast<int>(read.qual[i]) - qualOffset_;
            if (qVal < 0)
                qVal = 0;
            if (qVal >= kMaxQual)
                qVal = kMaxQual - 1;

            result.posQualityDist[i][qVal]++;

            // Base stats
            int baseIdx = 4;  // Default to N
            switch (read.seq[i]) {
                case 'A':
                case 'a':
                    baseIdx = 0;
                    break;
                case 'C':
                case 'c':
                    baseIdx = 1;
                    break;
                case 'G':
                case 'g':
                    baseIdx = 2;
                    break;
                case 'T':
                case 't':
                    baseIdx = 3;
                    break;
                default:
                    baseIdx = 4;
                    break;
            }
            result.posBaseDist[i][baseIdx]++;
        }
    }

    return result;
}

}  // namespace fq::statistic
