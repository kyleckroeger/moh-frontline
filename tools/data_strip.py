"""Emulate MW's per-object data dead-stripping on a compiled object.

SN's linker cannot drop single objects from a section. Each object a manifest
section names in `stripped_objects` is moved, with its trailing padding, to the
end of its compiler section. The existing trimmed-tail checks then keep it out
of the compared and credited bytes and reject retained references into it.

Relocations located in bytes the original linker removed (parked sections,
trimmed slices and stripped objects) are pointed at their own section, so that
dead data does not keep discarded functions alive in SN's link.

The rewrite permutes bytes and keeps the file size; the original compiler
output is still what the build report hashes.
"""
import re
import struct

from formats import Elf32

STT_OBJECT, STT_SECTION = 1, 3
R_PPC_ADDR32 = 1


def c_identifier(name):
    """MW numbers function statics (`name$123`); manifests use the C name."""
    match = re.fullmatch(r"([A-Za-z_]\w*)[$.]\d+", name)
    return match.group(1) if match else name


def section_chunks(obj, section, names):
    """Split a section at object starts; return (chunks, stripped flags)."""
    objects = sorted((s["address"], s["size"], c_identifier(s["name"])) for s in obj.symbols()
                     if s["section"] == section["index"] and s["type"] == STT_OBJECT and s["size"])
    starts = sorted({a for a, _, _ in objects} | {0})
    bounds = list(zip(starts, starts[1:] + [section["size"]]))
    stripped = [False] * len(bounds)
    for name in names:
        found = [o for o in objects if o[2] == name]
        if len(found) != 1:
            raise ValueError(f"Stripped object is not one compiler object: {name}")
        address, size, _ = found[0]
        if sum(1 for o in objects if o[0] == address) != 1:
            raise ValueError(f"Stripped object shares its address: {name}")
        index = starts.index(address)
        if address + size > bounds[index][1]:
            raise ValueError(f"Stripped object overlaps the next object: {name}")
        stripped[index] = True
    return bounds, stripped


def offset_map(bounds, stripped, size):
    """Kept chunks keep their order; stripped chunks follow them at the end."""
    order = [i for i, s in enumerate(stripped) if not s] + [i for i, s in enumerate(stripped) if s]
    new_start, cursor = {}, 0
    for i in order:
        new_start[i] = cursor
        cursor += bounds[i][1] - bounds[i][0]

    def remap(offset):
        if offset == size:
            return size
        for i, (start, end) in enumerate(bounds):
            if start <= offset < end:
                return new_start[i] + offset - start
        raise ValueError("Offset lies outside its section")
    return order, remap


def strip_objects(data, unit):
    """Return a rewritten object for linking, or the input when nothing applies."""
    obj = Elf32(data)
    out = bytearray(data)
    by_name = {s["name"]: s for s in obj.sections}
    remaps, dead = {}, {}  # section index -> remap function / dead [start, end) ranges
    for manifest in unit["sections"]:
        names = manifest.get("stripped_objects", [])
        section = by_name[manifest["name"]]
        if names:
            if not unit["strip_unused"] or manifest["name"] in (".text", ".init"):
                raise ValueError("Object stripping requires native unused-code stripping of data")
            bounds, stripped = section_chunks(obj, section, names)
            order, remap = offset_map(bounds, stripped, section["size"])
            remaps[section["index"]] = remap
            if section["type"] != 8:
                old = obj.contents(section)
                out[section["offset"]:section["offset"] + section["size"]] = b"".join(
                    old[bounds[i][0]:bounds[i][1]] for i in order)
        start = manifest.get("linked_offset", 0)
        if manifest["name"] not in (".text", ".init"):
            dead[section["index"]] = [(0, start), (start + manifest["size"], section["size"])]
    for name in unit.get("stripped_sections", []):
        dead[by_name[name]["index"]] = [(0, by_name[name]["size"])]

    symbol_table = next(s for s in obj.sections if s["type"] == 2)
    for i, symbol in enumerate(obj.symbols()):
        remap = remaps.get(symbol["section"])
        if remap and symbol["type"] != STT_SECTION:
            struct.pack_into(">I", out, symbol_table["offset"] + i * 16 + 4, remap(symbol["address"]))
    symbols = list(obj.symbols())
    section_symbol = {s["section"]: i for i, s in enumerate(symbols) if s["type"] == STT_SECTION}
    for rela in obj.sections:
        if rela["type"] != 4:
            continue
        target = rela["info"]
        for entry in range(rela["offset"], rela["offset"] + rela["size"], 12):
            location, info, addend = struct.unpack_from(">IIi", data, entry)
            symbol = symbols[info >> 8]
            if target in remaps:
                location = remaps[target](location)
            if symbol["type"] == STT_SECTION and symbol["section"] in remaps:
                addend = remaps[symbol["section"]](addend)
            if any(a <= location < b for a, b in dead.get(target, [])):
                # The original link removed these bytes. SN rejects R_PPC_NONE,
                # so refer to the dead section itself instead of a live symbol.
                if info & 255 != R_PPC_ADDR32:
                    raise ValueError("Unsupported relocation in stripped data")
                info, addend = section_symbol[target] << 8 | R_PPC_ADDR32, 0
            struct.pack_into(">IIi", out, entry, location, info, addend)
    return bytes(out)
