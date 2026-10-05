# Dolphin SDK units

Frontline links a Dolphin SDK build dated **Dec 17 2001** (the only SDK date
string in `Moh2RelGC.elf`). That is between the two public reconstructions this
project uses:

| Source tree | Upstream | SDK | Notes |
| --- | --- | --- | --- |
| `src/dolphin/`, `include/dolphin-sdk/` | Rising Sun's adaptation of [dolsdk2004](https://github.com/doldecomp/dolsdk2004) | 2004 era | Built with `-DSDK_REVISION=1`; `SDK_REVISION=0` did not fix any Frontline difference |
| `src/dolsdk2001/`, `include/dolsdk2001/` | [dolsdk2001](https://github.com/doldecomp/dolsdk2001) | May 22 2001 | Closer to Frontline; unit manifests set `source_root` |

Every accepted SDK unit is built with CodeWarrior **GC 1.2.5n**, `-O4,p
-inline auto`, matching the profile Rising Sun found for its SDK. GC 1.2.5
produces identical sizes but different epilogue scheduling (`mtlr` order), so
size comparisons alone cannot pick the compiler. `AX` and `GXStubs` also
verify with 1.2.5 and are recorded as built. A working profile is not proof of
the original compiler release.

## Local modifications to dolsdk2001

Frontline's Dec 2001 SDK already contains some changes found in later SDKs.
Each change below was required by, and verified against, the original bytes.
Upstream is commit `eb1234c` of dolsdk2001.

| File | Change | Evidence |
| --- | --- | --- |
| `gx/__gx.h` | Struct head `vNumNot`, `bpSentNot`, 16-bit `vNum`/`vLim`; `tevTcEnab` added after `tcsManEnab` (later fields +4) | `dirtyState` at `0x4F4`; `lhz 4`/`lhz 6` loads in `__GXSendFlushPrim` |
| `gx/*.c` (11 files) | `gx->bpSent = x` → `gx->bpSentNot = !x` | Inverted constants stored at `gx+2` |
| `gx/GXGeometry.c`, `gx/GXDisplayList.c` | Flush test on `vNumNot == 0` | Inverted branch in `GXBegin`/`GXCallDisplayList` |
| `gx/GXMisc.c` | `GXSetMisc` maintains `vNumNot`; `GXFlush` writes 8 words | 2004 code, sizes and bytes |
| `gx/GXTransform.c` | Scissor and viewport offset 342 (was 340) | Immediates and `.sdata2` constant `342.0f` |
| `gx/GXPixel.c` | `GXSetFogRangeAdj` centre offset 342 (was 340) | Immediate `center + 342` |
| `dvd/dvdfs.c` | Dec 2001 `OSPanic` line numbers 376, 739, 745 | `OSPanic` line-number immediates |
| `gx/GXFifo.c` | `CPUFifo`/`GPFifo` private and declared first; `__GXFifoInit` clears them | Local symbols and `.sbss` order |
| `ax/AXAlloc.c` | `__AXPushFreeStack` clears `priority` | 2004 code |
| `ax/AXAux.c` | Aux input getters test the callback; the B getter tests `__AXCallbackAuxA` (an SDK bug fixed later) | Relocation to `__AXCallbackAuxA` at `0x8011fd42` |
| `os/OSAlarm.c`, `os/__os.h` | System-time calls throughout; 2004 `OSSetPeriodicAlarm` and `DecrementerExceptionCallback` | Relocations to `__OSGetSystemTime` |
| `card/CARDUnlock.c` | `DoneCallback` returns after each error callback | 2004 code |
| `include/dolphin/card.h` | `CARDControl` gains `cid` and `diskID` (size `0x110`) | `mulli rX, chan, 272` |
| `gx/GXTev.c` | `GXSetTevOrder` updates `tevTcEnab` on the texcoord test (2004 code); replaces the plain ternary | Read-modify-write of `gx+0x4E0` (`lwz 1248`/`andc`/`or`/`stw 1248`) at `0x8012f600`-`0x8012f63c`; function is 476 bytes, not the 416-byte 2001 ternary |

## Local modifications to the 2004 tree

Five further units are verified from `src/dolphin/` (the 2004 adaptation).
`mtx44.c` needs no change; the `card` units keep behaviour the 2004 source
later changed, and `pad/Padclamp.c` keeps the Dec 2001 `PADClampRegion`, so they
are edited against the original bytes.

| File | Change | Evidence |
| --- | --- | --- |
| `card/CARDCheck.c` | `VerifyID` tests `encode` before the serial checks, using `OSGetFontEncode` | Check order in the original; relocation to `OSGetFontEncode` |
| `card/CARDFormat.c` | Format progress stays at the mount-step offset (`formatStep` = `mountStep`); `OSGetFontEncode` | Relocation to `OSGetFontEncode`; shared progress field |
| `card/CARDWrite.c` | `__CARDAccess` instead of `__CARDIsWritable` | Relocation to `__CARDAccess` |
| `pad/Padclamp.c`, `include/dolphin-sdk/dolphin/pad.h` | `PADClampRegion` drops the unused `radStick`/`radSubstick` (8 bytes); `ClampRegion` is non-const (`.sdata`); `ClampTrigger` takes only the trigger and reads `ClampRegion` itself | Region at `.sdata:0x8034e4c0` size 8; original PADClamp loads `-29856(r13)` and `1(r29)`, and reloads `minTrigger` after the store. The `ClampTrigger` signature is inferred from that inlined code; the function itself is dead-stripped |

## Porting workflow

```sh
python3 tools/port_unit.py <id> <source> --like <manifest-or-template> [--original-file X.c] [--compiler 1.2.5n]
# add "<id>.json" to config/GMFE69/project.json, then:
python3 tools/reconstruct.py
python3 tools/unit_diff.py <id>        # if verification reports differing bytes
```

`port_unit.py` compiles the source and drafts a manifest by pairing each
compiled relocation with the original relocation at the same offset. This is
possible because Frontline preserves `.rela.*`. It fixes every section address
and external without guesswork. The draft earns nothing until `reconstruct.py`
verifies the complete image. `--like` copies compiler settings and provenance
from an existing manifest (a Rising Sun manifest, or a template with
`source_root` and `upstream` for dolsdk2001).

## CodeWarrior linker behavior

The original was linked by the MW linker, unlike Rising Sun's ProDG link:

- **Dead stripping of functions.** Functions absent from the original are compiled
  and discarded (`strip_unused`, `discarded_functions`), as in Rising Sun.
- **Dead stripping of data.** MW also removes unreferenced data. A compiler
  section that only discarded code references is declared in
  `stripped_sections`. SN's linker has no `/DISCARD/`, so those sections are
  parked at `0x10000000 + n * 0x100000`, outside main memory, and never enter the
  rebuilt image. `verify_unit` checks they are parked; retained code that used
  them would fail the byte comparison.
- **Trimmed data.** When MW removed unused objects at the start or end of a data
  section, the manifest section names the kept slice (`linked_offset`,
  `linked_size`). The whole compiled section is linked at the shifted base, only
  the kept bytes are compared and credited, the trimmed part must hold whole
  compiler objects, and retained code may not refer into it. Stripping in the
  middle of a section is not supported.
- **Weak definitions.** Frontline has weak globals (for example `PPCHalt`,
  `__start`). External resolution treats them like globals.
- **Declaration-only symbols.** `__declspec(section ".init")` declarations in
  `__os.h` make MW emit unreferenced undefined symbols (`__init_data`, …). In
  Frontline some exist only as `static` functions of `__start.c`. A name in
  `undefined_in_discarded_code` may exist in the target only as another source
  file's private symbol.

## Open problems

- **Middle-of-section data stripping**, and small-data ordering in `dsp.c` and
  `dsp_task.c`: no single base places all kept objects.
- **Revision differences.** Remaining SDK files differ from both reconstructions
  in some functions. A `scratch`-style comparison of each function's size against
  the 2001 and 2004 builds shows where to start: `Pad` is explained entirely by
  2004 functions, which suggests the 2004 source as its base. `CARDCheck`,
  `CARDFormat` and `CARDWrite` were resolved that way and are now accepted.
- **Discarded functions with retained jump tables.** MW dead-strips a function
  while its switch tables sit in a data section that is otherwise kept. The SN
  linker then retains the function through the data relocation, so `GXPerf`
  (whose trimmed `.data` references `GXReadGPMetric`) cannot yet reproduce the
  original `.text` layout.
- Compiled code that calls a different function than the original is reported by
  `port_unit.py` (for example `OSGetTime` vs `__OSGetSystemTime`).
- `PPCArch`, `odenotstub`: linked function symbols differ (likely asm-only or
  alias functions).
