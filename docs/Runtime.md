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
| `src/tww/` | [zeldaret/tww](https://github.com/zeldaret/tww) `a1854d4` | MSL C, math, runtime, `DebuggerDriver.c`, `odenotstub.c` |
| `src/prime/runtime/` | [PrimeDecomp/prime](https://github.com/PrimeDecomp/prime) `32020a0` | `alloc.c`, `ansi_fp.c`, `ctype.c`, `rand.c` |

Units use the reference's own compiler profile: the Wind Waker's
`cflags_runtime` with GC/1.3 for MSL and the runtime, GC/1.2.5n for
`DebuggerDriver.c`, and GC/1.3.2 with `cflags_dolphin` for `odenotstub.c`;
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
| `prime/runtime/alloc.c` | Add `malloc` (a `__pool_alloc` wrapper, MSL's form) and move `__pool_alloc` before it, after `deallocate_from_fixed_pools` | Global `malloc` at `0x8013a6a8` (152 bytes) between `free` and `deallocate_from_fixed_pools`; Prime omits `malloc` because Prime never links it |

`ctype.c` comes from Prime because Frontline's `tolower` is a weak definition
(`_CTYPE_INLINE` is `__declspec(weak)` there); the Wind Waker defines it as a
plain function.

## Linker notes

SN's linker emits weak definitions as global. `validate_object` checks each
kept function's binding (weak, global or local) in the compiled object; the
linked comparison then treats weak and global alike.

## Open work

- `runtime.c`, `buffer_io.c`, `global_destructor_chain.c`, `NMWException.cp`,
  `Gecko_ExceptionPPC.cp` (partly covered), `uart_console_io.c`,
  `mslsupp.c`'s file functions, `ansi_files.c`'s file-table helpers,
  `fopen`/`fread`/`setvbuf`, `qsort`, `strstr`, `atoi`, `raise` and `pow`/`cosf`.
- MetroTRK differs from the Wind Waker's revision in most functions
  (`msghndlr.c`, `targimpl.c`, `serpoll.c`, `msgbuf.c`, `nubinit.c`,
  `dolphin_trk_glue.c`); `dolphin_trk.c` and `mem_TRK.c` also need `.init`
  code units, like `__start.c`.
