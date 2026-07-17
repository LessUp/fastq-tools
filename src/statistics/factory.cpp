#include <utility>

#include "statistics/fq_statistic.h"

namespace fq::statistics {

class Calculator::Impl {
public:
    explicit Impl(StatisticOptions options) : calculator(std::move(options)) {}

    FastqStatisticCalculator calculator;
};

Calculator::Calculator(StatisticOptions options)
    : impl_(std::make_unique<Impl>(std::move(options))) {}
Calculator::~Calculator() = default;
Calculator::Calculator(Calculator&&) noexcept = default;
auto Calculator::operator=(Calculator&&) noexcept -> Calculator& = default;

void Calculator::run() {
    impl_->calculator.run();
}

}  // namespace fq::statistics
