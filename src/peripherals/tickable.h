#pragma once

namespace peripherals {

class Tickable {
public:
    virtual void tick() = 0;
};

}  // namespace clock