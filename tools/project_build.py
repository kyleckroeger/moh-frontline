"""Compile reviewed source units and check every byte of the complete image."""
import hashlib
import json
import os
import re
import shutil
import struct
from pathlib import Path

from audit import load_target
from formats import Elf32, verify_load_image
from tool_runner import run, sha256
from setup import CONFIG, ROOT, setup
from setup_compiler import setup_compiler
from sdk_build import compile_sdk, resolve_external, sdk_link_script, stripped_address


def allocated(elf):
    return [s for s in elf.sections if s["flags"] & 2 and s["size"]]


def function_records(elf):
    return sorted((s["name"], s["address"], s["size"], s["binding"])
                  for s in elf.symbols() if s["type"] == 2 and s["section"])


def manifest_functions(unit):
    return sorted((f["name"], int(f["address"], 16), f["size"], f["binding"])
                  for f in unit["functions"])


def object_record(symbol):
    name = symbol["name"]
    # MW and SN append compilation-local sequence numbers to function statics.
    # Compare their C identifier, binding, exact address and size instead.
    if symbol["binding"] == 0 and re.fullmatch(r"[A-Za-z_]\w*\$\d+", name):
        name = name.rsplit("$", 1)[0]
    if symbol["binding"] == 0 and re.fullmatch(r"[A-Za-z_]\w*\.\d+", name):
        name = name.rsplit(".", 1)[0]
    return name, symbol["address"], symbol["size"], symbol["binding"]


def kept_slice(section):
    """MW strips unused leading or trailing data objects: [start, end) of compiled bytes kept."""
    start = section.get("linked_offset", 0)
    return start, start + section["size"]


def target_section(section):
    """Permit read-only subsections and named native SN vtables, never code/BSS."""
    name = section["name"]
    target = section.get("target_name", name)
    readonly = target == ".rodata" and name.startswith(".rodata.")
    vtable = target == ".data" and re.fullmatch(r"\.gnu\.linkonce\.d\._vt\.[A-Za-z0-9_]+", name)
    if target != name and (not (readonly or vtable) or section.get("type", 1) != 1):
        raise ValueError("Unsupported source section mapping")
    return target


def check_vtable_storage(elf, section, address, binding):
    """A linkonce mapping must own exactly one complete, named compiler vtable."""
    name = section["name"].removeprefix(".gnu.linkonce.d.")
    objects = [s for s in elf.symbols() if s["name"] == name and s["section"]]
    if (len(objects) != 1 or objects[0]["type"] != 1
            or objects[0]["address"] != address or objects[0]["size"] != section["size"]
            or objects[0]["binding"] not in binding):
        raise ValueError("Native vtable object ownership differs")
    overlapping = [s for s in elf.symbols() if s["size"] and s["type"] in (0, 1)
                   and s["section"] == objects[0]["section"]
                   and s["address"] < address + section["size"]
                   and address < s["address"] + s["size"]]
    if overlapping != objects:
        raise ValueError("Native vtable has overlapping object definitions")
    return objects[0]


