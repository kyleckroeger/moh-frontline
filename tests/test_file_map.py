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
        cls.by_function = {(e["name"], e["address"]): e for entries in cls.items.values()
                           for e in entries if e["size"]}

    def test_strong_rules_match_verified_units(self):
        project = json.loads((CONFIG / "project.json").read_text())
        checked = 0
        for name in project["units"]:
            unit = json.loads((CONFIG / name).read_text())
            for function in unit["functions"]:
                entry = self.by_function[(function["name"], int(function["address"], 16))]
                if entry["owner"] is None or "inferred" in entry["evidence"]:
                    continue
                checked += 1
                self.assertEqual(self.files[entry["owner"]], unit["original_file"],
                                 f"{function['name']} placed by {entry['evidence']}")
        self.assertGreater(checked, 0)


if __name__ == "__main__":
    unittest.main()
