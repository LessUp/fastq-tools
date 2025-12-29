#include <iostream>
#include <string>

#include "fqtools/common/common.h"

auto main() -> int {
    const std::string input = "  FastQTools  ";
    const std::string trimmed = fq::common::trim(input);

    std::cout << trimmed << '\n';
    return 0;
}
