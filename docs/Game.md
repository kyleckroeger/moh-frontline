# Game code

Frontline's own code (EA's engine and the game) is reconstructed in `src/game/`
as `reconstructed_game` units. No public source matches it byte for byte; the
references are semantic.

## Compiler profile

Established on `bstimer.cpp`, whose six functions (2,020 bytes) and exception
tables verify:

- CodeWarrior GC/1.3 or later (1.3, 1.3.2 and 2.x produce identical code for
  this file; GC/1.2.5n, which builds the Dolphin SDK, does not). Units use
  GC/1.3.2 until a file distinguishes the release. A working profile is not
  proof of the original release.
- `-O4,p` with instruction scheduling on, `-use_lmw_stmw on`,
  `-Cpp_exceptions on`, `-inline auto` (not deferred: functions are emitted in
  source order and only earlier definitions are inlined), `-RTTI off`,
  `-str reuse`, `-enum int`, `-fp hardware`, default small-data thresholds.

### Additional evidence from `propdat.cpp` (not yet verified as a unit)

Per-function comparisons of a `propdat.cpp` candidate (38 of its 43 functions
match; the unit is not accepted) add three observations. `bstimer.cpp` compiles
identically with and without both flag changes, so its manifest is unaffected.

- **`-fp_contract on`.** Float code uses fused `fmadds`/`fnmsub` (distance
  sums, the inline `sqrtf` refinement steps). Without the flag the compiler emits
  separate multiplies and adds.
- **`-str reuse,readonly`.** The file's string literals (the MMG warnings,
  `"Unknown property type\n"`) are in `.rodata`, after a 108-byte header-defined
  `dwi::dwi_prime_list`. Without `readonly` they go to `.data`.
- **Release.** GC/1.3 and GC/2.0p1 produce different code for this file;
  GC/1.3.2, 1.3.2r, 2.0, 2.5, 2.6 and 2.7 produce identical code. This narrows the
  candidate releases but still does not identify the original one.

Other compiler behaviour that the source has to reproduce:

- Templates not declared `inline` (`offsetPtr<T>`) are instantiated at the end of
  the unit as weak functions and are never auto-inlined; `inline` templates are.
- Stack slots for inlined temporaries are grouped by inline depth (deepest
  lowest), in source order within a depth. In the endian-swap code this determines
  the field types and which small inline wrappers exist.
- A by-value parameter or a user-declared copy constructor keeps a `CVector3` in
  memory, which changes both scheduling and whether multiply-adds are fused.

## References

