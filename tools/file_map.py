#!/usr/bin/env python3
"""Recover per-file section ranges of the original from symbols and relocations.

Evidence, strongest first:
1. File-local symbols (including MW's @N constants and section markers) belong
   to the file record they follow in the symbol table.
2. Contributions appear in file-record (link) order in every section; .ctors
   and .dtors are exempt (MW places __init_cpp_exceptions first).
   An object starting exactly at a file's private label (e.g. its section
   marker "...bss.0") belongs to that file.
3. A global whose bytes refer to another file's private symbol belongs to that
   file, if the link order allows it. A function belongs to the file whose
   (private) exception-index entry points to it.
4. A global between two items of the same file belongs to that file.
Only when those stall, a weaker rule is applied; it and everything placed
using its results are labeled as inference:
5. same-class: a C++ method goes with the other methods of its class.

Scored against verified units, rules 1-4 have been exact. A sole-caller rule
was tried and rejected: library functions are mostly called from other files.

Anything still ambiguous is reported, not guessed. Output is research data
(build/audit/file_map.json); it earns no source credit.
"""
import bisect
import json
import re
import struct
from collections import defaultdict
from pathlib import Path

from audit import load_target
from setup import ROOT

UNORDERED = (".ctors", ".dtors")
# The MW runtime's table entries precede link order; dtk projects rename them
# so the linker sorts them first (as in other CodeWarrior decompilations).
RENAMES = {("__init_cpp_exceptions.cpp", ".ctors"): ".ctors$10",
           ("__init_cpp_exceptions.cpp", ".dtors"): ".dtors$10",
           ("global_destructor_chain.c", ".dtors"): ".dtors$15"}
# CodeWarrior mangling: name__<len><Class>... or name__Q<n><len><Ns>...<len><Class>...
METHOD = re.compile(r"^.+?__(Q(\d))?(\d+)")


def class_of(name):
    """Innermost class of a mangled member, or None (templates are skipped)."""
    if name.startswith("__") and not name.startswith(("__ct__", "__dt__", "__as__")):
        return None
    match = METHOD.match(name)
    if not match or "<" in name:
        return None
    position, parts = match.start(3), int(match[2] or 1)
    for _ in range(parts):
        digits = re.match(r"\d+", name[position:])
        if not digits:
            return None
        length = int(digits[0])
        component = name[position + len(digits[0]):position + len(digits[0]) + length]
        position += len(digits[0]) + length
    return component if len(component) == length else None


def relocations_by_address(elf):
    """Original relocations as sorted (address, target symbol, addend)."""
    symbols = list(elf.symbols())
    result = []
    for section in elf.sections:
        if section["type"] != 4 or not elf.sections[section["info"]]["flags"] & 2:
            continue
        data = elf.contents(section)
        for offset in range(0, len(data), 12):
            where, info, addend = struct.unpack_from(">IIi", data, offset)
            result.append((where, symbols[info >> 8], addend))
    result.sort(key=lambda r: r[0])
    return result


