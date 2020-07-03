#include <catch2/catch.hpp>

#include <knocknock/peripherals/clock.h>

namespace peripherals {

namespace {

class ClockCounter : public Tickable {
public:
    ClockCounter() : count_() {}

    // Tickable::
    void tick() override { count_++; }

    [[nodiscard]] uint64_t count() const { return count_; }

private:
    uint64_t count_;
};

}  // namespace

TEST_CASE("Divisible frequency", "[peripherals][clock]") {
    ClockCounter counter;

    Clock clock(5, 50);
    clock.add_output(&counter);

    for (int i = 1; i <= 5; ++i) {
        clock.tick();
        REQUIRE(counter.count() == i * 10);
    }
}

TEST_CASE("Non divisible frequency", "[peripherals][clock]") {
    ClockCounter counter;

    Clock clock(15, 17263);
    clock.add_output(&counter);

    for (int i = 0; i < 15; ++i) {
        clock.tick();
    }

    // Actual tick count should be within 99% of the target frequency.
    REQUIRE(counter.count() >= (17263 * 0.99));
}

}  // namespace peripherals
