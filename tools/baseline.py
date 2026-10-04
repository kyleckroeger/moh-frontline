#!/usr/bin/env python3
"""Relink the original image through dtk and compare the entire ELF-derived DOL."""
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

from audit import load_target
from formats import Elf32, verify_load_image
from setup import CONFIG, ROOT, setup
from split_config import normalize_symbols, whole_image_splits


def run(*args):
    subprocess.run([str(arg) for arg in args], cwd=ROOT, check=True)


def baseline():
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
    (build / "splits.txt").write_text(whole_image_splits(
        (build / "elf-config/splits.txt").read_text(), elf, original.stem))
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
    pinned, split_sizes = {}, {}
    for unit in units:
        obj = Elf32((ROOT / unit["object"]).read_bytes())
        for section in obj.sections:
            if section["flags"] & 2:
                split_sizes[section["name"]] = split_sizes.get(section["name"], 0) + section["size"]
        for symbol in obj.symbols():
            if symbol["section"] or not symbol["binding"] or not symbol["name"]:
                continue
            if len(originals.get(symbol["name"], [])) != 1:
                raise ValueError(f"No unique original definition: {symbol['name']}")
            pinned[symbol["name"]] = originals[symbol["name"]][0]
    # dtk also omits the bytes of MW linker tables (_rom_copy_info,
    # _bss_init_info, _eti_init_info). GNU ld cannot regenerate them, so a
    # section tail is taken from the original only if pinned tables fill it.
    tails = []
    for section in elf.sections:
        if not section["flags"] & 2 or section["type"] == 8 or not section["size"]:
            continue
        start = section["address"] + split_sizes.get(section["name"], 0)
        cursor, end = start, section["address"] + section["size"]
        for symbol in sorted(pinned.values(), key=lambda s: s["address"]):
            if symbol["section"] == section["index"] and symbol["address"] == cursor and symbol["size"]:
                cursor += symbol["size"]
        # MW terminates the constructor and destructor tables with a null word.
        terminator = elf.contents(section)[-4:] == bytes(4)
        if section["name"] in (".ctors", ".dtors") and cursor == end - 4 and terminator:
            cursor = end
        if cursor != end:
            raise ValueError(f"Split omits original bytes outside linker tables: {section['name']}")
        if start < end:
            tails.append((section, start, end))
    (build / "linker-tables.s").write_text("".join(
        f'.section .linker_tables.{section["name"].lstrip(".")},"a",@progbits\n'
        f'.incbin "{original}",{section["offset"] + start - section["address"]},{end - start}\n'
        for section, start, end in tails))
    run(tools / "powerpc-eabi-as", "-o", build / "linker-tables.o", build / "linker-tables.s")
    script = "ENTRY(__start)\nSECTIONS {\n"
    script += f"__start = {config['entry']};\n_SDA_BASE_ = {config['sda_base']};\n_SDA2_BASE_ = {config['sda2_base']};\n"
    script += "".join(f"{name} {address} : {{ *({name}) *(.linker_tables.{name.lstrip('.')}) }}\n"
                      for name, address in config["sections"].items())
    script += "".join(f"{name} = {symbol['address']:#x};\n" for name, symbol in pinned.items())
    script += "}\n"
    (build / "link.ld").write_text(script)
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
    baseline()
