#include "clock.h"

#include <glog/logging.h>

namespace peripherals {

Clock::Clock(uint64_t in_frequency, uint64_t out_frequency)
    : in_frequency_(in_frequency),
      out_frequency_(out_frequency),
      outputs_() {
    DCHECK(in_frequency_ <= out_frequency_)
        << "Input frequency is larger than output frequency";
}

void Clock::addOutput(Tickable *output) {
    outputs_.push_back(output);
}

void Clock::tick() {
    for (uint64_t i = 0; i < (out_frequency_ / in_frequency_); ++i) {
        for (Tickable *observer : outputs_) {
            observer->tick();
        }
    }
}

}  // namespace peripherals
