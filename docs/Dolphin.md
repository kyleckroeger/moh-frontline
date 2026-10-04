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
- **Weak definitions.** Frontline has weak globals (for example `PPCHalt`,
  `__start`). External resolution treats them like globals.
- **Declaration-only symbols.** `__declspec(section ".init")` declarations in
  `__os.h` make MW emit unreferenced undefined symbols (`__init_data`, …). In
  Frontline some exist only as `static` functions of `__start.c`. A name in
  `undefined_in_discarded_code` may exist in the target only as another source
  file's private symbol.

## Open problems

- **Partial data stripping.** When MW removed some but not all objects of a data
  section (e.g. `__ai_src_time_start`/`_end` at the end of `ai.c`'s `.sbss`), the
  whole compiled section no longer fits. The `ai_src` fragment is accepted
  instead of the whole of `ai.c`.
- **Small-data ordering.** `dsp.c` and `dsp_task.c` place `.sbss` objects in an
  order different from the compiled object (inconsistent anchors).
- **Revision differences.** About 40 SDK files differ from both reconstructions
  in one or a few functions (`OSAlarm`, `GXAttr`, `CARD*`, `vi`, `Pad`, …). These
  need per-function source changes between the 2001 and 2004 code. Rising Sun's
  SDK units and dolsdk2001 together give two reference points.
- `GXBump`, `GXDisplayList`, `GXGeometry`, `GXTransform`: equal sizes but
  different bytes; inspect with `unit_diff.py`.
- `PPCArch`, `odenotstub`: linked function symbols differ (likely asm-only or
  alias functions).
