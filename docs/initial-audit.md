# Initial audit

Target: **Medal of Honor: Frontline**, GameCube, USA, disc ID `GMFE69`,
disc 0, revision 0. Findings below come from the pinned executables only.

## Disc executables

| File | Size | Role |
| --- | --- | --- |
| `boot.dol` / `Moh2BootRel.dol` | 168,544 | Boot program (identical bytes); loads the game ELF |
| `Moh2RelGC.elf` | 2,866,920 | Game executable; the decompilation target |
| `Moh2StubRelGC.elf` | 278,096 | Loader stub at `0x80680000` (`loadelf.cpp`, `loader.cpp`, Dolphin SDK) |

This mirrors Rising Sun's `boot.dol` → `MOH3RDVD.ELF` / `STUBRDVD.ELF` layout.

## `Moh2RelGC.elf`

- Entry `__start` at `0x80007140`; `_SDA_BASE_` `0x80355960`; `_SDA2_BASE_` `0x80357520`.
- Allocated sections: `.init`, `extab`, `extabindex`, `.text`, `.ctors`,
  `.dtors`, `.rodata`, `.data`, `.bss`, `.sdata`, `.sbss`, `.sdata2`, `.sbss2`.
- Executable bytes (progress denominator): `.init` 9,452 + `.text` 1,405,120
  = **1,414,572**.
- 18,811 symbols: 5,005 functions, 10,560 objects, 557 file records.
- `.rela.*` sections are preserved for code and data. `.debug` and `.line` are
  empty: there is no DWARF type information.

### Compiler

Metrowerks CodeWarrior, from: `extab`/`extabindex` exception tables,
`.mwcats.text`, `__sinit_<file>_cpp` static initializers, CodeWarrior C++ name
mangling, and the MW linker-generated `_rom_copy_info`, `_bss_init_info` and
`_eti_init_info` tables. The exact compiler version and flags are not yet
established. Rising Sun is a useful reference: its Dolphin SDK units match
with CodeWarrior `GC/1.2.5n`, but its game code is ProDG, which does not apply here.

### Rough composition

Heuristic grouping by file name, using dtk's preliminary `.text` ranges (see
the caveat below). Use it for orientation only.

| Group | Files | Share of `.text` |
| --- | ---: | ---: |
| C++ (`.cpp`): game code and EA C++ libraries | 240 | ~72% |
| Dolphin SDK | 90 | ~11% |
| EA audio (`s*.c`, `SNDI_*`) | 129 | ~8% |
| Other C (UI studio `UIS*.c`, `mainloop.c`, `DSPCode.c`, …) | 15 | ~4% |
| MSL / MW runtime | 36 | ~3% |
| MetroTRK debugger | 22 | ~2% |
| MSL math | 25 | ~1% |

`gc_idct.asm` is a hand-written assembly file record.

## Baseline relink

`tools/baseline.py` reproduces the complete ELF-derived DOL byte for byte:

1. `dtk elf2dol` produces the reference DOL (SHA-1 `0e96c0be…`, 1,641,760 bytes).
2. `dtk elf config` exports the original symbols. `_ctors`/`_dtors` are dropped
   because dtk recreates them for DOL inputs and rejects duplicates.
3. `dtk dol split` produces **one** object covering every allocated section.
4. GNU `ld` relinks it at the original section addresses. Symbols that dtk leaves
   undefined (MW linker symbols such as `_stack_addr`, `__ArenaLo`) are pinned to
   their unique original definitions.
5. dtk omits the bytes of MW linker tables. These are included from the original:
   `_rom_copy_info` + `_bss_init_info` at the end of `.init` (164 bytes),
   `_eti_init_info` at the end of `extabindex` (32 bytes), and the null
   terminators of `.ctors` and `.dtors` (4 bytes each). The script fails if
   anything other than these pinned tables is missing.

The relink proves the toolchain and layout. It earns no source credit.

## Per-file splits: not yet trustworthy

`dtk elf config` emits ranges for all 557 files in every section. They are not
yet usable for a per-file split:

- Ranges for globals with no file-local anchor are attributed to the **last** file
  record, `sdfx.c`. This includes `__start` in `.init` and the first `.bss`,
  `.rodata`, `.sdata` and `.sbss` ranges, which sit next to `Moh2.cpp`. The
  resulting link order is cyclic.
- `__init_cpp_exceptions.cpp` comes first in `.ctors`/`.dtors`. This is normal MW
  linker behavior that a link-order resolver must allow.
- Some ranges are empty, run into the alignment padding before the next
  section, or wrap to `0x0` at the end of `.sbss2`.

The preserved relocations should make correct ownership recoverable. That is
the first research task, and it also feeds the decomp.dev code map.
