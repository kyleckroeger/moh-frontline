"""Mutation checks against the local target: damaged images must not pass."""
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from audit import load_target
from setup import ORIG
from formats import Elf32, dol_sections, verify_load_image


class TargetVerification(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        original = ORIG / "Moh2RelGC.elf"
        reference = ROOT / "build/baseline/reference.dol"
        if not original.exists() or not reference.exists():
            raise unittest.SkipTest("Import the game and run baseline.py first")
        _, cls.elf = load_target()
        cls.reference = reference.read_bytes()

    def test_verified_load_image(self):
        verify_load_image(self.elf, self.reference)

    def test_changed_instruction_fails(self):
        damaged = bytearray(self.reference)
        text = next(s for s in self.elf.sections if s["name"] == ".text")
        start, offset, _ = next(s for s in dol_sections(damaged) if s[0] == text["address"])
        damaged[offset + text["size"] - 1] ^= 1
        with self.assertRaisesRegex(ValueError, "Loaded bytes changed"):
            verify_load_image(self.elf, damaged)

    def test_changed_entry_point_fails(self):
        damaged = bytearray(self.reference)
        struct.pack_into(">I", damaged, 0xE0, self.elf.entry + 4)
        with self.assertRaisesRegex(ValueError, "Entry point changed"):
            verify_load_image(self.elf, damaged)

    def test_truncated_bss_extent_fails(self):
        damaged = bytearray(self.reference)
        start = struct.unpack_from(">I", damaged, 0xD8)[0]
        last = max((s for s in self.elf.sections if s["type"] == 8), key=lambda s: s["address"])
        struct.pack_into(">I", damaged, 0xDC, last["address"] + last["size"] - 4 - start)
        with self.assertRaisesRegex(ValueError, "BSS range lost"):
            verify_load_image(self.elf, damaged)

    def test_truncated_dol_fails(self):
        with self.assertRaises(ValueError):
            verify_load_image(self.elf, self.reference[:-32])

    def test_truncated_elf_fails(self):
        with self.assertRaises(ValueError):
            Elf32(self.elf.data[:-1])

    def test_wrong_architecture_fails(self):
        damaged = bytearray(self.elf.data)
        struct.pack_into(">H", damaged, 18, 8)
        with self.assertRaisesRegex(ValueError, "PowerPC"):
            Elf32(damaged)


if __name__ == "__main__":
    unittest.main()
