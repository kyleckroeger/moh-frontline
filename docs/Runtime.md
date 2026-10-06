# Metrowerks runtime libraries

Frontline links the Metrowerks Standard Library (MSL C), the PowerPC EABI
runtime (C++ support, exceptions, integer helpers), the fdlibm-based math
library, MetroTRK and the debugger stubs (`DebuggerDriver.c`, `odenotstub.c`).
These sit at `0x80131000`–`0x801439e4` and in parts of `.init`.

## References

Function sizes were compared between Frontline and other decompiled MWCC
GameCube games before any source was chosen
(`scratch/refs/sizecmp.py`). The Wind Waker's library build matched 240 of
242 shared functions by size, Prime 124 of 130, Pikmin 97 of 194 and Sunshine
93 of 186. The Wind Waker is therefore the primary reference, and Prime fills
in files it lacks or links in a different revision. Both are CC0; see
[Licensing.md](Licensing.md).

| Tree | Upstream | Used for |
| --- | --- | --- |
| `src/tww/` | [zeldaret/tww](https://github.com/zeldaret/tww) `a1854d4` | MSL C, math, runtime, MetroTRK, `__ppc_eabi_init.cpp`, `OSReboot.c`, `DebuggerDriver.c`, `odenotstub.c` |
| `src/prime/runtime/` | [PrimeDecomp/prime](https://github.com/PrimeDecomp/prime) `32020a0` | `alloc.c`, `ansi_fp.c`, `ctype.c`, `qsort.c`, `rand.c`, and `__cvt_sll_flt` |

Units use the reference's own compiler profile: the Wind Waker's
`cflags_runtime` with GC/1.3 for MSL and the runtime, GC/1.2.5n for
`DebuggerDriver.c`, GC/1.3.2 with `cflags_trk` for MetroTRK, GC/1.3.2 with
`cflags_dolphin` for `odenotstub.c`, and GC/1.2.5n with `cflags_dolphin` for
`__ppc_eabi_init.cpp`;
Prime's `cflags_runtime` (with `-gccinc`) and GC/1.3 for Prime files. Per-file
extra flags follow the reference's `configure.py` (for example `-inline noauto`
for `alloc.c`). `-DVERSION=0` selects the Wind Waker demo variants, whose map
matched Frontline best. A working profile is not proof of the original
compiler release.

Files with only global symbols have no file record in the target. They are
drafted with `port_unit.py --no-file-record` and their manifests carry no
`original_file`.

## Local modifications

| File | Change | Evidence |
| --- | --- | --- |
| `tww/.../MSL_Common/Src/printf.c` | Add `vsprintf` (a `vsnprintf` wrapper) before `sprintf`, as the older MSL in Pikmin and Prime defines it | Global `vsprintf` at `0x8013db94` (120 bytes) between `sprintf` and `vprintf`; with `-inline deferred` functions are emitted in reverse source order |
| `tww/.../MSL_Common/Src/printf.c` | `__StringWrite` and `__FileWrite` are global, not `static` | Global symbols at `0x8013dd54` and `0x8013ddc0` |
| `tww/.../MSL_Common/Src/abort_exit.c` | `__aborting` and `__console_exit` are global, not `static`. Adds `abort` (absent upstream): `raise(SIGABRT)`, then the atexit/console part of `exit`; it precedes `exit` in the source because `-inline deferred` emits functions in reverse order | Global `.sbss` objects at `0x8034f318` and `0x8034f324`; `__atexit_curr_func` stays file-local as in the target |
| `tww/.../Runtime/Src/runtime.c` | Add `__cvt_sll_flt` (`asm`, copied from Prime's `runtime.c`) between `__shr2i` and `__cvt_dbl_usll` | Global `__cvt_sll_flt` at `0x80138dec` (180 bytes) between them; Prime's whole file does not fit, because its `__save_fpr`-family helpers are global where the target's are file-local |
| `tww/.../Runtime/Src/NMWException.cp` | Declare `terminate`, `unexpected` and their setters in namespace `std` (the file declared them at global scope), and give `__throw_catch_compare` C linkage | `duhandler__3stdFv` at `0x80138878` is 40 bytes and calls through `thandler` (`lwz r12,-29704(r13)`), so it inlines `std::terminate`; the global-scope declaration made it call an external `::terminate`. Global `__throw_catch_compare` at `0x801385fc` is unmangled |
| `tww/dolphin/os/__ppc_eabi_init.cpp` | Add the `.init` functions `__init_hardware` and `__flush_cache` (`asm`, from dolsdk2001's `__ppc_eabi_init.c`, with C declarations of `__OSPSInit`/`__OSCacheInit`), and make `__init_cpp` `static` | `__init_hardware` (`0x80007330`) and `__flush_cache` (`0x80007350`) belong to the `__ppc_eabi_init.cpp` record with `__init_user`, file-local `__init_cpp` and `_ExitProcess`; the 2001 `.c` file also defines `abort`/`exit`, which Frontline takes from MSL |
| `tww/TRK_MINNOW_DOLPHIN/Portable/serpoll.c`, `tww/PowerPC_EABI_Support/MetroTRK/trk.h` | `TRKFramingState.receiveState` is a `u8`, as in [Pikmin's](https://github.com/doldecomp/pikmin) CC0 `trktypes.h`. `TRKGetInput` reads the command byte and passes commands below `0x80` to `TRKProcessInput` (inlined) or releases the buffer, as in Pikmin's `serpoll.c`. The unit is a fragment: Frontline's `TRKTestForPacket` is a 696-byte framing state machine (`TRKReadUARTPoll`, `0x7E`/`0x7D` escapes) that neither reference has, so the TWW body was removed and the function is declared only | `TRKInitializeSerialHandler` stores `receiveState` with `stb` at `+8`; `TRKGetInput` at `0x80132050` calls `TRKTestForPacket`, `TRKGetBuffer`, `TRKSetBufferPosition`, `TRKReadBuffer1_ui8`, compares with `0x80`, then builds and posts the event in place. The fragment covers `0x80131fd4`–`0x801320e8` (276 bytes) and all of the file's `.bss`; `TRKTestForPacket` (`0x801320e8`, 696 bytes) remains |
| `tww/TRK_MINNOW_DOLPHIN/Portable/notify.c` | `TRKDoNotifyStopped` takes a `u8` command, appends it to the message before adding the stop or exception information, and declares its locals in the order that gives the target's stack slots. The structure follows [Pikmin's](https://github.com/doldecomp/pikmin) CC0 `notify.c` with the append in place of `TRKWaitForACK`. The append is an `inline` definition of `TRKAppendBuffer1_ui8` (msgbuf.c's body) in this file; where the original defines that inline copy is unknown | `TRKDoNotifyStopped` at `0x80135600` (216 bytes) checks `position >= 0x880` (error `0x301`), stores the byte at `data[position]` and increments `position` and `length` inline, then compares `(u8)cmd` with `0x90`; `msg` is at `8(r1)` and `bufIdx` at `12(r1)` |
| `tww/TRK_MINNOW_DOLPHIN/Portable/dispatch.c` | `TRKDispatchMessage` masks the command byte (`command &= 0xFF`, as in [Pikmin's](https://github.com/doldecomp/pikmin) CC0 `dispatch.c`), and `gTRKDispatchTable[23]` is `TRKDoSetOption` instead of `TRKDoUnsupported` | `clrlwi r3,r4,24` before the size compare in `TRKDispatchMessage` at `0x8013242c`; the `.data` relocation at `gTRKDispatchTable+0x5c` (`0x80190404`) is `TRKDoSetOption` |
| `tww/TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk.c` | `__TRK_reset` calls `__TRK_copy_vectors` instead of `OSResetSystem`, and `__TRK_copy_vectors` takes the exception mask through `TRKTargetTranslate(0x44)` and copies every enabled vector (no skip of vector 4). Both follow [Pikmin's](https://github.com/doldecomp/pikmin) CC0 `dolphin_trk.c` | `__TRK_reset` at `0x8000930c` in `.init` (260 bytes) inlines `__TRK_copy_vectors`, `TRK_copy_vector` and `TRKTargetTranslate`; its loop tests `1 << i` for `i` 0 to 14 with no `i != 4` compare. The four `.text` functions at `0x80137080` (320 bytes) are unchanged |
| `tww/dolphin/os/OSReboot.c` | Frontline's older revision: `Run` is a `fralloc` `asm` function that calls `OSDisableInterrupts` and `ICFlashInvalidate` itself, the form of [Pikmin's](https://github.com/doldecomp/pikmin) CC0 `OSReboot.c`, so `__OSReboot` no longer calls them; `__OSReboot` stores `resetCode` (not 0) at `0x817FFFFC`. Built with the SDK compiler (1.2.5n) and SDK flags | `Run`, `Callback` and `__OSReboot` at `0x8011457c` (536 bytes); `__OSReboot` writes `r29` (the first argument) to `-4(0x81800000)` and calls `Run`, which has a frame and both calls. `ReadApploader` is inlined and `OSSetSaveRegion` is not linked |
| `prime/runtime/alloc.c` | Add `malloc` (a `__pool_alloc` wrapper, MSL's form) and move `__pool_alloc` before it, after `deallocate_from_fixed_pools` | Global `malloc` at `0x8013a6a8` (152 bytes) between `free` and `deallocate_from_fixed_pools`; Prime omits `malloc` because Prime never links it |

`ctype.c` comes from Prime because Frontline's `tolower` is a weak definition
(`_CTYPE_INLINE` is `__declspec(weak)` there); the Wind Waker defines it as a
plain function.

## Linker notes

Global labels inside kept code (for example the `_savegpr_14` entries of
`runtime.c`'s file-local `__save_gpr`) are passed to SN as `--undefined` roots
alongside global functions; otherwise SN drops code that other files reach
only through those labels.

SN's linker emits weak definitions as global. `validate_object` checks each
kept function's binding (weak, global or local) in the compiled object; the
linked comparison then treats weak and global alike.

## Reconstructions without a matching reference (`src/msl/`)

- `signal.c`: `raise` with a six-entry handler table; signal 1 (`SIGABRT`)
  with the default handler returns 0 instead of exiting.
- `uart_console_io.c`: the Wind Waker source with `__init_uart_console`
  inlined, `__write_console` weak and no `__close_console`.
- `bsearch.c`: checks its arguments, compares element 0, then binary-searches
  elements 1 to `num - 1`.
- `mslsupp.c`: MetroTRK's MSL file hooks (`__open_file`, `__read_file`,
  `__write_file`, `__position_file`, `__close_file`, `__close_console`,
  `__TRK_write_console`, `__read_console`) and the local `convertFileMode`.
  The console hooks check `GetUseSerialIO` and repeat the file access on
  stdout/stdin instead of calling `__write_file`/`__read_file`. The original object has debug sections and is
  built with inlining off (the TRK profile otherwise); `convertFileMode`
  takes the mode by pointer and returns a full-width value.
- `w_pow.c`: fdlibm's `_IEEE_LIBM` `pow` wrapper.
- `NewMore.cp`: `std::exception`'s destructor and `what()`, both weak (as
  header-inline members emitted with the vtable); built with RTTI on.

Functions added to Wind Waker copies (absent upstream, reconstructed from the
disassembly): `clearerr` in `misc_io.c` (placed before `__stdio_atexit`),
`__find_unopened_file`, `__init_file` and `__flush_line_buffered_output_files` in
`ansi_files.c`, `fopen` and `freopen` in `file_io.c`, `strstr` in `string.c`, `atoi` in `strtoul.c` (`strtol(str, NULL, 10)`,
with `strtol` inlined), and `__load_buffer` and `setvbuf` in `buffer_io.c`
(source order reversed relative to the target).

## Open work

- No reference links these, so they need reconstruction from the
  disassembly: the C++ throw/unwind code of `Gecko_ExceptionPPC.cp`
  (`__throw`, `ExPPC_*`, about 5 KB), `__get_file_modes` (a draft differs in
  one register choice) and `__fread`/`fread`.
- MetroTRK: Frontline links the Wind Waker's revision (117 of 118 function
  sizes agree), but the Wind Waker decompilation marks most TRK files
  non-matching. The matching files (`mainloop`, `nubevent`, `usr_put`,
  `mutex_TRK`, `flush_cache`, `mpc_7xx_603e`, `targcont`, `main_TRK`,
  `target_options`, `dolphin_trk`, `dispatch`, `notify`) are verified. The rest needs decompilation work: for
  example `TRKMessageSend` is a 40-byte stub in the reference and 476 bytes in
  the target (`msg.c`, `support.c`, `nubinit.c`,
  `msgbuf.c`, `serpoll.c` (`TRKTestForPacket`), `msghndlr.c`, `targimpl.c`,
  `dolphin_trk_glue.c`, `mem_TRK.c`), plus the assembly `targsupp.s`.
- `msgbuf.c` is an inlining puzzle: the target's readers inline `TRKReadBuffer`
  while its appenders call `TRKAppendBuffer` out of line (`TRKAppendBuffer1_ui16`
  is 84 bytes). With `-inline deferred,auto` both are inlined; with plain
  `-inline auto` and source in address order, neither is. Compiler releases
  GC/1.1 to 2.0 do not change this, and Sunshine's settings (no auto-inlining)
  break the readers. The Wind Waker's own build has the same mismatch.
