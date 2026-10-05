# Reuse index and next work

How to choose the next reconstruction target for `Moh2RelGC.elf` (GMFE69). This
is a **research lead index**, not accepted source: a symbol or file-name match
earns no credit, and only the complete-image comparison in
`tools/reconstruct.py` accepts a unit. Sizes below come from the current
`progress/GMFE69.snapshot.json` and will move as units are accepted.

Read `docs/Roadmap.md` for the plan and `docs/Licensing.md` for reference terms.

## A compiler caveat that shapes every reference

Frontline is **Metrowerks CodeWarrior**, not SN/ProDG. That splits the useful
references in two:

- **Byte-match candidates** — references built with CodeWarrior/MWCC
  (dolsdk2001/2004, other GC MWCC games). These can sometimes drop in directly.
- **Semantic references only** — the sibling *Rising Sun* game code and its
  libraries are **ProDG/SN**. Their source describes behaviour, but its bytes
  will not match Frontline and its Newlib/libgcc references do not apply here.
  Frontline's C runtime is MSL and its C++ runtime is the MW PPC EABI
  (`__register_global_object`, `__construct_array`), not GCC libgcc.

A name match is a lead; only generated code and data checked byte-for-byte
against GMFE69 accepts source.

## Status (from the verified snapshot)

| Category | Unfinished bytes |
| --- | ---: |
| Accepted (`restored_library`) | 51,416 |
| Symbol evidence, incomplete | 934,812 |
| Inferred file groups | 305,428 |
| Unknown file | 113,748 |
| Unidentified / padding | 9,016 |
| Local symbols only | 152 |
| **Total executable** | **1,414,572** |

## Reference catalog

