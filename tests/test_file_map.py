"""The recovered file map must agree with every verified unit (local only)."""
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from setup import CONFIG, ORIG


class FileMap(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if not (ORIG / "Moh2RelGC.elf").exists():
            raise unittest.SkipTest("Import the game first")
        from audit import load_target
        from file_map import build_file_map
        _, elf = load_target()
        cls.files, cls.items, _, _ = build_file_map(elf)
        from file_map import verified_units
        cls.units = verified_units()
        cls.with_units = build_file_map(elf, cls.units)
        cls.by_function = {(e["name"], e["address"]): e for entries in cls.items.values()
                           for e in entries if e["size"]}

    def test_strong_rules_match_verified_units(self):
        project = json.loads((CONFIG / "project.json").read_text())
        checked = 0
        for name in project["units"]:
            unit = json.loads((CONFIG / name).read_text())
            if "original_file" not in unit:
                continue  # A file with only global symbols has no record to compare.
            for function in unit["functions"]:
                entry = self.by_function[(function["name"], int(function["address"], 16))]
                if entry["owner"] is None or "inferred" in entry["evidence"]:
                    continue
                checked += 1
                self.assertEqual(self.files[entry["owner"]], unit["original_file"],
                                 f"{function['name']} placed by {entry['evidence']}")
        self.assertGreater(checked, 0)

    def test_verified_units_settle_their_own_items(self):
        files, items, _, unresolved = self.with_units
        by_function = {(e["name"], e["address"]): e for entries in items.values() for e in entries if e["size"]}
        for unit in self.units:
            for function in unit["functions"]:
                entry = by_function[(function["name"], int(function["address"], 16))]
                if "original_file" in unit:
                    self.assertEqual(files[entry["owner"]], unit["original_file"], function["name"])
                else:
                    self.assertIsNone(entry["owner"], function["name"])
                    self.assertIsNotNone(entry["evidence"], function["name"])
        self.assertLess(len(unresolved), len(build_unresolved(self)))

    def test_unit_contradicting_a_local_symbol_fails(self):
        from audit import load_target
        from file_map import build_file_map
        unit = json.loads(json.dumps(next(u for u in self.units if u.get("original_file") == "printf.c")))
        other = next(u for u in self.units if u.get("original_file") == "OSRtc.c")
        unit["original_file"], unit["original_file_index"] = other["original_file"], other["original_file_index"]
        with self.assertRaisesRegex(ValueError, "contradicts"):
            build_file_map(load_target()[1], [unit])


def build_unresolved(case):
    from audit import load_target
    from file_map import build_file_map
    return build_file_map(load_target()[1])[3]


if __name__ == "__main__":
    unittest.main()
