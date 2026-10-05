"""Checks of the objdiff config generator's pure range logic."""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from objdiff_config import split_ranges, splits_text, unit_ranges


def unit(uid, *sections):
    return {"id": uid, "sections": [
        {"name": name, "address": hex(address), "size": size}
        for name, address, size in sections]}


class ObjdiffRanges(unittest.TestCase):
    def test_range_maps_to_target_section_and_end(self):
        self.assertEqual(unit_ranges(unit("X", (".text", 0x80001000, 16))),
                         [(".text", 0x80001000, 0x80001010)])

    def test_word_end_rounds_up_when_a_gap_follows(self):
        units = [unit("A", (".data", 0x80000000, 6)),
                 unit("B", (".data", 0x80000010, 4))]
        ranges = split_ranges(units, {".data": (0x80000000, 0x80000020)})
        self.assertEqual(ranges["A"], [(".data", 0x80000000, 0x80000008)])
        self.assertEqual(ranges["B"], [(".data", 0x80000010, 0x80000014)])

    def test_small_gap_is_absorbed_by_meeting_the_next_unit(self):
        units = [unit("A", (".data", 0x80000000, 6)),
                 unit("B", (".data", 0x80000007, 4))]
        ranges = split_ranges(units, {".data": (0x80000000, 0x80000010)})
        self.assertEqual(ranges["A"], [(".data", 0x80000000, 0x80000007)])

    def test_splits_text_lists_every_unit_sorted(self):
        units = [unit("A", (".text", 0x80001000, 4)),
                 unit("B", (".text", 0x80001010, 8))]
        ranges = split_ranges(units, {".text": (0x80001000, 0x80001020)})
        text = splits_text("Sections:\n\t.text type:code align:16", units, ranges)
        self.assertIn("A:\n\t.text       start:0x80001000 end:0x80001004", text)
        self.assertLess(text.index("A:"), text.index("B:"))


if __name__ == "__main__":
    unittest.main()