def validate_units(original, units):
    """Pin ownership to original symbols and reject duplicate coverage."""
    symbols = list(original.symbols())
    original_functions = function_records(original)
    names = {s["name"] for s in symbols}
    intervals = []
    if len({u["id"] for u in units}) != len(units):
        raise ValueError("Duplicate unit identifier")
    for unit in units:
        file_index = unit.get("original_file_index")
        if file_index is not None:
            if (not isinstance(file_index, int) or not 0 <= file_index < len(symbols)
                    or symbols[file_index]["type"] != 4
                    or symbols[file_index]["name"] != unit.get("original_file")):
                raise ValueError("Original source-file record differs")
        if unit["category"] not in ("reconstructed_game", "restored_library"):
            raise ValueError("Unknown source category")
        retain_functions = unit.get("retain_function_symbols", False)
        if not isinstance(retain_functions, bool) or (retain_functions and not unit["strip_unused"]):
            raise ValueError("Function-symbol retention requires native unused-code stripping")
        if not set(unit.get("sda_externals", [])) <= set(unit["externals"]):
            raise ValueError("Small-data external lacks an original definition")
        local_externals = set(unit.get("local_externals", []))
        if (not local_externals <= set(unit["externals"])
                or (local_externals and not unit.get("original_file"))):
            raise ValueError("Private external dependency scope is missing")
        text = next(s for s in unit["sections"] if s["name"] == ".text")
        start, end = int(text["address"], 16), int(text["address"], 16) + text["size"]
        functions = manifest_functions(unit)
        if functions != [f for f in original_functions if start <= f[1] < end]:
            raise ValueError(f"Original function coverage differs: {unit['id']}")
        cursor = start
        for _, address, size, _ in sorted(functions, key=lambda f: f[1]):
            if address != cursor or size <= 0:
                raise ValueError("Code coverage has gaps or overlaps")
            cursor += size
        if cursor != end:
            raise ValueError("Code coverage excludes part of a function")
        for section in unit["sections"]:
            address, size = int(section["address"], 16), section["size"]
            matches = [s for s in original.sections if s["name"] == target_section(section) and
                       s["flags"] & 2 and s["type"] == section.get("type", 1) and size > 0 and
                       s["address"] <= address < address + size <= s["address"] + s["size"]]
            if len(matches) != 1:
                raise ValueError("Source section is outside the original section")
            if section["name"].startswith(".gnu.linkonce.d."):
                obj = check_vtable_storage(original, section, address, (1, 2))
                if obj["section"] != matches[0]["index"]:
                    raise ValueError("Native vtable belongs to another section")
            intervals.append((address, address + size, unit["id"], section["name"]))
        for name, address in unit["externals"].items():
            resolve_external(original, unit, name)
            if start <= int(address, 16) < end:
                raise ValueError(f"External is not an original definition: {name}")
        local_discards = set(unit.get("discarded_local_functions", []))
        if not local_discards <= set(unit["discarded_functions"]) or (local_discards and not unit.get("original_file")):
            raise ValueError("Discarded local function scope is missing")
        file_name, local_names, linkable = None, set(), set()
        for symbol in symbols:
            if symbol["type"] == 4:
                file_name = symbol["name"]
            if file_name == unit.get("original_file") and symbol["binding"] == 0:
                local_names.add(symbol["name"])
            if symbol["binding"] or file_name == unit.get("original_file"):
                linkable.add(symbol["name"])
        for name in unit["discarded_functions"]:
            if name in names and (name not in local_discards or name in local_names):
                raise ValueError(f"Cannot discard a symbol present in the target: {name}")
        # MW emits unreferenced declarations as undefined symbols. A target
        # symbol may share the name only as another source file's private one.
        for name in unit["undefined_in_discarded_code"]:
            if name in linkable:
                raise ValueError(f"Cannot discard a symbol present in the target: {name}")
    intervals.sort()
    for a, b in zip(intervals, intervals[1:]):
        if b[0] < a[1]:
            raise ValueError(f"Source ranges overlap: {a[2:]} and {b[2:]}")
    return intervals


