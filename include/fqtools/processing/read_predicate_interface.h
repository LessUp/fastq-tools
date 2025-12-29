#pragma once

#include "fqtools/io/fastq_io.h"

namespace fq::processing {

class ReadPredicateInterface {
public:
    virtual ~ReadPredicateInterface() = default;
    virtual auto evaluate(const fq::io::FastqRecord& read) const -> bool = 0;
};

}  // namespace fq::processing
