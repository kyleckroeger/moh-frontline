#!/usr/bin/env python3
"""Relink the original image through dtk and compare the entire ELF-derived DOL."""
import argparse
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

from audit import load_target
from formats import Elf32, verify_load_image
from setup import CONFIG, ROOT, setup
from file_map import build_file_map, ranges, splits_text
from split_config import normalize_symbols, whole_image_splits


def run(*args):
    subprocess.run([str(arg) for arg in args], cwd=ROOT, check=True)


def baseline(per_file=True):
    tools = setup()
    original, elf = load_target()
    config = json.loads((CONFIG / "baseline.json").read_text())
    build = ROOT / "build/baseline"
    if build.exists():
        shutil.rmtree(build)
    build.mkdir(parents=True)
    reference = build / "reference.dol"
    run(tools / "dtk", "elf2dol", original, reference)
    expected = reference.read_bytes()
    if len(expected) != config["normalized_dol_size"] or hashlib.sha1(expected).hexdigest() != config["normalized_dol_sha1"]:
        raise ValueError("ELF conversion differs from the pinned reference")
    verify_load_image(elf, expected)
    run(tools / "dtk", "elf", "config", original, build / "elf-config")
    (build / "symbols.txt").write_text(normalize_symbols((build / "elf-config/symbols.txt").read_text()))
    dtk_splits = (build / "elf-config/splits.txt").read_text()
    if per_file:
        files, items, sections, _ = build_file_map(elf)
        owned, _ = ranges(files, items, sections)
        splits = splits_text(files, owned, dtk_splits.split("\n\n", 1)[0])
    else:
        splits = whole_image_splits(dtk_splits, elf, original.stem)
    (build / "splits.txt").write_text(splits)
    (build / "config.yml").write_text(
        "object: " + json.dumps(str(reference)) + "\n"
        "hash: " + config["normalized_dol_sha1"] + "\n"
        "symbols: " + json.dumps(str(build / "symbols.txt")) + "\n"
        "splits: " + json.dumps(str(build / "splits.txt")) + "\nquick_analysis: true\n")
    run(tools / "dtk", "dol", "split", build / "config.yml", build / "split")
    units = json.loads((build / "split/config.json").read_text())["units"]
    # dtk leaves MW linker-generated symbols (stack, arena, init tables)
    # undefined. Pin each to its unique original definition, nothing else.
    originals = {}
    for symbol in elf.symbols():
        if symbol["section"] and symbol["binding"] and symbol["type"] != 4:
            originals.setdefault(symbol["name"], []).append(symbol)
    defined, undefined = set(), set()
    for unit in units:
        for symbol in Elf32((ROOT / unit["object"]).read_bytes()).symbols():
            if symbol["binding"] and symbol["name"]:
                (defined if symbol["section"] else undefined).add(symbol["name"])
    pinned = {}
    for name in sorted(undefined - defined):
        if len(originals.get(name, [])) != 1:
            raise ValueError(f"No unique original definition: {name}")
        pinned[name] = originals[name][0]
    # dtk also omits the bytes of MW linker tables (_rom_copy_info,
    # _bss_init_info, _eti_init_info) and the null words ending .ctors/.dtors.
    # GNU ld cannot regenerate them: take each section's tail from the
    # original, made only of pinned tables and that terminator.
    tails = []
    for section in elf.sections:
        if not section["flags"] & 2 or section["type"] == 8 or not section["size"]:
            continue
        end = section["address"] + section["size"]
        cursor = end
        if section["name"] in (".ctors", ".dtors") and elf.contents(section)[-4:] == bytes(4):
            cursor -= 4
        for symbol in sorted(pinned.values(), key=lambda s: -s["address"]):
            if symbol["section"] == section["index"] and symbol["size"] and symbol["address"] + symbol["size"] == cursor:
                cursor = symbol["address"]
        if cursor < end:
            tails.append((section, cursor, end))
    (build / "linker-tables.s").write_text("".join(
        f'.section .linker_tables.{section["name"].lstrip(".")},"a",@progbits\n'
        f'.incbin "{original}",{section["offset"] + start - section["address"]},{end - start}\n'
        for section, start, end in tails))
    run(tools / "powerpc-eabi-as", "-o", build / "linker-tables.o", build / "linker-tables.s")
    script = "ENTRY(__start)\nSECTIONS {\n"
    script += f"__start = {config['entry']};\n_SDA_BASE_ = {config['sda_base']};\n_SDA2_BASE_ = {config['sda2_base']};\n"
    script += "".join(f"{name} {address} : {{ *(SORT_BY_NAME({name}$*)) *({name}) *(.linker_tables.{name.lstrip('.')}) }}\n"
                      for name, address in config["sections"].items())
    script += "".join(f"{name} = {symbol['address']:#x};\n" for name, symbol in pinned.items())
    script += "}\n"
    (build / "link.ld").write_text(script)
    # GNU ld reads "name@version" in global names, but MW uses '@' in thunks
    # (@32@f) and dtk globalizes duplicate @N labels. Rename them in every
    # object; names do not affect the linked bytes.
    versioned = set()
    for unit in units:
        versioned |= {s["name"] for s in Elf32((ROOT / unit["object"]).read_bytes()).symbols()
                      if s["binding"] and "@" in s["name"]}
    if versioned:
        (build / "rename.syms").write_text("".join(f"{n} {n.replace('@', '$at$')}\n" for n in sorted(versioned)))
        for unit in units:
            run(tools / "powerpc-eabi-objcopy", f"--redefine-syms={build / 'rename.syms'}", ROOT / unit["object"])
    run(tools / "powerpc-eabi-ld", "-e", config["entry"], "-T", build / "link.ld",
        "-o", build / "relinked.elf", *[u["object"] for u in units], build / "linker-tables.o")
    run(tools / "dtk", "elf2dol", build / "relinked.elf", build / "relinked.dol")
    actual = (build / "relinked.dol").read_bytes()
    if actual != expected:
        raise ValueError("Complete reference comparison failed")
    verify_load_image(elf, actual)
    report = dict(status="identical", target=original.name,
                  original_elf_sha256=hashlib.sha256(elf.data).hexdigest(),
                  comparison="complete ELF-derived DOL; every allocated ELF byte also checked",
                  dol_sha1=hashlib.sha1(actual).hexdigest(), dol_size=len(actual),
                  reconstructed_source_bytes=0, original_object_count=len(units),
                  tool_lock_sha256=hashlib.sha256((ROOT / "tools/toolchain.json").read_bytes()).hexdigest())
    (build / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    print(f"Baseline verified: {len(actual)} bytes, SHA-1 {report['dol_sha1']}")
    print("Original objects only. No reconstructed-source or full-original-ELF match is claimed.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--whole-image", action="store_true",
                        help="Split one object spanning every section instead of per-file objects")
    baseline(per_file=not parser.parse_args().whole_image)
