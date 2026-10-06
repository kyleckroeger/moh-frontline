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
| `src/tww/` | [zeldaret/tww](https://github.com/zeldaret/tww) `a1854d4` | MSL C, math, runtime, MetroTRK, `__ppc_eabi_init.cpp`, `DebuggerDriver.c`, `odenotstub.c` |
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
| `tww/.../MSL_Common/Src/abort_exit.c` | `__aborting` and `__console_exit` are global, not `static` | Global `.sbss` objects at `0x8034f318` and `0x8034f324`; `__atexit_curr_func` stays file-local as in the target |
| `tww/.../Runtime/Src/runtime.c` | Add `__cvt_sll_flt` (`asm`, copied from Prime's `runtime.c`) between `__shr2i` and `__cvt_dbl_usll` | Global `__cvt_sll_flt` at `0x80138dec` (180 bytes) between them; Prime's whole file does not fit, because its `__save_fpr`-family helpers are global where the target's are file-local |
| `tww/.../Runtime/Src/NMWException.cp` | Declare `terminate`, `unexpected` and their setters in namespace `std` (the file declared them at global scope), and give `__throw_catch_compare` C linkage | `duhandler__3stdFv` at `0x80138878` is 40 bytes and calls through `thandler` (`lwz r12,-29704(r13)`), so it inlines `std::terminate`; the global-scope declaration made it call an external `::terminate`. Global `__throw_catch_compare` at `0x801385fc` is unmangled |
| `tww/dolphin/os/__ppc_eabi_init.cpp` | Add the `.init` functions `__init_hardware` and `__flush_cache` (`asm`, from dolsdk2001's `__ppc_eabi_init.c`, with C declarations of `__OSPSInit`/`__OSCacheInit`), and make `__init_cpp` `static` | `__init_hardware` (`0x80007330`) and `__flush_cache` (`0x80007350`) belong to the `__ppc_eabi_init.cpp` record with `__init_user`, file-local `__init_cpp` and `_ExitProcess`; the 2001 `.c` file also defines `abort`/`exit`, which Frontline takes from MSL |
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

## Open work

- No reference links these, so they need reconstruction from the
  disassembly: the C++ throw/unwind code of `Gecko_ExceptionPPC.cp`
  (`__throw`, `ExPPC_*`, about 5 KB), `mslsupp.c`'s file hooks (`__open_file`
  and the rest; Pikmin 2 has only placeholder stubs), `buffer_io.c`'s
  `__load_buffer` and `setvbuf`, `ansi_files.c`'s file-table helpers,
  `fopen`/`freopen`/`fread`, `bsearch`, `strstr`, `atoi`, `raise`, `abort`,
  `clearerr`, `NewMore.cp`, `uart_console_io.c`'s `__write_console`, and the
  `pow`/`cosf` wrappers.
- MetroTRK: Frontline links the Wind Waker's revision (117 of 118 function
  sizes agree), but the Wind Waker decompilation marks most TRK files
  non-matching. The matching files (`mainloop`, `nubevent`, `usr_put`,
  `mutex_TRK`, `flush_cache`, `mpc_7xx_603e`, `targcont`, `main_TRK`,
  `target_options`) are verified. The rest needs decompilation work: for
  example `TRKMessageSend` is a 40-byte stub in the reference and 476 bytes in
  the target (`msg.c`, `dispatch.c`, `notify.c`, `support.c`, `nubinit.c`,
  `msgbuf.c`, `serpoll.c`, `msghndlr.c`, `targimpl.c`, `dolphin_trk.c`,
  `dolphin_trk_glue.c`, `mem_TRK.c`), plus the assembly `targsupp.s`.
- `msgbuf.c` is an inlining puzzle: the target's readers inline `TRKReadBuffer`
  while its appenders call `TRKAppendBuffer` out of line (`TRKAppendBuffer1_ui16`
  is 84 bytes). With `-inline deferred,auto` both are inlined; with plain
  `-inline auto` and source in address order, neither is. Compiler releases
  GC/1.1 to 2.0 do not change this, and Sunshine's settings (no auto-inlining)
  break the readers. The Wind Waker's own build has the same mismatch.
