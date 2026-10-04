# Progress reports and decomp.dev

The project exports the [objdiff report v2 schema](https://github.com/encounter/objdiff/blob/0c48d711c7bd51f791b353d7d85ba948b277e2f2/objdiff-core/protos/report.proto)
using its own byte-verification tooling, adapted from
[moh-rising-sun](https://github.com/lifewillbeokay/moh-rising-sun). GitHub Actions
uploads `report.json` in an artifact named `GMFE69_report`, as required by the
[decomp.dev integration guide](https://decomp.wiki/tools/decomp-dev).
There is no decomp.dev-specific repository metadata file.

## Verification boundary

The public workflow validates a **locally verified snapshot**. It does not have
the original executables and does not rerun the proprietary compiler toolchain.
`progress/GMFE69.snapshot.json` records hashes of every source file, header,
configuration file and build script, plus verified unit counts and object hashes.
It also includes an executable-symbol map for unfinished code, generated from the
pinned original during capture.
Changes, additions and removals in those directories make CI fail until someone
with the pinned original completes verification and refreshes the snapshot.
Documentation-only changes do not require rebuilding.

To refresh after an accepted source/tooling change, on a supported local machine:

```sh
python3 tools/reconstruct.py
python3 -m unittest discover -s tests -v
python3 tools/progress_report.py --capture
```

Capture rechecks the complete rebuilt image against its pinned identity, original
load layout, source ranges and compiled unit hashes/bytes. It emits only selected
public metadata, never original bytes or local filesystem paths. Commit the
snapshot with the source change. Maintainers must review snapshot updates;
fingerprints detect stale inputs, not fabricated assertions or a malicious verifier.

CI runs the tests, checks the complete input inventory and generates the objdiff
report. Anyone can reproduce that public export without a game image:

```sh
python3 tools/progress_report.py
```

## Counting rules

The overall denominator is every allocated executable byte in the pinned
`Moh2RelGC.elf`: 1,414,572 bytes (`.init` 9,452 + `.text` 1,405,120). Only
nonoverlapping, verified source-built function bytes earn credit. Restored library
code and reconstructed game code are distinct categories. Accepted units may be
fragments of an original translation unit.

Unreconstructed code is represented by named file groups, explicitly unassigned
function groups, shared entry-point groups, and unidentified code/padding ranges,
all with zero matching credit. Their ranges plus accepted source must partition
the complete executable with no gaps or overlaps. The code map infers file groups
only from file-local symbols for now. Rising Sun's adjacent-marker inference uses
GCC `gcc2_compiled.` markers, which CodeWarrior does not emit, so many functions
appear under "Unknown file" until relocation-based ownership is implemented (see
the [initial audit](initial-audit.md)). Data, BSS, linker tables and original
context earn no code credit. No fuzzy partial-match credit is used. Runtime is untested.

## Registering the project

Once a successful default-branch workflow has uploaded its report, a repository
admin can add the repository at <https://decomp.dev/manage/new>, naming the game
**Medal of Honor: Frontline** and platform **GameCube**. Repository creation and
report publication do not themselves register a project on decomp.dev. Its GitHub
app is optional for faster updates and PR comments; the site also polls reports.
