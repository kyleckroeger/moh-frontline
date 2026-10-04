"""Adapt dtk's ELF-derived configuration for splitting the ELF-derived DOL.

`dtk elf config` reads the original symbols and relocations. Its per-file
ranges are not yet trustworthy for this target: globals with no file-local
anchor are attributed to the last file record (sdfx.c), which makes the link
order cyclic. The baseline therefore splits one unit spanning every allocated
section, using the original symbols only to guide analysis. See
docs/initial-audit.md.
"""

# dtk recreates these from the DOL's .ctors/.dtors and rejects duplicates.
DTK_GENERATED = ("_ctors", "_dtors")


def normalize_symbols(text):
    lines = [line for line in text.splitlines()
             if line.split(" = ", 1)[0] not in DTK_GENERATED]
    return "\n".join(lines) + "\n"


def whole_image_splits(text, elf, unit):
    """Keep dtk's section table; replace its file ranges with one complete unit."""
    header = text.split("\n\n", 1)[0]
    if not header.startswith("Sections:"):
        raise ValueError("Unexpected dtk splits format")
    ranges = "".join(f"\t{s['name']:<11} start:{s['address']:#010x} end:{s['address'] + s['size']:#010x}\n"
                     for s in elf.sections if s["flags"] & 2 and s["size"])
    return f"{header}\n\n{unit}:\n{ranges}"