def build_file_map(elf):
    symbols = list(elf.symbols())
    sections = {s["index"]: s for s in elf.sections if s["flags"] & 2 and s["size"]}
    files, owner_of_local = [], {}
    current = None
    for index, symbol in enumerate(symbols):
        if symbol["type"] == 4:
            current = len(files)
            files.append(symbol["name"].replace("\\", "/").rsplit("/", 1)[-1])
        elif symbol["binding"] == 0 and symbol["section"] in sections and current is not None:
            owner_of_local[index] = current
    # Items: every symbol with an address inside an allocated section.
    items = defaultdict(list)   # section index -> [dict]
    for index, symbol in enumerate(symbols):
        if symbol["section"] not in sections or symbol["type"] == 4 or not symbol["name"]:
            continue
        section = sections[symbol["section"]]
        if not section["address"] <= symbol["address"] < section["address"] + max(section["size"], 1):
            continue
        items[symbol["section"]].append(dict(index=index, name=symbol["name"], address=symbol["address"],
                                             size=symbol["size"], local=symbol["binding"] == 0,
                                             owner=owner_of_local.get(index), evidence="local-symbol"
                                             if index in owner_of_local else None))
    # Private-symbol references: address -> file that owns the referenced local.
    relocs = relocations_by_address(elf)
    addresses = [r[0] for r in relocs]
    private = {}
    for index, owner in owner_of_local.items():
        private[(symbols[index]["name"], symbols[index]["address"])] = owner

    def referenced_owners(start, end):
        owners = set()
        for where, target, _ in relocs[bisect.bisect_left(addresses, start):bisect.bisect_left(addresses, end)]:
            owner = private.get((target["name"], target["address"])) if target["binding"] == 0 else None
            if owner is not None:
                owners.add(owner)
        return owners

    def assign(weak):
        changed = False
        classes = defaultdict(set)
        if weak:
            for entries in items.values():
                for entry in entries:
                    if entry["owner"] is not None and class_of(entry["name"]):
                        classes[class_of(entry["name"])].add(entry["owner"])
        for section_index, entries in items.items():
            name = sections[section_index]["name"]
            ordered = name not in UNORDERED
            anchored = sorted((e["address"], e["owner"]) for e in entries
                              if e["owner"] is not None and (weak or "inferred" not in e["evidence"]))
            keys = [a for a, _ in anchored]
            for entry in entries:
                if entry["owner"] is not None:
                    continue
                cut = bisect.bisect_right(keys, entry["address"])
                before = max((o for _, o in anchored[:cut]), default=None)
                after = min((o for _, o in anchored[cut:]), default=None)
                low = before if before is not None and ordered else 0
                high = after if after is not None and ordered else len(files) - 1
                if ordered and before is not None and before == after:
                    entry.update(owner=low, evidence="between-same-file" + (" (inferred)" if weak else ""))
                    changed = True
                    continue
                owners = {o for o in referenced_owners(entry["address"], entry["address"] + entry["size"])
                          if low <= o <= high}
                evidence = "private-reference"
                if not owners and weak:
                    owners = {o for o in classes.get(class_of(entry["name"]), ()) if low <= o <= high}
                    evidence = "same-class (inferred)"
                elif weak:
                    evidence = "private-reference (inferred)"
                if len(owners) == 1:
                    entry.update(owner=owners.pop(), evidence=evidence)
                    changed = True
                    anchored = sorted(anchored + [(entry["address"], entry["owner"])])
                    keys = [a for a, _ in anchored]
        return changed

    # Constructor/destructor table words belong to the file of the function
    # they point to (its __sinit_*/__destroy_global_chain-style routine).
    for section_index, section in sections.items():
        if section["name"] not in UNORDERED:
            continue
        symbolized = {e["address"] for e in items[section_index] if e["owner"] is not None}
        for where, target, addend in relocs:
            if section["address"] <= where < section["address"] + section["size"] and where not in symbolized:
                items[section_index].append(dict(index=None, name=f"@table_{where:x}", address=where, size=4,
                                                 local=True, owner=None, evidence=None, target=target["address"] + addend))
    # Exception-index entries (private) point at their own file's functions.
    index_section = next((i for i, s in sections.items() if s["name"] == "extabindex"), None)
    entry_owner = {e["address"]: e["owner"] for e in items.get(index_section, []) if e["owner"] is not None}
    functions = {e["address"]: e for entries in items.values() for e in entries if e["size"]}
    for where, target, addend in relocs:
        owner = entry_owner.get(where)   # word 0 of an entry is the function pointer
        function = functions.get(target["address"] + addend)
        if owner is not None and function is not None and function["owner"] is None:
            function.update(owner=owner, evidence="exception-index")
    for entries in items.values():
        labels = {e["address"]: e["owner"] for e in entries if e["local"] and e["owner"] is not None}
        for entry in entries:
            if entry["owner"] is None and entry["size"] and entry["address"] in labels:
                entry.update(owner=labels[entry["address"]], evidence="same-address")
    for section_index, section in sections.items():
        for entry in items[section_index] if section["name"] in UNORDERED else []:
            function = functions.get(entry.get("target"))
            if entry["owner"] is None and function is not None and function["owner"] is not None:
                entry.update(owner=function["owner"], evidence="table-target")
    while assign(False):
        pass
    while assign(True):
        pass
    unresolved = []
    for section_index, entries in items.items():
        name = sections[section_index]["name"]
        for entry in entries:
            if entry["owner"] is None and entry["size"]:
                unresolved.append((name, entry["name"], entry["address"]))
    return files, items, sections, unresolved


