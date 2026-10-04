#!/usr/bin/env python3
"""Draft a unit manifest by matching a compiled object to the original.

The original executable keeps its relocations. Each relocation in the compiled
object is paired with the original relocation at the same code or data offset,
which fixes the address of every compiled section and every external without
guessing. Named functions and objects anchor sections that have no references.
The draft earns nothing by itself: reconstruct.py must verify every byte.
"""
import argparse
import json
import struct
from pathlib import Path

from audit import load_target
from formats import Elf32
from sdk_build import compile_sdk
from setup import CONFIG, ROOT
from setup_compiler import setup_compiler
from tool_runner import run

SDA21 = 109


def relocations(elf):
    """Yield (target section index, offset, type, symbol, addend) for allocated targets."""
    symbols = list(elf.symbols())
    for section in elf.sections:
        if section["type"] != 4 or not elf.sections[section["info"]]["flags"] & 2:
            continue
        data = elf.contents(section)
        for offset in range(0, len(data), 12):
            where, info, addend = struct.unpack_from(">IIi", data, offset)
            yield section["info"], where, info & 255, symbols[info >> 8], addend


class Original:
    def __init__(self, elf):
        self.elf = elf
        self.symbols = list(elf.symbols())
        self.relocs = {where: (kind, symbol, addend) for _, where, kind, symbol, addend in relocations(elf)}
        self.files = [i for i, s in enumerate(self.symbols) if s["type"] == 4]

    def file_index(self, name, index=None):
        matches = [i for i in self.files if self.symbols[i]["name"] == name]
        if index is not None:
            if index not in matches:
                raise ValueError(f"Symbol {index} is not a file record for {name}")
            return index
        if len(matches) != 1:
            raise ValueError(f"{len(matches)} file records named {name}; pass --file-index")
        return matches[0]

    def locals_of(self, file_index):
        result = {}
        for symbol in self.symbols[file_index + 1:]:
            if symbol["type"] == 4:
                break
            if symbol["binding"] == 0 and symbol["section"] and symbol["name"]:
                result.setdefault(symbol["name"], []).append(symbol)
        return result

    def globals(self, name):
        return [s for s in self.symbols if s["name"] == name and s["binding"] and s["section"]
                and s["type"] != 4 and s["section"] < 0xFF00]


def anchor(addresses, name, value):
    if addresses.setdefault(name, value) != value:
        raise ValueError(f"Inconsistent address for {name}: {addresses[name]:#x} vs {value:#x}")


def find(symbol, original, locals_):
    candidates = locals_.get(symbol["name"], []) if symbol["binding"] == 0 else original.globals(symbol["name"])
    if symbol["binding"] == 0 and not candidates:
        # MW numbers function statics per compilation: name$123.
        stem = symbol["name"].split("$", 1)[0]
        candidates = [s for key, items in locals_.items() if key.split("$", 1)[0] == stem for s in items]
    return candidates[0] if len(candidates) == 1 else None


def text_layout(obj, original, locals_):
    """Keep functions the original has; discard absent ones; reject size changes."""
    text = next(s for s in obj.sections if s["name"] == ".text")
    functions = sorted((s for s in obj.symbols() if s["type"] == 2 and s["section"] == text["index"]),
                       key=lambda s: s["address"])
    kept, discarded, packed = [], [], 0
    for function in functions:
        target = find(function, original, locals_)
        absent = not (locals_.get(function["name"]) if function["binding"] == 0 else original.globals(function["name"]))
        if target is not None and target["size"] == function["size"]:
            kept.append((function, packed))
            packed += function["size"]
        elif absent:
            discarded.append(function)
        else:
            raise ValueError(f"Function differs from the original: {function['name']}")
    return text["index"], kept, discarded, packed


