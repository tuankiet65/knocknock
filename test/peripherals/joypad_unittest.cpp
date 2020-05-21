#include <catch2/catch.hpp>

#include <knocknock/peripherals/joypad.h>

using peripherals::Joypad;

static constexpr memory::MemoryAddr P1_REG_ADDR = 0xff00;

TEST_CASE("Initial state", "[peripherals][joypad]") {
    peripherals::Joypad joypad;

    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00111111);
}

TEST_CASE("P14 port", "[peripherals][joypad]") {
    peripherals::Joypad joypad;

    // Pull P14 low
    joypad.write(P1_REG_ADDR, 0b00100000);

    // No buttons are pressed so P10/P11/P12/P13 are high
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00101111);

    joypad.press(Joypad::Button::RIGHT);
    REQUIRE(joypad.is_pressed(Joypad::Button::RIGHT));
    joypad.press(Joypad::Button::UP);
    REQUIRE(joypad.is_pressed(Joypad::Button::UP));

    // P10/P12 low, P11/P13 high.
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00101010);

    joypad.press(Joypad::Button::LEFT);
    REQUIRE(joypad.is_pressed(Joypad::Button::LEFT));
    joypad.press(Joypad::Button::DOWN);
    REQUIRE(joypad.is_pressed(Joypad::Button::DOWN));

    // P10/P11/P12/P13 low.
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00100000);

    joypad.release(Joypad::Button::RIGHT);
    REQUIRE(!joypad.is_pressed(Joypad::Button::RIGHT));
    joypad.release(Joypad::Button::UP);
    REQUIRE(!joypad.is_pressed(Joypad::Button::UP));
    joypad.release(Joypad::Button::LEFT);
    REQUIRE(!joypad.is_pressed(Joypad::Button::LEFT));
    joypad.release(Joypad::Button::DOWN);
    REQUIRE(!joypad.is_pressed(Joypad::Button::DOWN));

    // P10/P11/P12/P13 high.
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00101111);
}

TEST_CASE("P15 port", "[peripherals][joypad]") {
    peripherals::Joypad joypad;

    // Pull P15 low
    joypad.write(P1_REG_ADDR, 0b00010000);

    // No buttons are pressed so P10/P11/P12/P13 are high
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00011111);

    joypad.press(Joypad::Button::A);
    REQUIRE(joypad.is_pressed(Joypad::Button::A));
    joypad.press(Joypad::Button::SELECT);
    REQUIRE(joypad.is_pressed(Joypad::Button::SELECT));

    // P10/P12 low, P11/P13 high.
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00011010);

    joypad.press(Joypad::Button::B);
    REQUIRE(joypad.is_pressed(Joypad::Button::B));
    joypad.press(Joypad::Button::START);
    REQUIRE(joypad.is_pressed(Joypad::Button::START));

    // P10/P11/P12/P13 low.
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00010000);

    joypad.release(Joypad::Button::A);
    REQUIRE(!joypad.is_pressed(Joypad::Button::A));
    joypad.release(Joypad::Button::B);
    REQUIRE(!joypad.is_pressed(Joypad::Button::B));
    joypad.release(Joypad::Button::SELECT);
    REQUIRE(!joypad.is_pressed(Joypad::Button::SELECT));
    joypad.release(Joypad::Button::START);
    REQUIRE(!joypad.is_pressed(Joypad::Button::START));

    // P10/P11/P12/P13 high.
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00011111);
}

TEST_CASE("P14 + P15 port", "[peripherals][joypad]") {
    peripherals::Joypad joypad;

    // Pull P14 and P15 low
    joypad.write(P1_REG_ADDR, 0b00000000);

    joypad.press(Joypad::Button::RIGHT);
    REQUIRE(joypad.is_pressed(Joypad::Button::RIGHT));
    joypad.press(Joypad::Button::START);
    REQUIRE(joypad.is_pressed(Joypad::Button::START));

    // P10/P13 low, P11/P12 high
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00000110);

    joypad.press(Joypad::Button::B);
    REQUIRE(joypad.is_pressed(Joypad::Button::B));
    joypad.press(Joypad::Button::UP);
    REQUIRE(joypad.is_pressed(Joypad::Button::UP));

    // P10/P11/P12/P13 low.
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00000000);

    joypad.release(Joypad::Button::START);
    REQUIRE(!joypad.is_pressed(Joypad::Button::START));
    joypad.release(Joypad::Button::UP);
    REQUIRE(!joypad.is_pressed(Joypad::Button::UP));

    // P10/P11 low, P12/P13 high
    REQUIRE(joypad.read(P1_REG_ADDR) == 0b00001100);
}