def ranges(files, items, sections):
    """Per-file [start, end) ranges. Padding goes to the preceding file only when
    the next item belongs to a known file; unresolved stretches stay unowned."""
    result = defaultdict(list)   # file ordinal -> [(section, start, end, alignment)]
    guessed = []                 # unowned stretches attached to a neighbour
    for section_index, entries in items.items():
        section = sections[section_index]
        stop = section["address"] + section["size"]
        sized = sorted((e for e in entries if e["size"] or e["owner"] is not None),
                       key=lambda e: (e["address"], -e["size"]))
        runs = []   # [owner, start, end of last item]
        for entry in sized:
            end = entry["address"] + entry["size"]
            if entry["owner"] is None:
                runs.append([None, entry["address"], end])
            elif runs and runs[-1][0] == entry["owner"]:
                runs[-1][2] = max(runs[-1][2], end)
            else:
                runs.append([entry["owner"], entry["address"], end])
        align = max(section["alignment"], 4)
        for i, (owner, begin, end) in enumerate(runs):
            if owner is None:
                continue
            j = i + 1
            # A split must start aligned (dtk). An unowned stretch that cannot
            # is attached to the preceding file for the split config only.
            while j < len(runs) and runs[j][0] is None and -(-end // align) * align > runs[j][1]:
                guessed.append((section["name"], runs[j][1], runs[j][2], owner))
                end = max(end, runs[j][2])
                j += 1
            following = runs[j] if j < len(runs) else None
            if following is None:
                end = stop
            elif following[0] is not None:
                end = following[1]
            else:
                end = min(-(-end // align) * align, following[1])
            if i == 0:
                begin = section["address"]
            if begin < end:
                result[owner].append((section["name"], begin, end, section["alignment"]))
    return result, guessed


def splits_text(files, owned, header):
    """dtk splits.txt: the original section table, then file ranges in link order."""
    repeats = {name for name in files if files.count(name) > 1}
    blocks = [header.rstrip()]
    for ordinal, name in enumerate(files):
        entries = owned.get(ordinal)
        if not entries:
            continue
        label = f"{Path(name).stem}_{ordinal}{Path(name).suffix}" if name in repeats else name
        lines = []
        for s, a, b, align in sorted(entries, key=lambda r: r[1]):
            rename = RENAMES.get((name, s))
            # Declare the real alignment where a file starts off the section's.
            start_align = a & -a
            lines.append(f"\t{s:<11} start:{a:#010x} end:{b:#010x}" +
                         (f" align:{start_align}" if start_align < align else "") +
                         (f" rename:{rename}" if rename else ""))
        blocks.append(f"{label}:\n" + "\n".join(lines))
    return "\n\n".join(blocks) + "\n"


def main():
    _, elf = load_target()
    files, items, sections, unresolved = build_file_map(elf)
    owned, guessed = ranges(files, items, sections)
    counts = defaultdict(int)
    for entries in items.values():
        for entry in entries:
            counts[entry["evidence"] or "unresolved"] += 1
    repeats = {name for name in files if files.count(name) > 1}
    output = {"files": [{"ordinal": i, "name": name + (f" [{i}]" if name in repeats else ""),
                         "ranges": [{"section": s, "start": hex(a), "end": hex(b)} for s, a, b, _ in owned.get(i, [])]}
                        for i, name in enumerate(files)],
              "unresolved": [{"section": s, "name": n, "address": hex(a)} for s, n, a in unresolved],
              "split_guesses": [{"section": s, "start": hex(a), "end": hex(b), "attached_to": files[o]}
                                for s, a, b, o in guessed],
              "evidence_counts": dict(counts)}
    path = ROOT / "build/audit/file_map.json"
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(output, indent=1) + "\n")
    print(dict(counts))
    print(f"{len(unresolved)} sized symbols without a file; see {path.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