def validate_object(obj, unit):
    if obj.kind != 1:
        raise ValueError("Expected a relocatable compiler object")
    symbols = list(obj.symbols())
    functions = [s for s in symbols if s["type"] == 2 and s["section"]]
    expected = {f["name"] for f in unit["functions"]} | set(unit["discarded_functions"])
    if len(functions) != len(expected) or {s["name"] for s in functions} != expected:
        raise ValueError("Unexpected compiler function set")
    output = {s["name"]: s for s in allocated(obj)}
    # MW's linker also dead-strips data. A compiler section that only discarded
    # code references is declared; verify_unit proves the link removed it.
    stripped = set(unit.get("stripped_sections", []))
    kept = {s["name"] for s in unit["sections"]}
    if (len(output) != len(allocated(obj)) or set(output) != kept | stripped or kept & stripped
            or (stripped and not unit["strip_unused"])):
        raise ValueError("Unexpected allocated compiler output")
    text = output[".text"]
    if text["flags"] != 6 or text["type"] != 1 or text["address"] != 0:
        raise ValueError("Unexpected compiler text")
    cursor = 0
    for f in sorted(functions, key=lambda s: s["address"]):
        if f["section"] != text["index"] or f["address"] != cursor or f["size"] <= 0:
            raise ValueError("Unaccounted compiler code")
        cursor += f["size"]
    if cursor != text["size"]:
        raise ValueError("Unaccounted compiler text bytes")
    trimmed = {}
    for section in unit["sections"]:
        actual = output[section["name"]]
        start, end = kept_slice(section)
        if (actual["type"] != section.get("type", 1) or
                (section["name"] != ".text" and actual["size"] != section.get("linked_size", section["size"])) or
                (section["name"] == ".text" and "linked_offset" in section)):
            raise ValueError("Compiler data layout differs from manifest")
        if section["name"] != ".text" and (start, end) != (0, actual["size"]):
            if not unit["strip_unused"]:
                raise ValueError("Trimmed data requires native unused-code stripping")
            objects = [(s["address"], s["address"] + s["size"]) for s in symbols
                       if s["section"] == actual["index"] and s["type"] == 1 and s["size"]]
            if any(a < start < b or a < end < b for a, b in objects):
                raise ValueError("Trimmed data cuts through a compiler object")
            for a, b in ((0, start), (end, actual["size"])):
                if a < b and not any(a <= x and y <= b for x, y in objects):
                    raise ValueError("Trimmed data holds no complete compiler object")
            trimmed[actual["index"]] = (start, end)
        if section["name"].startswith(".gnu.linkonce.d."):
            obj_symbol = check_vtable_storage(obj, section, 0, (2,))
            if actual["flags"] != 3 or obj_symbol["section"] != actual["index"]:
                raise ValueError("Native vtable compiler section differs")
    absent = set(unit["undefined_in_discarded_code"])
    undefined = {s["name"] for s in symbols if s["binding"] and not s["section"] and s["name"]}
    if undefined != set(unit["externals"]) | absent:
        raise ValueError("Unexpected compiler dependencies")
    discarded = [s for s in functions if s["name"] in unit["discarded_functions"]]
    declared_local = set(unit.get("discarded_local_functions", []))
    if any(s["binding"] != 0 for s in discarded if s["name"] in declared_local):
        raise ValueError("Discarded function is not local to this input")
    small_externals = set()
    # SN retains unused declaration symbols in its executable symbol table.
    # Every relocation to an absent dependency must be in code being discarded.
    for section in obj.sections:
        if section["type"] == 9:
            raise ValueError("Unsupported implicit-addend relocation")
        if section["type"] != 4:
            continue
        if section["entry_size"] != 12 or section["size"] % 12:
            raise ValueError("Unexpected relocation format")
        if not obj.sections[section["info"]]["flags"] & 2:
            continue  # Debug references do not survive into the load image.
        for offset in range(0, section["size"], 12):
            location, info, _ = struct.unpack_from(">IIi", obj.contents(section), offset)
            symbol = symbols[info >> 8]
            if info & 255 == 109 and not symbol["section"]:
                small_externals.add(symbol["name"])
            if symbol["section"] in trimmed:
                _, _, addend = struct.unpack_from(">IIi", obj.contents(section), offset)
                start, end = trimmed[symbol["section"]]
                retained = (not any(f["address"] <= location < f["address"] + f["size"] for f in discarded)
                            if section["info"] == text["index"] else
                            section["info"] not in trimmed or
                            trimmed[section["info"]][0] <= location < trimmed[section["info"]][1])
                if retained and not start <= symbol["address"] + addend < end:
                    raise ValueError("Retained code or data references trimmed data")
            if symbol["name"] in absent:
                if section["info"] != text["index"] or not any(
                        f["address"] <= location < f["address"] + f["size"] for f in discarded):
                    raise ValueError("Retained code or data references an absent dependency")
    if small_externals != set(unit.get("sda_externals", [])):
        raise ValueError("Small-data external dependencies differ")