Rising Sun (GR8E69, [moh-rising-sun](https://github.com/lifewillbeokay/moh-rising-sun))
is EA's next game on the same engine. Its reconstruction is ProDG code, so its
bytes never match, but its structure and descriptive names guide Frontline's
source. About 153 KB of Frontline's unfinished functions share a name with a
Rising Sun function; Rising Sun has reconstructed about 13 KB of those. Names
and layouts it marks as descriptive stay descriptive here.

## Method

1. Read the function's instructions and relocations; recover only the accessed
   fields and strides. Partial layouts are "views": unknown bytes stay
   `unknownNN` arrays and comments say what is established.
2. Write the function, preferably in the reference's structure. Rising Sun's
   small helpers are often inlined in Frontline (`bstimer.cpp` inlines
   `BSTimerRemoveTimerEvent`, `BSGetFreeTimerEvent` and the duplicate/latter
   removal loops); keeping them as `static` helpers reproduces the code.
3. Compare per function, then tune what the compiler leaves to the source:
   - Register allocation follows declaration order. In `bstimer.cpp` the
     declaration order of locals decided every remaining difference; search
     small permutations rather than guessing.
   - Store order through one pointer follows statement order.
   - A `switch` should list only the cases the code tests.
   - An extern object's size decides small-data addressing
     (`g_pMemBlockAllocator` is 28 bytes, so it is not in `.sbss`).
4. Draft the unit with `tools/port_unit.py`. It places `extab`/`extabindex` by
   their references to kept functions and keeps only the kept functions'
   entries.

## Units

| File | Functions | Bytes | Reference | Notes |
| --- | ---: | ---: | --- | --- |
| `bstimer.cpp` | 6 | 2,020 | Rising Sun `src/script/timer_*.cpp` | `BSObject` is a view (`queueIdentity` at `+16`); `BSTimerEvent_struct.ownsEventMemory` is a byte, `DoWeOwnThisMemory` returns `bool`; `BSInitTimer` links events forward |
| `propdat.cpp` (fragment) | 14 | 5,492 | Rising Sun `src/bpd/endian.cpp` | `0x80040c6c`-`0x800421e0`: the `EndianSwap` conversions (see "Endian conversions") and the machine-gun lookups (`SearchForClosestMachineGun`, `IsMGUsed`, `MarkMGAsUsed`) with the file's first four `.sdata2` constants. `sqrtf` is the SDK `math.h` `extern inline` form (volatile result). `CVector3` behaves as 8-byte aligned. The rest of the file stays original context |

## Endian conversions

`propdat.cpp` starts with `EndianSwap` for each property record. As in Rising
Sun, each converted field gets one `ChangeEndian` call. Frontline inlines every
conversion, so the overloads are recovered from the code they leave:

- `ChangeEndian(short&)`, `ChangeEndian(int&)` and `ChangeEndian(unsigned int&)`
  copy the value to a local byte array and swap it. The 32-bit form is written
  as two explicit pair swaps; a loop reproduces the bytes of most functions but
  moves the scheduler's split points in others.
- A `ChangeEndian` template converts other 32-bit types (pointers, `unsigned
  long`) through `int`. Floats go through `ChangeEndian(float&)`, which calls
  `EndianSwap(float&, bool)` (a weak function in this file); its `bool` is
  unused and its meaning is unknown.
- Each inline level puts its temporaries in a separate group of stack slots
  (deeper levels lower, source order within a level). The slot order of each
  target function therefore shows which fields are direct `int`s, which go
  through the template and which are floats, and that is how the views' field
  types were chosen. Where only one group appears, the types are not
  established.

The fragment covers only these functions. The remaining propdat functions (BSP
patch-up, trigger setup, player starts and `PatchUpAllPropertyData`) are drafted
in scratch and are not accepted.

## EA library units (`src/ea/`)

EA's shared libraries (the "REAL" runtime, sound and resource code linked into
Frontline) are reconstructed in `src/ea/` as `reconstructed_game` units, from
the disassembly alone (no reference source). They use the game profile with
two differences, both shown by the verified units:

- `-Cpp_exceptions off`: functions that call out (`CPU_detect`, `MEM_initadr`)
  have no `extab`/`extabindex` entries.
- `-str reuse` without `readonly`: string literals are in `.data`/`.sdata`
  (`"Gekko PowerPC"`, `"RAM"`).

Some files are named `.c` but have C++-mangled names (`sfir8.c`); they are built
with `-lang c++`. Struct and parameter types are views; where the code does not
establish a type, the source comments say so.

| File | Functions | Bytes | Notes |
| --- | ---: | ---: | --- |
| `fontnull.cpp` | 1 | 4 | Static `NULL_draw` referenced by the global `FONTnulldriver` table (20 bytes; only the draw entry is established) |
| `rcmp_mpc_codec_chunk_types.cpp` | 1 | 28 | Loop over a one-entry anonymous-namespace `ChunkTypes` table (`'MPCh'`) |
| `sfir8.c` | 1 | 40 | Clears an eight-float FIR history |
| `cpudetect.cpp` | 1 | 80 | Fills the static `cpuinfo` from the bus clock word at `0x800000FC` |
| `meminitadr.cpp` | 1 | 80 | One `MEMCLASS_create` call (`"RAM"`, 32-byte alignment) |

