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
| `surfacetype.cpp` | 5 | 116 | — | Lookups into the collision database's 12-byte surface table (`CDB +0x34`); the shoot-through flag is a one-bit field |
| `MallocInit.cpp` | 1 | 144 | — | Heap set-up from the OS arena (less 24 MB above that size) |
| `real_bridge.cpp` | 3 | 196 | — | REAL runtime start-up, update and shutdown calls |
| `system.cpp` | 3 | 208 | — | `SysInit`/`SysShutdown` with the module-active flag and a 36-byte critical section |
| `gcSystem.cpp` | 5 | 232 | — | Critical sections over `MUTEX_create`/`MUTEX_destroy` (a 28-byte mutex, owner thread and count) and `SysInitDependent` (`OSInit`, `DVDInit`) |
| `dmgeom.cpp` (fragment) | 1 | 44 | — | `DMGeomSetNodeState` only; `DMGeomResetState` is drafted but not matched |
| `LinkedList.cpp` | 8 | 424 | — | Singly linked list (head, tail, count) and its element |
| `texpack.cpp` | 6 | 428 | — | Texture pack lookup (`bsearch` over 16-byte names) and offset fix-up; with its weak `offsetPtr` instantiations. Its own copy of `offsetPtr<void>` was dropped by the linker in favour of propdat's, so the source declares that specialisation instead of instantiating it |
| `anim.cpp` | 11 | 532 | — | Animation module start-up, the user-opcode callback stacks and the global `g_AnimDB` (constructed by `__sinit_anim_cpp`) |
| `orient.cpp` | 2 | 300 | — | Fixed-point (one turn = 0x1000000) shortest-way angle interpolation, `extern "C"` names; the 64-bit product shape fixes where the difference is taken |
| `memory.cpp` | 7 | 748 | `DWI_alloc`/`DWI_allocalign` and the global `operator new`/`new[]`/`delete`/`delete[]` on the REAL heap; `DWI_alloc` is inlined into the `new` forms (hence two `memset` calls), and the `throw()` deletes keep their exception-spec frames |
| `vector.cpp` | 2 | 440 | `CVector3::Constrain` (spherical interpolation toward a target; Frontline's version takes a const target and has no opposite-vector case, unlike Rising Sun's) and `CVector2::Rotate`; the scaled target is an inline `float * CVector3` temporary |
| `framework.cpp` | 2 | 8 | Empty `FRAMEWORK::Init`/`Restore` |
| `isexportdefs.cpp` | 1 | 16 | `NullifyScreenAndLibrary` |
| `isShellGroup.cpp`, `ispausegroup.cpp`, `isShellLibrary.cpp`, `ispauselibrary.cpp` (fragments) | 4 | 56 | One-line selectors of the shell/pause screen and library tables; the files' string data is not reconstructed |
| `trig.cpp` (fragment) | 4 | 272 | `MathArcTan2`, `MathSinCos`, `MathCosf`, `MathSinf`. `MathLLAngleInit` (Taylor tables) is left out: 4 instructions differ (element 0 of `_Math_TaylorConst` is addressed through a copied base register), and a 12-byte object the linker stripped sits between the tables |

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

The sound library (`SNDI_*`, `SFILTER_*`) never fuses multiply-adds, so its
units are built without `-fp_contract on`, and with `-use_lmw_stmw off`
(`SFILTER_splitter` saves five registers through `_savegpr_27`). A sound filter starts with its
process function, a restore function (`+0x4`, where present) and an optional
input filter (`+0x8`) that is run first; the filter files share that layout.

Some files are named `.c` but have C++-mangled names (`sfir8.c`); they are built
with `-lang c++`. Struct and parameter types are views; where the code does not
establish a type, the source comments say so.

