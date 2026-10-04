#!/usr/bin/env python3
"""Extract only the pinned executables; never modify or copy the disc image."""
import argparse
import hashlib
import json
import struct
from pathlib import Path

from formats import cstring, dol_sections, span
from setup import CONFIG, GAME_ID, ORIG


def read_at(stream, offset, size):
    if offset < 0 or size < 0 or offset + size > stream.seek(0, 2):
        raise ValueError("Disc range extends beyond image")
    stream.seek(offset)
    data = stream.read(size)
    if len(data) != size:
        raise ValueError("Truncated disc read")
    return data


def import_disc(image):
    target = json.loads((CONFIG / "target.json").read_text())
    with image.open("rb") as stream:
        header = read_at(stream, 0, 0x440)
        if header[:6].decode("ascii") != target["disc_id"] or header[6:8] != bytes((target["disc_number"], target["revision"])):
            raise ValueError(f"Expected {GAME_ID} USA revision 0")
        if struct.unpack_from(">I", header, 0x1C)[0] != 0xC2339F3D:
            raise ValueError("Not a GameCube image")
        fst_offset, fst_size = struct.unpack_from(">II", header, 0x424)
        if not 12 <= fst_size <= 16 * 1024 * 1024:
            raise ValueError("Invalid filesystem table size")
        fst = read_at(stream, fst_offset, fst_size)
        count = struct.unpack_from(">I", fst, 8)[0]
        if not count or count * 12 > len(fst):
            raise ValueError("Invalid filesystem entry count")
        names = fst[count * 12:]
        directories = [("", count)]
        files = {}
        for index in range(1, count):
            while directories and index >= directories[-1][1]:
                directories.pop()
            if not directories:
                raise ValueError("Invalid filesystem nesting")
            word, offset, size = struct.unpack_from(">III", fst, index * 12)
            name = cstring(names, word & 0xFFFFFF)
            if not name or name in (".", "..") or "/" in name or "\\" in name:
                raise ValueError("Invalid filesystem entry name")
            path = directories[-1][0] + name
            if word >> 24:
                if word >> 24 != 1 or not index < size <= directories[-1][1]:
                    raise ValueError("Invalid directory extent")
                directories.append((path + "/", size))
            else:
                if path in files:
                    raise ValueError("Duplicate disc path")
                files[path] = (offset, size)
        dol_offset = struct.unpack_from(">I", header, 0x420)[0]
        dol_header = read_at(stream, dol_offset, 0x100)
        dol_size = max([0x100] + [offset + size for _, offset, size in dol_sections(dol_header)])
        files["boot.dol"] = (dol_offset, dol_size)
        selected = {}
        for name, expected in target["files"].items():
            if name not in files or files[name][1] != expected["size"]:
                raise ValueError(f"Missing or incorrect executable: {name}")
            data = read_at(stream, *files[name])
            for algorithm in ("sha1", "sha256"):
                if hashlib.new(algorithm, data).hexdigest() != expected[algorithm]:
                    raise ValueError(f"{name} does not match the pinned target")
            selected[name] = data
    output = ORIG
    output.mkdir(parents=True, exist_ok=True)
    # Validate the entire extraction before writing anything.
    for name, data in selected.items():
        path = output / name
        if path.exists() and path.read_bytes() != data:
            raise ValueError(f"Refusing to overwrite a differing original: {name}")
    for name, data in selected.items():
        (output / name).write_bytes(data)
    print(f"Verified and extracted {len(selected)} executables into orig/{GAME_ID}")
    print("Executable hashes verified; full retail-disc recovery/checksum is not established.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("image", type=Path)
    import_disc(parser.parse_args().image)
