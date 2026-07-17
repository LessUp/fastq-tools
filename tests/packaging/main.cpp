#include <iostream>
#include <type_traits>

#include <fqtools/fq.h>

auto main() -> int {
    static_assert(!std::is_copy_constructible_v<fq::processing::Pipeline>);
    static_assert(!std::is_copy_constructible_v<fq::statistics::Calculator>);
    fq::processing::Pipeline pipeline;
    static_cast<void>(pipeline);
    std::cout << "FastQTools\n";
    return 0;
}
