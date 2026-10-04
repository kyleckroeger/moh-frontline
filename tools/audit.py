#!/usr/bin/env python3
"""Inventory original symbols into ignored research output, without source credit."""
import collections
import hashlib
import json
import subprocess
from pathlib import Path

from formats import Elf32
from setup import CONFIG, ORIG, ROOT, setup


def load_target():
    target = json.loads((CONFIG / "target.json").read_text())
    original = ORIG / target["primary"]
    data = original.read_bytes()
    expected = target["files"][target["primary"]]
    if len(data) != expected["size"] or hashlib.sha256(data).hexdigest() != expected["sha256"]:
        raise ValueError("Original ELF differs from the pinned target; run import_disc.py")
    return original, Elf32(data)


def audit():
    tools = setup()
    original, elf = load_target()
    output = ROOT / "build/audit"
    output.mkdir(parents=True, exist_ok=True)
    symbols = list(elf.symbols())
    counts = collections.Counter(s["type"] for s in symbols)
    summary = dict(target=original.name, sha256=hashlib.sha256(elf.data).hexdigest(),
                   entry=hex(elf.entry), symbol_entries=len(symbols),
                   function_symbol_entries=counts[2], file_symbol_entries=counts[4],
                   object_symbol_entries=counts[1], sections=elf.sections)
    (output / "summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    (output / "symbols.json").write_text(json.dumps(symbols, indent=2) + "\n")
    subprocess.run([str(tools / "dtk"), "dwarf", "dump", "--no-color", str(original),
                    "-o", str(output / "dwarf.txt")], check=True)
    subprocess.run([str(tools / "dtk"), "elf", "config", str(original),
                    str(output / "elf-config")], cwd=output, check=True)
    print(f"{len(symbols)} symbols; {counts[2]} function entries; {counts[4]} file entries")
    print("Research exports: build/audit (Git-ignored; no recovered-source progress)")


if __name__ == "__main__":
    audit()
