#include <catch2/catch.hpp>
#include <map>

#include <knocknock/interrupt.h>

namespace interrupt {

namespace {

class InterruptCounter : public Interruptible {
public:
    InterruptCounter() : counter() {}

    bool interrupt(InterruptType reason) override {
        if (counter.count(reason) == 0) {
            counter[reason] = 0;
        }

        counter[reason]++;
        return true;
    }

    std::map<InterruptType, int> counter;
};

const std::map<InterruptType, uint8_t> MASKS = {
    {InterruptType::VBLANK, 1 << 0},
    {InterruptType::LCD_STATUS, 1 << 1},
    {InterruptType::TIMER, 1 << 2},
    {InterruptType::SERIAL, 1 << 3},
    {InterruptType::JOYPAD, 1 << 4}};

constexpr memory::MemoryAddr IF = 0xff0f;
constexpr memory::MemoryAddr IE = 0xffff;

// Three upper bits of IF and IE are unused and high by default, so this mask
// must be applied to values intended to be compared to IF and IE.
constexpr uint8_t UNUSED_BIT_MASK = 0b1110'0000;

}  // namespace

TEST_CASE("Interrupt via interrupt()", "[interrupt]") {
    InterruptCounter sink;
    InterruptController controller(&sink);

    for (const auto &[interrupt, mask] : MASKS) {
        // Clear the counter for each run.
        sink.counter.clear();

        // Disable all interrupts.
        controller[IE] = 0;

        REQUIRE(sink.counter[interrupt] == 0);

        controller.interrupt(interrupt);
        controller.tick();
        // Request a interrupt using IF, but the interrupt is not enabled, so
        // the value and counter does not change.
        REQUIRE(controller[IF] == (UNUSED_BIT_MASK | mask));
        REQUIRE(sink.counter[interrupt] == 0);

        // Enable the interrupt
        controller[IE] = mask;
        controller.tick();
        // Now that the interrupt is enabled and requested, the interrupt
        // controller will service it. In that case, the counter is modified
        // and the value of the IF register changed to reset the bit.
        REQUIRE(controller[IF] == (UNUSED_BIT_MASK | 0));
        REQUIRE(sink.counter[interrupt] == 1);

        // Request the interrupt again when that interrupt is already enabled.
        controller.interrupt(interrupt);
        controller.tick();
        REQUIRE(controller[IF] == (UNUSED_BIT_MASK | 0));
        REQUIRE(sink.counter[interrupt] == 2);
    }
}

TEST_CASE("Interrupt via modifying IF", "[interrupt]") {
    InterruptCounter sink;
    InterruptController controller(&sink);

    for (const auto &[interrupt, mask] : MASKS) {
        // Clear the counter for each run.
        sink.counter.clear();

        // Disable all interrupts.
        controller[IE] = 0;

        REQUIRE(sink.counter[interrupt] == 0);

        controller[IF] = mask;
        controller.tick();
        // Request a interrupt using IF, but the interrupt is not enabled, so
        // the value and counter does not change.
        REQUIRE(controller[IF] == (UNUSED_BIT_MASK | mask));
        REQUIRE(sink.counter[interrupt] == 0);

        // Enable the interrupt
        controller[IE] = mask;
        controller.tick();
        // Now that the interrupt is enabled and requested, the interrupt
        // controller will service it. In that case, the counter is modified
        // and the value of the IF register changed to reset the bit.
        REQUIRE(controller[IF] == (UNUSED_BIT_MASK | 0));
        REQUIRE(sink.counter[interrupt] == 1);

        // Request the interrupt again.
        controller[IF] = mask;
        controller.tick();
        REQUIRE(controller[IF] == (UNUSED_BIT_MASK | 0));
        REQUIRE(sink.counter[interrupt] == 2);
    }
}

}  // namespace interrupt