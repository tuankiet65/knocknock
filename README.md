# knocknock

A WIP Game Boy emulator, written during my ~~downtime at work. The name is
choosen after the company I'm doing an intern at (exactly which company is left
as an exercise for the reader)~~ downtime in school, I've left that company for
undergraduate studies.

Current status:
* Pass all blargg's cpu_instrs tests.
* Runs some games with none or minor graphics glitches.

## Features

### Core library

- CPU
    - [x] All instructions implemented
    - [x] Cycle-accurate decoder
    - [ ] Cycle-accurate instructions
- Memory bank controllers
    - [x] FlatROM aka no memory bank controller
    - [x] MBC1 (verified using mooneye)
    - [x] MBC2 (verified using mooneye)
    - [ ] MBC3
    - [ ] MBC5
- Peripherals
    - [x] Serial
    - [x] Joypad
    - [x] Clock
    - [ ] Timer (partly implemented, not accurate)
- PPU
    - [x] Tile
    - [x] Tile Map
    - [x] Tile Data
    - [x] OAM Sprite
    - [ ] Renderer (partly implemented, not accurate)
- [ ] Audio

### knocknock-sdl
- [x] Memory viewer
- [x] Memory map viewer
- PPU debugger
    - [x] Tile Data viewer
    - [x] Window / background viewer
    - [x] Rendered frame viewer
    - [ ] OAM viewer
    - [ ] Jump to next scanline
- CPU debugger
    - [ ] Disassembly viewer
    - [ ] Register viewer
    - [ ] Step into, over, to next instruction, ...
    - [ ] Breakpoints
- [ ] Interrupt viewer

## Building knocknock

### Dependencies
You'll need:
* CMake.
* A recent C++ compiler that supports C++17 (tested: GCC 10.0, Clang 10.0, latest Microsoft Visual C++)
* Doxygen for generating documents.
* SDL2 (only needed for the SDL frontend)
* FFmpeg (only needed for the SDL frontend)

Some dependencies are bundled with the source, but you can use flags to force knocknock to use the
system version instead:
* fmt (flag: `USE_SYSTEM_FMT`)
* glog (flag: `USE_SYSTEM_GLOG`)
* Catch2 (flag: `USE_SYSTEM_CATCH2`) (only needed for tests)

### Linux
Create a build directory and change to it:
```
mkdir build
cd build
```

Then invoke CMake and make:
```
cmake -DCMAKE_BUILD_TYPE=BUILD_TYPE [additional options] ..
make -jCORES
```

Replace `CORES` with the number of CPU cores and `BUILD_TYPE` with one of these build types:
* `Release` for a release build (recommemded, most performance)
* `MinSizeRel` for a release build, optimized for binary size (might be faster than Release)
* `Debug` if you want to debug the emulator (and please send a pull request or file an issue if you find any bugs!)

This will only build the core `knocknock` library. To also build other components, use these additional options:
* `-DWITH_SDL_FRONTEND=On` to build the SDL GUI frontend (recommended, this is probably what you want)
* `-DWITH_CLI_FRONTEND=On` to build the CLI (command-line) frontend. This frontend is pretty useless, as it emulates the
  ROM, but only prints out the serial output to stdout.
* `-DWITH_TESTS=On` to build the unittests.

## References
* [Game Boy: Complete Technical Reference](https://gekkio.fi/files/gb-docs/gbctr.pdf) by Joonas Javanainen
* [Game Boy(tm) CPU Manual](http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf), various authors, compiled DP
* [Gameboy Development Wiki](http://gbdev.gg8.se/wiki/articles/Main_Page)
* [GameBoy Memory Map](http://gameboy.mongenel.com/dmg/asmmemmap.html) by Randy Mongenel
* [Awesome GBDev](https://github.com/gbdev/awesome-gbdev)
* https://floooh.github.io/2017/12/10/z80-emu-evolution.html
* http://www.z80.info/decoding.htm
* https://gbdev.github.io/gb-opcodes/optables/
* https://raw.githubusercontent.com/floooh/emu-info/master/z80/z80ins.txt
* https://github.com/floooh/emu-info
* http://rvbelzen.tripod.com/z80prgtemp/
* http://rvbelzen.tripod.com/z80prgtemp/fig2-27.htm
* https://hacktix.github.io/GBEDG/timers/ (timers)
* https://github.com/AntonioND/giibiiadvance/blob/master/docs/TCAGBD.pdf (HALT instruction)