def verify_unit(original, linked, unit):
    if linked.kind != 2:
        raise ValueError("Expected a linked source executable")
    actual_sections = allocated(linked)
    stripped = unit.get("stripped_sections", [])
    parked = sorted((s["name"], s["address"]) for s in actual_sections if s["name"] in stripped)
    if parked != sorted((name, stripped_address(i)) for i, name in enumerate(stripped)):
        raise ValueError("Stripped sections were not parked outside the image")
    actual_sections = [s for s in actual_sections if s["name"] not in stripped]
    expected = sorted((s["name"], int(s["address"], 16) - s.get("linked_offset", 0),
                       s.get("linked_size", s["size"])) for s in unit["sections"])
    if sorted((s["name"], s["address"], s["size"]) for s in actual_sections) != expected:
        raise ValueError("Linked source sections differ from manifest")
    # Only the kept slice of a trimmed section is source-built output.
    manifest_sections = {s["name"]: s for s in unit["sections"]}
    actual_sections = [dict(s, address=s["address"] + kept_slice(manifest_sections[s["name"]])[0],
                            offset=s["offset"] + kept_slice(manifest_sections[s["name"]])[0],
                            size=manifest_sections[s["name"]]["size"]) for s in actual_sections]
    if function_records(linked) != manifest_functions(unit):
        raise ValueError("Linked source functions differ from original")
    if any(s["type"] in (4, 9) and s["size"] for s in linked.sections):
        raise ValueError("Linked source still has relocations")
    undefined_symbols = [s for s in linked.symbols() if s["binding"] and not s["section"] and s["name"]]
    # SN drops empty anchor sections but keeps their resolved symbol values.
    # Accept only the reviewed SDA externals, at their pinned original address.
    anchors = set(unit.get("sda_externals", []))
    for symbol in undefined_symbols:
        if symbol["name"] in anchors and (symbol["address"] != int(unit["externals"][symbol["name"]], 16)
                                           or symbol["size"] != 0 or symbol["type"] != 0):
            raise ValueError("Resolved small-data anchor differs")
    undefined = {s["name"] for s in undefined_symbols} - anchors
    if not undefined <= set(unit["undefined_in_discarded_code"]):
        raise ValueError("Unresolved linked source dependency")
    blobs = []
    for section in actual_sections:
        manifest_section = next(s for s in unit["sections"] if s["name"] == section["name"])
        target = next(s for s in original.sections if s["name"] == target_section(manifest_section))
        if section["type"] != target["type"] or section["flags"] != target["flags"]:
            raise ValueError("Linked source section flags or type differ")
        if section["name"].startswith(".gnu.linkonce.d."):
            original_object = check_vtable_storage(original, manifest_section, section["address"], (1, 2))
            obj = check_vtable_storage(linked, manifest_section, section["address"], (original_object["binding"],))
            if obj["section"] != section["index"]:
                raise ValueError("Native vtable linked section differs")
        if section["type"] == 8:
            # Zeroes alone cannot verify BSS ownership. Require the original
            # named objects, sizes and addresses, scoped by source-file symbols.
            # SN .lcomm records retain a size but use STT_NOTYPE, including in
            # the original executable. Zero-size labels never establish storage.
            storage_types = (0, 1) if unit.get("compiler_family", "GC") == "ProDG" else (1,)
            original_objects, file_name, file_index = [], None, None
            for index, symbol in enumerate(original.symbols()):
                if symbol["type"] == 4:
                    file_name = symbol["name"]
                    file_index = index
                if symbol["type"] in storage_types and symbol["size"] and symbol["section"] == target["index"]:
                    if symbol["address"] < section["address"] + section["size"] and symbol["address"] + symbol["size"] > section["address"]:
                        if not section["address"] <= symbol["address"] < symbol["address"] + symbol["size"] <= section["address"] + section["size"]:
                            raise ValueError("BSS range excludes part of an original object")
                        if not symbol["binding"] and file_name != unit.get("original_file"):
                            raise ValueError("BSS range contains another source file's object")
                        if (not symbol["binding"] and "original_file_index" in unit
                                and file_index != unit["original_file_index"]):
                            raise ValueError("BSS range contains another source-file record's object")
                        original_objects.append(object_record(symbol))
            actual_objects = [object_record(s)
                              for s in linked.symbols() if s["type"] in storage_types and s["size"]
                              and s["section"] == section["index"]
                              and section["address"] <= s["address"] < section["address"] + section["size"]]
            if not original_objects or sorted(actual_objects) != sorted(original_objects):
                raise ValueError(f"BSS object ownership differs: {unit['id']} {section['name']}")
        offset = section["address"] - target["address"]
        data = linked.contents(section) if section["type"] != 8 else None
        if section["type"] != 8 and data != original.contents(target)[offset:offset + section["size"]]:
            raise ValueError(f"Compiled source bytes differ: {unit['id']} {section['name']}")
        blobs.append(dict(unit=unit["id"], section=target["name"], source_section=section["name"], address=section["address"],
                          size=section["size"], type=section["type"], data=data, category=unit["category"]))
    return blobs


