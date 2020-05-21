#pragma once

#include <cstdint>
#include <vector>

#include "knocknock/peripherals/tickable.h"

namespace peripherals {

class Clock : public Tickable {
public:
    Clock(uint64_t in_frequency, uint64_t out_frequency);

    void addOutput(Tickable *output);

    // Tickable::
    void tick() override;

private:
    const uint64_t in_frequency_;
    const uint64_t out_frequency_;

    std::vector<Tickable *> outputs_;
};

}  // namespace peripherals
