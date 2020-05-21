/**
 * Constants for start/end addresses and sizes of various memory regions.
 * @file regions.h
 */
#pragma once

#include "knocknock/memory/memory.h"

namespace memory {

/** ROM bank #0 begin address */
inline constexpr MemoryAddr ROM_0_BEGIN = 0x0000;
/** ROM bank #0 size */
inline constexpr MemorySize ROM_0_SIZE = 0x4000;
/** ROM bank #0 end address */
inline constexpr MemoryAddr ROM_0_END = ROM_0_BEGIN + ROM_0_SIZE - 1;

/** Switchable ROM bank begin address */
inline constexpr MemoryAddr ROM_SWITCHABLE_BEGIN = 0x4000;
/** Switchable ROM bank size */
inline constexpr MemorySize ROM_SWITCHABLE_SIZE = 0x4000;
/** Switchable ROM bank end address */
inline constexpr MemoryAddr ROM_SWITCHABLE_END =
    ROM_SWITCHABLE_BEGIN + ROM_SWITCHABLE_SIZE - 1;

/** Video RAM begin address */
inline constexpr MemoryAddr VRAM_BEGIN = 0x8000;
/** Video RAM size */
inline constexpr MemorySize VRAM_SIZE = 0x2000;
/** Video RAM end address */
inline constexpr MemoryAddr VRAM_END = VRAM_BEGIN + VRAM_SIZE - 1;

/** External (on cartridge, bank switchable) RAM begin address */
inline constexpr MemoryAddr RAM_EXTERNAL_BEGIN = 0xa000;
/** External RAM size */
inline constexpr MemorySize RAM_EXTERNAL_SIZE = 0x2000;
/** External RAM end address */
inline constexpr MemoryAddr RAM_EXTERNAL_END =
    RAM_EXTERNAL_BEGIN + RAM_EXTERNAL_SIZE - 1;

/** Internal RAM begin address */
inline constexpr MemoryAddr RAM_INTERNAL_BEGIN = 0xc000;
/** Internal RAM size */
inline constexpr MemorySize RAM_INTERNAL_SIZE = 0x2000;
/** Internal RAM end address */
inline constexpr MemoryAddr RAM_INTERNAL_END =
    RAM_INTERNAL_BEGIN + RAM_INTERNAL_SIZE - 1;

/**
 * RAM Echo region begin address. This region mirrors the Internal RAM region
 * from 0xc000 - 0xddff
 */
inline constexpr MemoryAddr RAM_ECHO_BEGIN = 0xe000;
/** RAM echo region size */
inline constexpr MemorySize RAM_ECHO_SIZE = 0x1e00;
/** RAM echo region end address */
inline constexpr MemoryAddr RAM_ECHO_END = RAM_ECHO_BEGIN + RAM_ECHO_SIZE - 1;

/** Spirit Attribute Memory (OAM) begin address */
inline constexpr MemoryAddr OAM_BEGIN = 0xfe00;
/** OAM size */
inline constexpr MemorySize OAM_SIZE = 0x00a0;
/** OAM end address */
inline constexpr MemoryAddr OAM_END = OAM_BEGIN + OAM_SIZE - 1;

/** Memory-mapped IO begin address */
inline constexpr MemoryAddr IO_BEGIN = 0xff00;
/** Memory-mapped IO size */
inline constexpr MemorySize IO_SIZE  = 0x0080;
/** Memory-mapped IO end address */
inline constexpr MemoryAddr IO_END = IO_BEGIN + IO_SIZE - 1;

/** High RAM begin address */
inline constexpr MemoryAddr HRAM_BEGIN = 0xff80;
/** High RAM size */
inline constexpr MemorySize HRAM_SIZE = 0x007f;
/** High RAM end address */
inline constexpr MemoryAddr HRAM_END = HRAM_BEGIN + HRAM_SIZE - 1;

/**
 * Interrupt Enable Register.
 */
inline constexpr MemoryAddr IE_REG = 0xffff;

}  // namespace memory