def write_context(original_path, original, blobs, directory):
    """Include each source blob once and omit its entire original interval."""
    assembly = []
    script = [f"ENTRY(__start)\n__start = {original.entry:#x};\nSECTIONS {{"]
    assertions = []
    for section in original.sections:
        if not section["flags"] & 2:
            continue
        name = section["name"]
        flags = "a" + ("w" if section["flags"] & 1 else "") + ("x" if section["flags"] & 4 else "")
        if section["type"] not in (1, 8):
            raise ValueError("Unsupported original section type")
        nobits = section["type"] == 8
        kind = "nobits" if nobits else "progbits"

        def original_fragment(label, address, size):
            body = f'.space {size}' if nobits else (
                f'.incbin "{original_path}",{section["offset"] + address - section["address"]},{size}')
            assembly.append(f'.section {label},"{flags}",@{kind}\n{body}')
            script.append(f'KEEP(*({label}))')

        fragments = sorted((b for b in blobs if b["section"] == name), key=lambda b: b["address"])
        cursor = section["address"]
        script.append(f'{name} {cursor:#x} {"(NOLOAD) " if nobits else ""}: {{')
        for index, blob in enumerate(fragments):
            if blob["address"] < cursor or blob["address"] + blob["size"] > section["address"] + section["size"]:
                raise ValueError("Invalid source context interval")
            if blob["address"] > cursor:
                label = f'.orig{name}_{index}'
                original_fragment(label, cursor, blob["address"] - cursor)
            source_name = blob.get("source_section", name)
            label = f'.source.{blob["unit"]}{source_name}'
            begin = f'__source_{blob["unit"]}_{source_name[1:]}_start'
            finish = f'__source_{blob["unit"]}_{source_name[1:]}_end'
            if blob.get("type", 1) != section["type"]:
                raise ValueError("Source context section type differs")
            body = f'.space {blob["size"]}' if nobits else f'.incbin "{blob["path"]}"'
            assembly.append(f'.section {label},"{flags}",@{kind}\n{body}')
            script.append(f'{begin} = .; *({label}) {finish} = .;')
            cursor = blob["address"] + blob["size"]
            assertions.extend([f'ASSERT({begin} == {blob["address"]:#x}, "Source start moved")',
                               f'ASSERT({finish} == {cursor:#x}, "Source end moved")'])
        if cursor < section["address"] + section["size"]:
            label = f'.orig{name}_tail'
            original_fragment(label, cursor, section["address"] + section["size"] - cursor)
        if not section["size"]:
            original_fragment(f'.orig{name}_empty', cursor, 0)
        script.append('}')
    (directory / "context.s").write_text("\n".join(assembly) + "\n")
    (directory / "context.ld").write_text("\n".join(script + ["}"] + assertions) + "\n")