def match(obj, original, file_index):
    """Return section addresses, external addresses and the stripped text layout."""
    symbols = list(obj.symbols())
    locals_ = original.locals_of(file_index)
    names = {s["index"]: s["name"] for s in obj.sections}
    addresses, externals = {}, {}
    text_index, kept, discarded, text_size = text_layout(obj, original, locals_)

    def position(section, offset):
        """Offset after stripping: discarded code has no position."""
        if section != text_index:
            return offset
        for function, packed in kept:
            if function["address"] <= offset < function["address"] + function["size"]:
                return packed + offset - function["address"]
        return None

    # Named functions and objects anchor their sections.
    for symbol in symbols:
        if symbol["section"] and symbol["section"] < 0xFF00 and symbol["type"] in (1, 2) and not symbol["name"].startswith("@"):
            target = find(symbol, original, locals_)
            offset = position(symbol["section"], symbol["address"])
            if target is not None and target["size"] == symbol["size"] and offset is not None:
                anchor(addresses, names[symbol["section"]], target["address"] - offset)
    if ".text" not in addresses:
        raise ValueError("No compiled function was found in the original file record")
    # Relocations pin the rest; repeat until no new section is placed.
    changed = True
    while changed:
        changed = False
        for section, where, kind, symbol, addend in relocations(obj):
            base = addresses.get(names[section])
            where = position(section, where)
            if base is None or where is None or base + where not in original.relocs:
                continue
            okind, osymbol, oaddend = original.relocs[base + where]
            if okind != kind:
                raise ValueError(f"Relocation type differs at {base + where:#x}")
            target = osymbol["address"] + oaddend
            if symbol["section"] == 0:
                anchor(externals, symbol["name"], target - addend)
            elif symbol["section"] < 0xFF00:
                offset = position(symbol["section"], symbol["address"] + addend)
                if offset is None:
                    raise ValueError(f"Kept code references discarded code at {base + where:#x}")
                name = names[symbol["section"]]
                if name not in addresses:
                    changed = True
                anchor(addresses, name, target - offset)
    return addresses, externals, kept, discarded, text_size


def referenced(obj, index, kept, addresses):
    """Whether retained code, or a placed section, refers into a section."""
    names = {s["index"]: s["name"] for s in obj.sections}
    text = next(s["index"] for s in obj.sections if s["name"] == ".text")
    for section, where, _, symbol, _ in relocations(obj):
        if symbol["section"] != index:
            continue
        if section == text and any(f["address"] <= where < f["address"] + f["size"] for f, _ in kept):
            return True
        if section != text and names[section] in addresses:
            return True
    return False


def compare_functions(obj, original, file_index):
    """Print compiled vs original function layout to diagnose a failed match."""
    locals_ = original.locals_of(file_index)
    for symbol in sorted(obj.symbols(), key=lambda s: s["address"]):
        if symbol["type"] != 2 or not symbol["section"]:
            continue
        found = locals_.get(symbol["name"], []) if symbol["binding"] == 0 else original.globals(symbol["name"])
        where = ", ".join(f"{s['address']:#x} size {s['size']}" for s in found) or "absent"
        flag = "" if len(found) == 1 and found[0]["size"] == symbol["size"] else "  <--"
        print(f"  +{symbol['address']:#06x} size {symbol['size']:5}  {symbol['name']:40} original: {where}{flag}")


