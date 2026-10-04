"""Bounded readers for the GameCube disc, DOL and ELF32 formats used here."""
import struct


def span(data, offset, size):
    if offset < 0 or size < 0 or offset + size > len(data):
        raise ValueError("Binary range extends beyond input")
    return data[offset:offset + size]


def cstring(data, offset):
    if not 0 <= offset < len(data):
        raise ValueError("String offset extends beyond table")
    end = data.find(b"\0", offset)
    if end < 0:
        raise ValueError("Unterminated string")
    return data[offset:end].decode("utf-8", errors="replace")


def dol_sections(data):
    span(data, 0, 0x100)
    offsets = struct.unpack_from(">18I", data, 0)
    addresses = struct.unpack_from(">18I", data, 0x48)
    sizes = struct.unpack_from(">18I", data, 0x90)
    return [(address, offset, size) for address, offset, size
            in zip(addresses, offsets, sizes) if size]


class Elf32:
    def __init__(self, data):
        span(data, 0, 52)
        if data[:7] != b"\x7fELF\x01\x02\x01":
            raise ValueError("Expected a big-endian ELF32 file")
        fields = struct.unpack_from(">HHIIIIIHHHHHH", data, 16)
        if fields[1] != 20 or fields[2] != 1:
            raise ValueError("Expected a PowerPC ELF")
        self.data = data
        self.kind, self.entry = fields[0], fields[3]
        shoff, shsize, count, names_index = fields[5], fields[10], fields[11], fields[12]
        if shsize != 40 or not 0 < names_index < count:
            raise ValueError("Unsupported section table")
        span(data, shoff, shsize * count)
        raw = [struct.unpack_from(">10I", data, shoff + i * shsize) for i in range(count)]
        names = span(data, raw[names_index][4], raw[names_index][5])
        self.sections = []
        for i, section in enumerate(raw):
            name, kind, flags, address, offset, size, link, info, align, entsize = section
            if kind != 8:
                span(data, offset, size)
            self.sections.append(dict(index=i, name=cstring(names, name), type=kind,
                                      flags=flags, address=address, offset=offset,
                                      size=size, link=link, info=info,
                                      alignment=align, entry_size=entsize))

    def contents(self, section):
        if section["type"] == 8:
            raise ValueError("NOBITS section has no file contents")
        return span(self.data, section["offset"], section["size"])

    def symbols(self):
        for section in self.sections:
            if section["type"] != 2:
                continue
            if section["entry_size"] != 16 or section["size"] % 16:
                raise ValueError("Invalid ELF32 symbol table")
            if not 0 <= section["link"] < len(self.sections):
                raise ValueError("Invalid ELF symbol string table")
            strings = self.contents(self.sections[section["link"]])
            symbols = self.contents(section)
            for offset in range(0, len(symbols), 16):
                name, value, size, info, other, index = struct.unpack_from(">IIIBBH", symbols, offset)
                yield dict(name=cstring(strings, name), address=value, size=size,
                           type=info & 15, binding=info >> 4, section=index)


def verify_load_image(elf, dol):
    """Check every original allocated file byte and every original BSS extent."""
    sections = dol_sections(dol)
    for _, offset, size in sections:
        span(dol, offset, size)
    entry = struct.unpack_from(">I", dol, 0xE0)[0]
    bss_start, bss_size = struct.unpack_from(">II", dol, 0xD8)
    if entry != elf.entry:
        raise ValueError("Entry point changed")
    for section in elf.sections:
        if not section["flags"] & 2:
            continue
        start, size = section["address"], section["size"]
        if section["type"] == 8:
            if not bss_start <= start <= start + size <= bss_start + bss_size:
                raise ValueError(f"BSS range lost: {section['name']}")
        elif size:
            matches = [(address, offset) for address, offset, length in sections
                       if address <= start and start + size <= address + length]
            if len(matches) != 1:
                raise ValueError(f"Loaded section missing or ambiguous: {section['name']}")
            address, offset = matches[0]
            if span(dol, offset + start - address, size) != elf.contents(section):
                raise ValueError(f"Loaded bytes changed: {section['name']}")
