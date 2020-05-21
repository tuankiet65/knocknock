#include <catch2/catch.hpp>

#include <knocknock/peripherals/serial.h>

namespace peripherals {

TEST_CASE("Receive", "[peripherals][serial]") {
    Serial serial;

    // Send the byte 0x69 from outside to the Gameboy.
    serial.send(0x69);

    // Initiate a transfer session.
    serial.write(0xff02, 0x81);

    for (int i = 0; i < 8; ++i) {
        serial.tick();
    }

    // Check that the initiate flag in SC is reset.
    REQUIRE(serial.read(0xff02) == 0x01);
    // Check that SB contains what we just sent.
    REQUIRE(serial.read(0xff01) == 0x69);
}

TEST_CASE("Send", "[peripherals][serial]") {
    Serial serial;

    // Send the byte 0x69 from the Gameboy to outside.
    serial.write(0xff01, 0x69);
    serial.write(0xff02, 0x81);

    for (int i = 0; i < 8; ++i) {
        serial.tick();
    }

    // We just transferred one byte, check that the initiate flag is reset,
    // so that we can transfer another byte.
    REQUIRE(serial.read(0xff02) == 0x01);

    // Send the byte 0x96 from the Gameboy to outside.
    serial.write(0xff01, 0x96);
    serial.write(0xff02, 0x81);

    for (int i = 0; i < 8; ++i) {
        serial.tick();
    }

    // Check that initiate flag is reset.
    REQUIRE(serial.read(0xff02) == 0x01);

    // Check that the Gameboy first send the byte 0x69 to outside.
    auto received = serial.receive();
    REQUIRE(received);
    REQUIRE((*received) == 0x69);

    // Then check that the Gameboy send the byte 0x96 to outside.
    received = serial.receive();
    REQUIRE(received);
    REQUIRE(*received == 0x96);
}

}  // namespace peripherals
