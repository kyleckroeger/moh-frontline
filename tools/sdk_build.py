"""CodeWarrior compilation and shared SN linking with typed small-data externals."""
import struct
from functools import lru_cache

from formats import Elf32
from setup import ROOT


@lru_cache(maxsize=4)
def external_symbol_index(original):
    """Index the immutable parsed target once for all source-unit dependencies."""
    file_name, definitions = None, {}
    for symbol in original.symbols():
        if symbol["type"] == 4:
            file_name = symbol["name"]
        if symbol["section"]:
            key = (symbol["name"], symbol["binding"], file_name if not symbol["binding"] else None)
            definitions.setdefault(key, []).append(symbol)
    return definitions


def resolve_external(original, unit, name):
    """Resolve a context dependency, including explicitly scoped private symbols."""
    local = name in unit.get("local_externals", [])
    key = (name, 0 if local else 1, unit.get("original_file") if local else None)
    matches = external_symbol_index(original).get(key, [])
    if len(matches) != 1 or matches[0]["address"] != int(unit["externals"][name], 16):
        raise ValueError(f"External is not an original definition: {name}")
    return matches[0]


def compile_sdk(unit, compiler, wrapper, work, execute):
    execute("compile", wrapper, compiler / "mwcceppc.exe", "-c",
            *unit["compiler_flags"], "-I-",
            *[f"-I{ROOT / path}" for path in unit["include_dirs"]],
            "-ir", ROOT / "src/dolphin", ROOT / unit["source"], "-o", "compiled.o")


def sdk_link_script(unit, obj, original, tools, work, execute):
    """Zero-size anchors supply ELF section identity, never code or data bytes.

    SN's --fix-sda selects r2/r13 from the final address. SDA21
    relocations also require a section-relative definition, not SHN_ABS.
    Explicit input selectors keep anchors out of compiled data sections.
    """
    symbols = list(obj.symbols())
    small = set()
    for section in obj.sections:
        if section["type"] != 4 or not obj.sections[section["info"]]["flags"] & 2:
            continue
        for offset in range(0, section["size"], 12):
            _, info, _ = struct.unpack_from(">IIi", obj.contents(section), offset)
            symbol = symbols[info >> 8]
            if info & 255 == 109 and symbol["section"] == 0:
                small.add(symbol["name"])
    script = ["SECTIONS {"]
    for section in unit["sections"]:
        name = section["name"]
        script.append(f"{name} {section['address']} : {{ compiled.o({name}) }}")
    originals = list(original.symbols())
    for index, (name, address) in enumerate(unit["externals"].items()):
        if name not in small:
            script.append(f"{name} = {address};")
            continue
        target = resolve_external(original, unit, name)
        section = original.sections[target["section"]]
        flags = "a" + ("w" if section["flags"] & 1 else "")
        kind = "nobits" if section["type"] == 8 else "progbits"
        anchor = f"anchor{index}"
        (work / f"{anchor}.s").write_text(
            f'.section {section["name"]},"{flags}",@{kind}\n.global {name}\n{name}:\n')
        execute(anchor, tools / "powerpc-eabi-as", f"{anchor}.s", "-o", f"{anchor}.o")
        anchor_obj = Elf32((work / f"{anchor}.o").read_bytes())
        if any(s["size"] for s in anchor_obj.sections if s["flags"] & 2):
            raise ValueError("External anchor unexpectedly contains bytes")
        script.append(f"{section['name']}.{anchor} {address} : {{ {anchor}.o({section['name']}) }}")
    for name in ("_SDA_BASE_", "_SDA2_BASE_"):
        value = next(s["address"] for s in originals if s["name"] == name)
        script.append(f"{name} = {value:#x};")
    return "\n".join(script + ["}"]) + "\n"
