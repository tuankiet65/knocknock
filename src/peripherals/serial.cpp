#include "serial.h"

#include <glog/logging.h>

namespace peripherals {

namespace {

const memory::MemoryAddr DATA_REG = 0xff01;
const memory::MemoryAddr CONTROL_REG = 0xff02;

const uint8_t CONTROL_REG_START = 1u << 7u;

uint8_t MSB(uint8_t value) {
    return ((value & (1u << 7u)) >> 7u);
}

}  // namespace

Serial::Serial()
    : memory::Memory(),
      is_in_session_(false),
      data_(),
      control_(),
      in_buffer_(),
      current_in_byte_(),
      out_buffer_(),
      current_out_byte_(),
      remaining_bits_() {}

memory::MemoryValue Serial::read(memory::MemoryAddr addr) const {
    if (addr == DATA_REG) {
        return data_;
    }

    if (addr == CONTROL_REG) {
        return control_;
    }

    DCHECK(false) << "Invalid read from Serial.";
    return 0xff;
}

void Serial::write(memory::MemoryAddr addr, memory::MemoryValue value) {
    if (addr == DATA_REG) {
        if (is_in_session_) {
            LOG(ERROR) << "Writing to SB when serial is in session";
            return;
        }

        data_ = value;
        return;
    }

    if (addr == CONTROL_REG) {
        control_ = value;

        if (control_ & CONTROL_REG_START && (!is_in_session_)) {
            begin_session();
        }

        return;
    }

    DCHECK(false) << "Invalid write to Serial";
}

void Serial::send(uint8_t value) {
    in_buffer_.push(value);
}

std::optional<uint8_t> Serial::receive() {
    if (out_buffer_.empty()) {
        return {};
    }

    uint8_t result = out_buffer_.front();
    out_buffer_.pop();

    return result;
}

void Serial::begin_session() {
    is_in_session_ = true;

    if (!in_buffer_.empty()) {
        current_in_byte_ = in_buffer_.front();
        in_buffer_.pop();
    } else {
        current_in_byte_ = 0;
    }

    current_out_byte_ = 0;

    remaining_bits_ = 8;
}

void Serial::end_session() {
    is_in_session_ = false;

    out_buffer_.push(current_out_byte_);

    control_ &= ~CONTROL_REG_START;
}

void Serial::tick() {
    if (!is_in_session_) {
        return;
    }

    // Send the MSB of data_ and shift it one bit to the left.
    current_out_byte_ = (current_out_byte_ << 1) | MSB(data_);
    data_ <<= 1;

    // Receive the MSB of the incoming data and shift the incoming data one bit
    // to the left.
    data_ |= MSB(current_in_byte_);
    current_in_byte_ <<= 1;

    remaining_bits_--;

    if (remaining_bits_ == 0) {
        end_session();
    }
}

void Serial::register_to_mmu(memory::MMU *mmu) {
    mmu->register_region(this, DATA_REG, CONTROL_REG);
}

}  // namespace peripherals
