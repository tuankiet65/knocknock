# knocknock

A WIP Game Boy emulator, written during my ~~downtime at work. The name is
choosen after the company I'm doing an intern at (exactly which company is left
as an exercise for the reader)~~ downtime in school, I've left that company for
undergraduate studies.

Current status:
* Can load a cartridge from file and parse its header.
* The decoder is able to decode instructions, however the CPU can't execute them
  (yet)

## Build
You'll need:
* CMake
* A recent C++ compiler that supports C++17 (tested: GCC)
* Doxygen for generating documents.

## Roadmap
* Implement all instructions
* Implement all memory modules
* Implement all peripherals: timers, interrupts
* Integrate ImGui
* Documents

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
