# Micro-op

Looking at a multi-cycle instruction, we can see that that instruction is
essentially broken down into multiple sub-instructions, each of them is executed
at each cycle. For example, `ADD A, (HL)`, which takes 2 cycles, can be broken
down to 3 sub-instructions:
* Read opcode
* Perform a memory read at the address pointed to by HL into a temporary
  register
* Perform an addition from the temporary register to the accumulator

We call these sub-instructions micro-operations (or micro-op, uop). Thus
what we're going to do is to break all multi-cycle instructions into a bunch
of uop, which is then executed by the CPU. This ensures that the emulator is
cycle-accurate to a degree.

Looking at the example again, we can see that `ADD A, (HL)` is broken down into
three uop, yet it only takes 2 cycles. What gives? Turns out the CPU has
something called fetch-execute overlap. Let's assume that `I1`, `I2` and `I3`
take 2 cycles to complete, yet they must be broken down into 3 uops:
```
F: fetch; E: execute; ....: don't care

    | n      | n + 1  | n + 2  | n + 3  | n + 4  | n + 5  | n + 6  |
I1: |FFFFFFFF|EEEEEEEE|EEEEEEEE|
I2:                   |FFFFFFFF|EEEEEEEE|EEEEEEEE|
I3:                                     |FFFFFFFF|EEEEEEEE|EEEEEEEE|
```

We can see while each instruction is executed in three clock cycles, thanks to
the fetch-execute overlap, it only takes 7 clock cycles to execute three of them.
It's 7 and not 6 because durin the fetching of I1, there is no other instruction
executing.

# List of micro-ops

## Shortcuts:
* `Imm8`: Register holding the immediate 8-bit value, if any
* `Imm16`: Register holding the immediate 16-bit value, if any
* `Tmp8`: Temporary 8-bit register
* `Tmp16`: Temporary 16-bit register
* `reg8`: All 8-bit registers (`A, B, C, D, E, H, L, Tmp8, Imm8`)
* `Flag`: All flags: `Z, NZ, C, NC`

## Miscellaneous
* `NOP`: Does nothing
* `CSKIP Flag`: Skip the next instruction if `Flag` is false.

## Jumps
* `JP Imm16`: Jumps to Imm16
* `JP HL`: Jumps to HL
* `JR Imm8Sign`: Performs a relative jump by `Imm8Sign`.

## Logic
* `CP reg8`: Compare A with reg8

## Load
* `LD tmp8, (HL)`: load value from (HL) into tmp8
