#pragma once
#include "fqtools/io/fastq_io.h"

namespace fq::processing {

class ReadMutatorInterface {
public:
    virtual ~ReadMutatorInterface() = default;
    virtual void process(fq::io::FastqRecord& read) = 0;
};

}  // namespace fq::processing