def progress(original, units, project):
    sections = {s["name"]: s["size"] for s in original.sections if s["flags"] & 6 == 6}
    total = sum(sections.values())
    if sections != project["progress"]["executable_sections"] or total != project["progress"]["code_bytes"]:
        raise ValueError("Progress denominator differs from the original executable")
    validate_units(original, units)
    categories = {category: sum(f["size"] for u in units if u["category"] == category for f in u["functions"])
                  for category in ("reconstructed_game", "restored_library")}
    matched = sum(categories.values())
    return dict(matching_code_bytes=matched, total_executable_code_bytes=total,
                percent=100 * matched / total, categories=categories,
                matching_functions=sum(len(u["functions"]) for u in units))


def build_project():
    directory = ROOT / "build/reconstruction"
    if directory.exists():
        shutil.rmtree(directory)
    directory.mkdir(parents=True)
    original_path, original = load_target()
    project_path = CONFIG / "project.json"
    project = json.loads(project_path.read_text())
    units = [json.loads((project_path.parent / name).read_text()) for name in project["units"]]
    validate_units(original, units)
    tools = setup()
    compiler_keys = {(u.get("compiler_family", "GC"), u["compiler_version"]) for u in units}
    compiler_keys.add(("ProDG", "3.8.1"))
    compilers = {f"{family}/{version}": setup_compiler(version, family) for family, version in sorted(compiler_keys)}
    blobs, reports = [], []
    for unit in units:
        work = directory / "units" / unit["id"]
        work.mkdir(parents=True)
        family = unit.get("compiler_family", "GC")
        compiler, wrapper = compilers[f"{family}/{unit['compiler_version']}"]
        linker, _ = compilers["ProDG/3.8.1"]
        env = os.environ.copy()
        env["SN_NGC_PATH"] = "Z:" + str(compiler).replace("/", "\\")
        source = ROOT / unit["source"]
        shutil.copyfile(source, work / source.name)

        def execute(stage, *args):
            run(args, work, work / f"{stage}.log", env)

        if family == "GC":
            compile_sdk(unit, compiler, wrapper, work, execute)
        elif family == "ProDG":
            execute("compile", wrapper, compiler / "ngccc.exe", "-v", *unit["compiler_flags"],
                    *[f"-I{ROOT / path}" for path in unit["include_dirs"]],
                    "-S", source.name, "-o", "compiled.s")
            assembler = next(p for p in compiler.iterdir() if p.name.lower() == "ngcas.exe")
            execute("assemble", wrapper, assembler, "compiled.s", "-o", "compiled.o")
        else:
            raise ValueError("Unsupported compiler family")
        obj = Elf32((work / "compiled.o").read_bytes())
        validate_object(obj, unit)
        use_small_data = family == "GC" or "sda_externals" in unit
        if use_small_data:
            script = sdk_link_script(unit, obj, original, tools, work, execute)
        else:
            script = "SECTIONS {\n" + "".join(f"{s['name']} {s['address']} : {{ *({s['name']}) }}\n" for s in unit["sections"])
            script += "".join(f"{name} = {address};\n" for name, address in unit["externals"].items()) + "}\n"
        (work / "source.ld").write_text(script)
        strip_args = ["--strip-unused"] + [arg for f in unit["functions"] if f["binding"]
                                            for arg in ("--undefined", f["name"])] if unit["strip_unused"] else []
        if unit.get("retain_function_symbols", False):
            # Preserve local entry points and their verification symbols without
            # creating unresolved global symbols through --undefined.
            (work / "functions.keep").write_text("".join(f["name"] + "\n" for f in unit["functions"]))
            strip_args += ["--retain-symbols-file", "functions.keep"]
        execute("link", wrapper, linker / "ngcld.exe", *strip_args,
                *(["--fix-sda"] if use_small_data else []), "-T", "source.ld",
                "-o", "compiled.elf", *([] if use_small_data else ["compiled.o"]))
        generated = verify_unit(original, Elf32((work / "compiled.elf").read_bytes()), unit)
        for blob in generated:
            data = blob.pop("data")
            if data is not None:
                blob["path"] = str(work / (blob["source_section"][1:] + ".bin"))
                Path(blob["path"]).write_bytes(data)
        blobs.extend(generated)
        reports.append(dict(id=unit["id"], category=unit["category"],
                            functions=len(unit["functions"]), code_bytes=sum(f["size"] for f in unit["functions"]),
                            compiler_family=family, compiler_version=unit["compiler_version"], compiler_flags=unit["compiler_flags"],
                            strip_unused=unit["strip_unused"], discarded_functions=unit["discarded_functions"],
                            object_sha256=sha256(work / "compiled.o"),
                            linked_sha256=sha256(work / "compiled.elf")))
        print(f"Verified {unit['id']}: {reports[-1]['code_bytes']} code bytes", flush=True)
    write_context(original_path, original, blobs, directory)
    for stage, args in [
        ("context-assemble", [tools / "powerpc-eabi-as", "context.s", "-o", "context.o"]),
        ("context-link", [tools / "powerpc-eabi-ld", "-T", "context.ld", "-o", "rebuilt.elf", "context.o"]),
        ("convert", [tools / "dtk", "elf2dol", "rebuilt.elf", "rebuilt.dol"]),
        ("reference", [tools / "dtk", "elf2dol", original_path, "reference.dol"]),
    ]:
        run(args, directory, directory / f"{stage}.log")
    expected = (directory / "reference.dol").read_bytes()
    baseline = json.loads((project_path.parent / "baseline.json").read_text())
    if len(expected) != baseline["normalized_dol_size"] or hashlib.sha1(expected).hexdigest() != baseline["normalized_dol_sha1"]:
        raise ValueError("Reference conversion differs from its pinned identity")
    actual = (directory / "rebuilt.dol").read_bytes()
    if actual != expected:
        raise ValueError("Complete reconstructed image differs from original")
    verify_load_image(original, actual)
    # Fingerprint all source, headers, manifests, and build tools, including any
    # unused headers. These are provenance, not substitutes for byte verification.
    inputs = [p for folder in ("src", "include", "config", "tools")
              for p in (ROOT / folder).rglob("*") if p.is_file() and "__pycache__" not in p.parts]
    report = dict(status="identical", target=original_path.name, original_elf_sha256=sha256(original_path),
                  comparison="complete ELF-derived DOL and every allocated ELF byte, entry point and BSS extent",
                  progress=progress(original, units, project), units=reports,
                  source_data_bytes=sum(b["size"] for b in blobs if b["section"] != ".text" and b["type"] != 8),
                  source_bss_bytes=sum(b["size"] for b in blobs if b["type"] == 8),
                  source_blobs=[dict(b, sha256=sha256(Path(b["path"]))) if "path" in b else b for b in blobs],
                  input_sha256={str(p.relative_to(ROOT)): sha256(p) for p in sorted(inputs)},
                  compiler_executables={version: {p.name: sha256(p) for p in compiler.glob("*.exe")}
                                        for version, (compiler, _) in compilers.items()},
                  dol_size=len(actual), dol_sha1=hashlib.sha1(actual).hexdigest(),
                  compiler_release_proven=False, runtime_tested=False)
    (directory / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    result = report["progress"]
    print(f"Complete image identical. Matching code: {result['matching_code_bytes']:,} / "
          f"{result['total_executable_code_bytes']:,} bytes ({result['percent']:.6f}%).")


if __name__ == "__main__":
    build_project()