| File | Functions | Bytes | Notes |
| --- | ---: | ---: | --- |
| `fontnull.cpp` | 1 | 4 | Static `NULL_draw` referenced by the global `FONTnulldriver` table (20 bytes; only the draw entry is established) |
| `rcmp_mpc_codec_chunk_types.cpp` | 1 | 28 | Loop over a one-entry anonymous-namespace `ChunkTypes` table (`'MPCh'`) |
| `sfir8.c` | 2 | 244 | Eight-tap symmetric FIR (eight history samples, five coefficients) and its reset |
| `cpudetect.cpp` | 1 | 80 | Fills the static `cpuinfo` from the bus clock word at `0x800000FC` |
| `meminitadr.cpp` | 1 | 80 | One `MEMCLASS_create` call (`"RAM"`, 32-byte alignment) |
| `sfamplf.c` | 3 | 492 | Amplifier filter: process (gain loop, unrolled by the compiler), create, modify (`parameter / 256`) |
| `sflpffir8.c` | 3 | 356 | Low-pass FIR; its process filters the requested count after the input filter succeeds (the other two use the input's returned count). Modify: `2 * (p0 >> 8) / (p1 >> 8)` |
| `sfhpffir8.c` | 3 | 348 | High-pass FIR. Modify: `(p0 >> 7) / (p1 >> 8)` |
| `sfbpffir8.c` | 3 | 400 | Band-pass FIR; modify reads its parameters into locals before either store |
| `sfsrc.c` | 3 | 152 | Source filter: copies from a caller buffer and advances it |
| `sfft24.c` | 2 | 112 | Float to integer conversion clamped to +/-32767 |
| `sfsplit.c` | 3 | 384 | Splitter: alternately pulls from its input (keeping a copy) and replays the copy |
| `sfmixer.c` | 3 | 632 | Mixer: adds a second input's output into the first's |
| `sflpf.c` | 3 | 836 | One-pole low-pass: `y = y * feedback + gain * in` |
| `SNDI_sin.c` | 1 | 124 | Taylor series to x^13 after reducing below 2*pi |
| `SNDI_cos.c` | 1 | 124 | Taylor series to x^12 after reducing below 2*pi |
| `SNDI_root1x.c` | 1 | 116 | Binomial series for sqrt(1 + x); terms are separate variables summed in one expression |
| `spantoaz.c` | 1 | 20 | Pan-to-azimuth table lookup (the 128-entry table is this file's data) |
| `ssine.c` | 1 | 124 | Integer sine from a 257-entry quarter-wave table |
| `SNDI_findprime.c` | 1 | 152 | First prime at or above `a * b / 1000`; integer root by stepping `i * (i - 1)` |
| `SNDI_mult16.c` | 1 | 44 | `SNDI_findprime(a, b / 16) * 16` |
| `sexithndl.c` | 1 | 32 | Calls `SNDSYS_restore` |
| `slinkmix.c` | 1 | 88 | Installs the seven main-CPU mixer entry points |
| `idct.cpp` | 1 | 232 | Clip-table set-up of the MPEG-2 reference decoder's fast IDCT (`iclp[i]` clamped to -256..255) |
| `inittmr.cpp` | 5 | 524 | REAL timer set-up: a periodic OS alarm at `bus clock / 4 / hz` posts to the timer thread, which runs eight handler slots; the tick counters are volatile |
| `timerthread.cpp` | 4 | 352 | The timer thread's message loop and its queue, stack and thread objects |
| `exit.cpp` | 4 | 488 | REAL exit handlers: a 64-entry table run in reverse on restore; `REAL_exit` inlines `REAL_restore` |
| `ssysreal.c` | 3 | 172 | Hooks the sound system into REAL (system task, abort hook, exit handler); its local static gets a run-time guard (`init$`) |
| `initosalloc.cpp` | 4 | 380 | OS heap set-up with the Dolphin arena-rounding idiom, and MSL's `__sys_alloc`/`__sys_free` on the REAL allocator |
| `abortmsg.cpp` | 2 | 400 | `REAL_abortmessage` / `SYSTEM_abortmessage`: format into a 512-byte buffer with CodeWarrior's `__builtin_va_info`, then the abort hook or print plus `REAL_exit` |
| `memclass.cpp` | 2 | 592 | `MEMCLASS_create` lays out the LOW/free/HIGH blocks of a REAL memory class, plus `MEMCLASS_remove`; built like the sound library (`-use_lmw_stmw off`, `_savegpr_21`). `MEMBLOCK`/`MEMCLASS` members are inferred from offsets |
| `memunused.cpp` | 1 | 64 | `MEM_totalunused` (its own file record starts at 0x8014f4b8, between `memclass.cpp` and `memalloc.cpp`) |
| `meminit.cpp` | 1 | 136 | `MEM_init`: the largest OS-heap block becomes the default class; `MEM_restore` is registered as an exit handler |
| `memrestore.cpp` | 1 | 372 | `MEM_restore`: clears duplicate `memclass` entries, then removes the classes and frees their OS-heap blocks |
| `debugger.cpp`, `fontdriver.cpp`, `timer.cpp` | 4 | 28 | `DEBUG_break` (empty), `FONT_installdriver`, `TIMER_gettick`/`TIMER_getfrequency` |
| `coda.cpp` | 2 | 16 | `SND::CODASetNew`/`CODASetDelete` allocation hooks |
| `memclear.cpp`, `rcmp2real.cpp` | 2 | 76 | `MEM_clear`; `RCMP::RCMP_SYSTEM::SetREALDefaults` routes RCMP allocation to `MEM_allocalign`/`MEM_free` |
| Sound one-function files: `sbhdrsze.c`, `smemhigh.cpp`, `sover.c`, `sstqmem.c`, `sstqreqi.c`, `sinitut.c`, `spoutlat.c`, `smixtmul.c`, `sstcrtap.c` | 9 | 428 | Small API wrappers; fields of the `sndgs`/`sndmix` globals are read through inferred offsets because their layouts are unknown |
| `fontchar.cpp`, `pad.cpp` | 4 | 156 | `FONT_bsearch` (character table binary search); `PAD_init`/`PAD_getdataptr`/`PAD_update` over the platform pad layer |
| Sound stream and control files: `sinit16.c`, `sinitxa.c`, `sgetpvol.c`, `sstsetgl.c`, `smixfram.c`, `sstgetpv.c`, `ssthighp.c`, `sstlowp.c`, `sstpmult.c`, `sstrmdry.cpp`, `ssttmul.c`, `sstvol.c`, `sstfxlev.c`, `splysdef.c`, `srrange.c`, `sstop.c` | 16 | 1,308 | Stream setters store the value in the stream record and forward it to the voice; records and globals are read through inferred offsets |
| `fontcreate.cpp`, `fontinit.cpp` | 4 | 252 | Font creation through the current driver's hooks; the standard font is created once and destroyed at exit |
| `signals.cpp`, `mutex2.cpp`, `memmove.cpp` | 8 | 480 | REAL signals (one-slot OS message queue) and mutexes (`MUTEX_destroy` poisons the record with 0xdeadbeef); `MEM_move` copies backwards on overlap |
| `memfill.cpp` | 1 | 492 | `MEM_fill`: align with byte/halfword/word stores, `while (size >= 32)` block stores (the compiler turns it into a counted, unrolled loop), then the tail |
| `memcopy.cpp` | 1 | 1,332 | `MEM_copy`: byte, halfword or word copy by the relative alignment of source and destination (byte temporaries are `int`) |
| `initvblt.cpp` | 1 | 104 | `ttDoVTimerMsg`: vertical-blank tick and its eight subscribers |
| Sound voice and bank files: `spatkey.c`, `spktctoh.c`, `sstovrhd.c`, `smasterv.c`, `spitch.c`, `sfxlevel.c`, `sbplay.c`, `sautovol.c` | 9 | 1,300 | `iSNDpatchkey` (the per-patch voice iterator), master volume, pitch/effect/fade controls, bank playback |
| `s3dlow.c`, `slib.c`, `smixptch.c` | 3 | 604 | `SND3dpos`; `iSNDcalcvol` (four volumes out of 127, divided by 127³, then optional key-scale and curve tables); `MIX_setpitch` creates the channel's resampler on first use |
| `smixhip.c`, `sbpatinf.c`, `susercb.c`, `sclnt100.c`, `sgetdata.c`, `sststat.c` | 9 | 1,410 | High-pass stage, patch info, the user-data and 100 Hz client lists (`sndgs` viewed through an inferred struct), big-endian sample reads, stream status |
| `slinklst.c`, `ssysserv.c`, `sgettag.c`, `sstrstat.c` | 10 | 1,316 | Doubly linked lists, sound server clients and `SNDSYS_service`, the header tag reader, request status (64-bit helpers return `SINT64` in r3/r4) |
| `sfilter.c`, `supf.c` | 6 | 720 | Mixer filter chains (insert by priority, remove, connect ports) and the 16-bit PCM unpacker |
| `sbremove.c` | 1 | 360 | `SNDbankremove`: stop the bank's voices, run user-data callbacks per timbre, free sample memory (recursive for -1) |
| `seffect.c` | 3 | 568 | Effect bus lookup (`switch` on the bus flags), bus set-up and master send levels |
| `sbadd.c` | 2 | 576 | `SNDbankadd` (download samples in 4 KB pieces, resolve patches) and the per-timbre user-data callback |
| `smixc.c`, `sfxrev.c` | 2 | 636 | `mixc` (scaled add into a mix buffer) and `MIXI_reverbblock` (comb filter with a one-pole low-pass in the feedback, 1e-30 anti-denormal offset) |
| `sdfx.c` | 3 | 732 | Platform effects: reverb select/restore (inlined into `SNDPLATFORM_fxinit`) and per-voice send levels as DSP aux-bus levels or software wet gain (scales 1/127 and 1/(127·32767)) |
| `sx87d16.c` | 1 | 392 | `decode16x87`: 16-bit PCM to float |
| More sound files: `sbvalid.c`, `sst3dpos.c`, `ssthold.c`, `sctlfilt.c`, `shipass.c`, `slowpass.c`, `sstgetrp.c`, `sstopall.c`, `sstautov.c`, `sattrdef.c`, `scalcfx.c`, `sballoc.c`, `sbhdrcpy.c`, `sctrldry.cpp`, `stimemul.c` | 17 | 1,768 | Bank slots, voice controls applied to each platform voice (`iSNDpatchkey` loop), stream setters and defaults; record members are inferred views |
| `memblock.cpp` | 1 | 44 | `MEM_initblock` writes a block header ('BM', flags, size, neighbour links); the name and tail-size arguments are not stored |
| `memlist.cpp` | 5 | 464 | Address-ordered circular free list (`FREE_find`, `FREE_findlargest`, `FREE_gettotalfree`, `FREE_add`, `FREE_remove`) with the class's 'BS' sentinel; `FREE_findlargest` needs the operand order of `0 > size - 1 ? 0 : size - 1` for its branchless max |
| `memstd.cpp` | 1 | 8 | `MEM_size` |
| `systask.cpp` | 4 | 772 | `SYNCTASK_*`: a 16-entry table of tick-scheduled callbacks with guarded local statics; entry layout inferred |
| `bmem.cpp` | 4 | 680 | Block pools (`BPoolMan`): pool chain and circular free list; the static `AddNewPool` is defined last so it is not inlined. Members inferred |
| `syncfile.cpp` | 9 | 1,100 | Synchronous FILESYS open/read/close/size/addbig/delbig/exists: chunked (0x8000) block IO driven by a completion callback; the context's polled fields are volatile, and `#pragma dont_inline` keeps `syncblockio` out of `FILESYS_readsync` as in the original |