| Reference | Covers | Built with | Confidence | Notes |
| --- | --- | --- | --- | --- |
| [dolsdk2001](https://github.com/doldecomp/dolsdk2001) | Dolphin SDK | MWCC GC | High | In use; Dec 2001 sits between this and 2004 |
| [dolsdk2004](https://github.com/doldecomp/dolsdk2004) | Dolphin SDK | MWCC GC | High | In use for the units that keep 2004 behaviour |
| Other MWCC GC decomps (e.g. [Prime](https://github.com/PrimeDecomp/prime), [TWW](https://github.com/zeldaret/tww), [mkdd](https://github.com/doldecomp/mkdd), [melee](https://github.com/doldecomp/melee)) | MSL and CW runtime functions | MWCC GC | High | Best lead for `__dl__FPv`, `__nw__FUl`, `__register_global_object`, `__construct_array`, `memset`, `strlen`, `sprintf` |
| [NFS Most Wanted reconstruction](https://github.com/dbalatoni13/nfsmw) | EAGL animation/loading/rendering, EA engine | MWCC GC | Medium-High | Same EA engine family; check layouts against GMFE69 before trusting |
| [Rising Sun](https://github.com/lifewillbeokay/moh-rising-sun) (GR8E69) | EA engine, game C++, Lua/EAGL | ProDG (game) / MWCC (SDK) | Medium | Semantic source; game bytes will not match. Its [ReuseMap](https://github.com/lifewillbeokay/moh-rising-sun/blob/main/docs/ReuseMap.md) shows the method |
| MOH Frontline PS2 (SLUS) | Same game, formats and names | different arch/compiler | Medium | Naming/format leads only; check independently against GMFE69 |
| Lua 4.0.1 / STLport 4.5.3 | Script/containers, *if used* | source | Unknown | Applicability to Frontline unconfirmed |
| [MetroTRK](https://github.com/encounter) | Debugger stubs | MWCC | Low-Medium | Confirm the target really links these units first |

Preserve attribution and license terms for anything adapted; record provenance
in the unit manifest.

## Backlog 1 — remaining Dolphin SDK (Phase A, reference-backed)

Unfinished SDK files total **69,408 bytes**. The four highest are `dvd.c`
(9,140), `vi.c` (6,648), `Pad.c` (4,960) and `CARDBios.c` (4,672); the rest
range from `GXInit.c` (4,332) down to `dsp.c` (264). Reference: dolsdk2001 /
dolsdk2004, with per-unit revision notes in `docs/Dolphin.md`. This is the
cheapest work and it settles file boundaries.

Known blockers (`docs/Dolphin.md` open problems): `dsp.c` / `dsp_task.c`
small-data ordering, `GXPerf` retained jump tables, and `PPCArch` /
`odenotstub` linked-symbol differences.

## Backlog 2 — high fan-in helpers (do these first)

Ranked by unique unfinished callers (`tools/dependencies.py`). Small helpers
with many callers unlock the most downstream code.

| Callers | Bytes | Symbol | Candidate reference |
| ---: | ---: | --- | --- |
| 190 | 68 | `__dl__FPv` | MW runtime (other MWCC decomps) |
| 129 | 60 | `CMatrix::InitClass` | EA engine / cross-binary |
| 106 | 20 | `OSDisableInterrupts` | dolsdk2001/2004 |
| 102 | 80 | `DebugMsg(char const*, ...)` | EA engine / cross-binary |
| 102 | 36 | `OSRestoreInterrupts` | dolsdk2001/2004 |
| 95 | 336 | `MEM_free` | EA memory / cross-binary |
| 83 | 48 | `memset` | MSL |
| 79 | 68 | `CMatrix::operator=` | EA engine / cross-binary |
| 69 | 80 | `memcpy` | MSL |
| 67 | 304 | `BSObject::TriggerEvent` | EA engine / cross-binary |
| 65 | 16 | `CScene::GetPlayer` | EA engine / cross-binary |
| 65 | 28 | `CMatrix::SetPos` | EA engine / cross-binary |
| 64 | 52 | `SNDSYS_leavecritical` | EA audio |
| 64 | 52 | `SNDSYS_entercritical` | EA audio |
| 55 | 32 | `CMatrix::Multiply` | EA engine / cross-binary |
| 49 | 224 | `sprintf` | MSL |
| 45 | 248 | `__construct_array` | MW runtime |
| 41 | 236 | `CQuaternion::GetMatrix` | EA engine / cross-binary |
| 41 | 8 | `OSGetTick` | dolsdk2001/2004 (accepted) |
| 36 | 24 | `__register_global_object` | MW runtime |
| 35 | 184 | `strcpy` | MSL |
| 34 | 120 | `__nw__FUl` | MW runtime |
| 33 | 28 | `strlen` | MSL |
| 33 | 56 | `CMatrix::Ident` | EA engine / cross-binary |
| 33 | 116 | `DWI_alloc` | EA working memory / cross-binary |

The MSL and MW-runtime rows are Phase A/B with a likely byte-match reference.
The `CMatrix`, `BSObject`, `CScene`, `MEM_*` and audio rows are game/engine
code: Phase B if a reference exists, Phase C otherwise.

## Backlog 3 — game code: reuse or Ghidra

Largest unfinished file groups (Phase B if reference-backed, else Phase C):

| Bytes | File | Likely family |
| ---: | --- | --- |
| 93,236 | `bsbifunc.cpp` | Behaviour script built-ins |
| 45,684 | `player.cpp` | Player |
| 42,248 | `ShellMenu.cpp` | UI |
| 30,916 | `AIObject.cpp` | AI |
| 21,644 | `path.c` | Pathfinding |
| 21,516 | `cdbgeom.cpp` | Collision/geometry |
| 21,052 | `box.cpp` | Math/bounds |
| 20,880 | `scene.cpp` | Scene graph |
| 19,708 | `objcreate.cpp` | Object creation |
| 19,672 | `Soldier_object.cpp` | Actor |
| 19,496 | `UserInterface.cpp` | UI |
| 17,116 | `sound.cpp` | Audio |
| 16,872 | `Moh2.cpp` | Core |
| 16,568 | `particlesystem.cpp` | Particles |
| 16,304 | `propdat.cpp` | Properties |
| 14,920 | `AIFilter.cpp` | AI |
| 12,156 | `snddrv.c` | Audio driver |

Anything that shares an EA engine family with Rising Sun or NFS MW (matrix,
scene graph, EAGL, particles, properties, observers, containers) should be
tried against those references first. Mission/game-specific logic (AI,
`ShellMenu`, `bsbifunc`, player) has no public source and is the Ghidra/phased
work. `Unknown file` (113,748 bytes) and `unidentified` (9,016 bytes) must be
resolved with `tools/file_map.py` before assigning a source boundary.

## Cross-binary matching (Phase C)

Auto-transfer names and structures between binaries with Ghidra Version
Tracking or Diaphora, comparing GMFE69 against:

1. **NFS Most Wanted GC** and other MWCC EA GC titles — closest byte-level
   engine references.
2. **MOH Rising Sun GC** — same engine lineage, different compiler.
3. **MOH Frontline PS2** — same game, different architecture.

A moved name is a hypothesis; confirm the function's behaviour, size and bytes
on GMFE69 before writing source. Keep decompiler output under `scratch/`.

## Maintenance

- Regenerate the numbers with `tools/code_map.py` / `tools/dependencies.py`
  and the snapshot (`tools/progress_report.py --capture`).
- When a reference yields accepted bytes, record it here with the exact scope
  and the evidence, and update the unit manifest's `upstream` provenance.
- References that do not pan out should be marked as tried, not deleted.