def draft(args):
    reference = json.loads(Path(args.like).read_text()) if args.like else {}
    unit = {"id": args.id, "source": args.source, "category": args.category,
            "compiler_family": "GC", "compiler_version": args.compiler or reference["compiler_version"],
            "compiler_flags": reference.get("compiler_flags", []), "include_dirs": reference.get("include_dirs", []),
            "strip_unused": False}
    if "source_root" in reference:
        unit["source_root"] = reference["source_root"]
    if args.define:
        names = tuple(f"-D{d.split('=')[0]}=" for d in args.define)
        unit["compiler_flags"] = [f for f in unit["compiler_flags"] if not f.startswith(names)]
        unit["compiler_flags"] += [f"-D{d}" for d in args.define]
    _, elf = load_target()
    original = Original(elf)
    file_index = original.file_index(args.original_file or Path(args.source).name, args.file_index)
    work = ROOT / "scratch/port" / args.id
    work.mkdir(parents=True, exist_ok=True)
    compiler, wrapper = setup_compiler(unit["compiler_version"], "GC")
    compile_sdk(unit, compiler, wrapper, work, lambda stage, *a: run(a, work, work / f"{stage}.log"))
    obj = Elf32((work / "compiled.o").read_bytes())
    try:
        addresses, externals, kept, discarded, text_size = match(obj, original, file_index)
    except ValueError:
        compare_functions(obj, original, file_index)
        raise
    sections, stripped = [], []
    for section in obj.sections:
        if not section["flags"] & 2 or not section["size"]:
            continue
        if section["name"] not in addresses:
            if discarded and not referenced(obj, section["index"], kept, addresses):
                stripped.append(section["name"])
                continue
            raise ValueError(f"Could not place compiled section {section['name']}")
        size = text_size if section["name"] == ".text" else section["size"]
        sections.append({"name": section["name"], "address": hex(addresses[section["name"]]),
                         "size": size, "type": section["type"]})
    text = next(s for s in sections if s["name"] == ".text")
    start = int(text["address"], 16)
    functions = [{"name": s["name"], "address": hex(s["address"]), "size": s["size"], "binding": s["binding"]}
                 for s in original.symbols if s["type"] == 2 and s["section"]
                 and start <= s["address"] < start + text["size"]]
    obj_symbols = list(obj.symbols())
    undefined = sorted({s["name"] for s in obj_symbols if s["binding"] and not s["section"] and s["name"]})
    for name in undefined:
        if name not in externals and len(original.globals(name)) == 1:
            externals[name] = original.globals(name)[0]["address"]
    unit["strip_unused"] = bool(discarded)
    locals_ = original.locals_of(file_index)
    sda = sorted({symbol["name"] for _, _, kind, symbol, _ in relocations(obj) if kind == SDA21 and not symbol["section"]})
    unit.update(
        sections=sections, functions=functions,
        externals={name: hex(externals[name]) for name in undefined if name in externals},
        undefined_in_discarded_code=[name for name in undefined if name not in externals],
        discarded_functions=[f["name"] for f in discarded],
        upstream=dict(reference.get("upstream", {}), **({"path": args.upstream_path} if args.upstream_path else {})),
        original_file=original.symbols[file_index]["name"], original_file_index=file_index,
        local_externals=sorted(n for n in externals if n in undefined and n in locals_ and not original.globals(n)),
        discarded_local_functions=[f["name"] for f in discarded if f["binding"] == 0], sda_externals=sda)
    if stripped:
        unit["stripped_sections"] = stripped
    if args.like and "id" in reference:
        unit["adapted_from"] = {"repository": "https://github.com/lifewillbeokay/moh-rising-sun",
                                "manifest": Path(args.like).name}
    output = CONFIG / f"{args.id}.json"
    output.write_text(json.dumps(unit, indent=2) + "\n")
    print(f"Drafted {output.relative_to(ROOT)}: {len(functions)} functions, {text['size']} code bytes at {text['address']}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("id")
    parser.add_argument("source", help="Source path relative to the repository root")
    parser.add_argument("--like", help="Existing manifest to copy compiler settings and provenance from")
    parser.add_argument("--original-file", help="Original file-record name (default: source basename)")
    parser.add_argument("--file-index", type=int)
    parser.add_argument("--compiler")
    parser.add_argument("--define", action="append", help="Replace or add a -D definition, e.g. SDK_REVISION=0")
    parser.add_argument("--category", default="restored_library")
    parser.add_argument("--upstream-path", help="Path of the source in the upstream repository")
    draft(parser.parse_args())
