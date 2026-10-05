#!/usr/bin/env python3
"""Generate objdiff.json so the accepted units can be diffed locally.

Iteration tooling only: this does not verify anything and earns no progress
credit. Target objects are exact per-unit slices of the original produced by
the pinned dtk; base objects are the verified compiler objects from
`build/reconstruction`. Run it after `tools/reconstruct.py`. The generated
`objdiff.json` is ignored by git.

    python3 tools/objdiff_config.py
    objdiff            # open the generated config
"""
import argparse
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

from audit import load_target
from formats import Elf32
from project_build import target_section
from setup import CONFIG, ROOT, setup
from split_config import normalize_symbols

DEST = ROOT / "build/objdiff"
SCHEMA = "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json"
PROGRESS_CATEGORIES = [
    {"id": "reconstructed_game", "name": "Reconstructed game"},
    {"id": "restored_library", "name": "Restored library"},
]


def run(*args):
    subprocess.run([str(arg) for arg in args], cwd=ROOT, check=True)


def unit_ranges(unit):
    """(target-section, start, end) for every manifest section of a unit."""
    ranges = []
    for section in unit["sections"]:
        address = int(section["address"], 16)
        ranges.append((target_section(section), address, address + section["size"]))
    return ranges


def split_ranges(units, bounds):
    """Per-unit split ranges. A manifest range is exact kept bytes; a unit may
    end mid-word, which dtk cannot auto-fill around. Round each end up to the
    next word, absorbing a smaller gap by meeting the next unit's start. Starts
    stay exact; the gap before the first unit begins on a section boundary."""
    entries = {}
    for unit in units:
        for name, start, end in unit_ranges(unit):
            entries.setdefault(name, []).append([unit["id"], start, end])
    ranges = {unit["id"]: [] for unit in units}
    for name, group in entries.items():
        _, section_end = bounds[name]
        group.sort(key=lambda entry: entry[1])
        for index, entry in enumerate(group):
            unit_id, start, end = entry
            following = group[index + 1][1] if index + 1 < len(group) else section_end
            aligned = (end + 3) & ~3
            ranges[unit_id].append((name, start, aligned if aligned <= following else following))
    return ranges


def splits_text(header, units, ranges):
    """dtk splits covering exactly the accepted units; dtk auto-fills the rest."""
    blocks = []
    for unit in sorted(units, key=lambda u: min(start for _, start, _ in ranges[u["id"]])):
        lines = [f"{unit['id']}:"]
        for name, start, end in sorted(ranges[unit["id"]]):
            lines.append(f"\t{name:<11} start:{start:#010x} end:{end:#010x}")
        blocks.append("\n".join(lines))
    return header.rstrip("\n") + "\n\n" + "\n\n".join(blocks) + "\n"


def relative(path):
    return Path(path).relative_to(ROOT).as_posix()


def generate():
    tools = setup()
    original_path, elf = load_target()
    baseline = json.loads((CONFIG / "baseline.json").read_text())
    project = json.loads((CONFIG / "project.json").read_text())
    units = [json.loads((CONFIG / name).read_text()) for name in project["units"]]
    bounds = {section["name"]: (section["address"], section["address"] + section["size"])
              for section in elf.sections if section["flags"] & 2 and section["size"]}

    if DEST.exists():
        shutil.rmtree(DEST)
    DEST.mkdir(parents=True)
    reference = DEST / "reference.dol"
    run(tools / "dtk", "elf2dol", original_path, reference)
    digest = hashlib.sha1(reference.read_bytes()).hexdigest()
    if digest != baseline["normalized_dol_sha1"]:
        raise ValueError("ELF conversion differs from the pinned reference")
    run(tools / "dtk", "elf", "config", original_path, DEST / "elf-config")
    header = (DEST / "elf-config/splits.txt").read_text().split("\n\n", 1)[0]
    (DEST / "symbols.txt").write_text(normalize_symbols((DEST / "elf-config/symbols.txt").read_text()))
    (DEST / "splits.txt").write_text(splits_text(header, units, split_ranges(units, bounds)))
    (DEST / "config.yml").write_text(
        "object: " + json.dumps(str(reference)) + "\n"
        "hash: " + baseline["normalized_dol_sha1"] + "\n"
        "symbols: " + json.dumps(str(DEST / "symbols.txt")) + "\n"
        "splits: " + json.dumps(str(DEST / "splits.txt")) + "\nquick_analysis: true\n")
    run(tools / "dtk", "dol", "split", DEST / "config.yml", DEST / "split")
    targets = {entry["name"]: ROOT / entry["object"]
               for entry in json.loads((DEST / "split/config.json").read_text())["units"]}

    objdiff_units = []
    for unit in units:
        target = targets.get(unit["id"])
        base = ROOT / "build/reconstruction/units" / unit["id"] / "compiled.o"
        if target is None or not target.exists():
            raise ValueError(f"dtk did not split a target object for {unit['id']}")
        if not base.exists():
            raise ValueError(f"missing base object for {unit['id']}; run tools/reconstruct.py first")
        target_symbols = {symbol["name"] for symbol in Elf32(target.read_bytes()).symbols()
                          if symbol["type"] == 2 and symbol["section"]}
        mappings = {}
        for function in unit["functions"]:
            if function["name"] in target_symbols:
                continue
            # dtk globalizes duplicate local labels by appending "_<ADDRESS>".
            suffix = function["address"][2:].upper()
            for candidate in (f"{function['name']}_{suffix}", f"{function['name']}_{suffix.lower()}"):
                if candidate in target_symbols:
                    mappings[candidate] = function["name"]
                    break
        entry = {
            "name": unit["id"],
            "target_path": relative(target),
            "base_path": relative(base),
            "metadata": {"complete": True, "source_path": unit["source"],
                         "progress_categories": [unit["category"]]},
        }
        if mappings:
            entry["symbol_mappings"] = mappings
        objdiff_units.append(entry)

    config = {"$schema": SCHEMA, "min_version": "2.0.0",
              "build_target": False, "build_base": False,
              "units": objdiff_units, "progress_categories": PROGRESS_CATEGORIES}
    (ROOT / "objdiff.json").write_text(json.dumps(config, indent=2) + "\n")
    print(f"Wrote objdiff.json: {len(objdiff_units)} units")


if __name__ == "__main__":
    argparse.ArgumentParser(description=__doc__).parse_args()
    generate()
