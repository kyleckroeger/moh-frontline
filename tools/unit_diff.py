#!/usr/bin/env python3
"""Show which functions of a built unit differ from the original, with disassembly.

Run after reconstruct.py has linked the unit (it keeps the failing unit's
build/reconstruction/units/<id>/compiled.elf). Research output only.
"""
import argparse
import difflib
import json
import re
import subprocess

from audit import load_target
from formats import Elf32
from setup import CONFIG, ROOT, setup

INSTRUCTION = re.compile(r"^\s*[0-9a-f]+:\t")


def disassemble(tools, path, start, size):
    output = subprocess.run([str(tools / "powerpc-eabi-objdump"), "-d", "--no-show-raw-insn", "-M", "gekko",
                             f"--start-address={start:#x}", f"--stop-address={start + size:#x}", str(path)],
                            capture_output=True, text=True, check=True).stdout
    return [line.split(":", 1)[1].strip() for line in output.splitlines() if INSTRUCTION.match(line)]


def diff_unit(unit_id, context):
    tools = setup()
    original_path, original = load_target()
    unit = json.loads((CONFIG / f"{unit_id}.json").read_text())
    linked_path = ROOT / "build/reconstruction/units" / unit_id / "compiled.elf"
    linked = Elf32(linked_path.read_bytes())

    def read(elf, address, size):
        section = next(s for s in elf.sections if s["flags"] & 2 and s["type"] != 8
                       and s["address"] <= address < s["address"] + s["size"])
        return elf.contents(section)[address - section["address"]:address - section["address"] + size]

    differing = 0
    for function in sorted(unit["functions"], key=lambda f: int(f["address"], 16)):
        address = int(function["address"], 16)
        if read(original, address, function["size"]) == read(linked, address, function["size"]):
            continue
        differing += 1
        print(f"== {function['name']} @ {address:#x} ({function['size']} bytes) differs")
        before = disassemble(tools, original_path, address, function["size"])
        after = disassemble(tools, linked_path, address, function["size"])
        for line in difflib.unified_diff(before, after, "original", "built", n=context, lineterm=""):
            print("  " + line)
    print(f"{differing} of {len(unit['functions'])} functions differ")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("unit")
    parser.add_argument("--context", type=int, default=2)
    args = parser.parse_args()
    diff_unit(args.unit, args.context)
