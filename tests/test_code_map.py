"""Mapping must preserve uncertainty and byte coverage without original files."""
import copy
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from code_map import build_code_map, validate_code_map


def symbol(name, address=0, size=0, kind=2, binding=1, section=1):
    return dict(name=name, address=address, size=size, type=kind, binding=binding, section=section)


def file(name):
    return symbol(name, kind=4, binding=0, section=0xfff1)


def marker(address):
    return symbol("gcc2_compiled.", address, kind=0, binding=0)


class FakeElf:
    def __init__(self, symbols, size=96):
        self.records = symbols
        self.sections = [dict(index=1, name=".text", address=0x1000, size=size, flags=6)]

    def symbols(self):
        return iter(self.records)


class CodeMapping(unittest.TestCase):
    def example(self):
        # Globals follow all file/local records, as they do in the pinned ELF.
        return FakeElf([file("a.cpp"), marker(0x1000), symbol("a_local", 0x1004, 4, binding=0),
                        file("b.cpp"), marker(0x1020), symbol("b_local", 0x1020, 4, binding=0),
                        file("c.cpp"), marker(0x1040), symbol("c_local", 0x1040, 4, binding=0),
                        symbol("a_global", 0x1000, 4), symbol("b_global", 0x1024, 4),
                        symbol("unknown_global", 0x1044, 4)])

    def test_local_scope_and_global_marker_inference_stay_distinct(self):
        mapped = build_code_map(self.example(), [(0x1004, 0x1008)])
        a = next(u for u in mapped["units"] if u.get("file_name") == "a.cpp")
        self.assertEqual([r["name"] for r in a["ranges"]], ["a_global"])
        self.assertEqual(a["ranges"][0]["evidence"], "adjacent-compiler-markers")
        b = next(u for u in mapped["units"] if u.get("file_name") == "b.cpp")
        self.assertEqual({r["evidence"] for r in b["ranges"]}, {"local-file-symbol", "adjacent-compiler-markers"})
        c = next(u for u in mapped["units"] if u.get("file_name") == "c.cpp")
        self.assertEqual([r["name"] for r in c["ranges"]], ["c_local"])
        self.assertTrue(any(u["name"].startswith("Unknown file/") and
                            u["ranges"][0]["name"] == "unknown_global" for u in mapped["units"]))

    def test_intervening_file_without_marker_blocks_inference(self):
        elf = FakeElf([file("a.cpp"), marker(0x1000), file("unmarked.cpp"),
                       file("b.cpp"), marker(0x1040), symbol("ambiguous", 0x1008, 4)])
        mapped = build_code_map(elf, [])
        self.assertTrue(mapped["units"][0]["name"].startswith("Unknown file/"))

    def test_foreign_local_function_blocks_interval_inference(self):
        elf = FakeElf([file("a.cpp"), marker(0x1000), file("b.cpp"), marker(0x1040),
                       symbol("b_local_before_marker", 0x1010, 4, binding=0),
                       symbol("ambiguous_global", 0x1000, 4)])
        mapped = build_code_map(elf, [])
        unit = next(u for u in mapped["units"] if u["ranges"][0]["name"] == "ambiguous_global")
        self.assertTrue(unit["name"].startswith("Unknown file/"))

    def test_shared_entry_points_earn_only_union_size(self):
        elf = FakeElf([symbol("save_14", 0x1000, 12), symbol("save_15", 0x1004, 8),
                       symbol("save_16", 0x1008, 4)], size=16)
        mapped = build_code_map(elf, [])
        shared = mapped["units"][0]["ranges"][0]
        self.assertEqual(shared["size"], 12)
        self.assertEqual(len(shared["symbols"]), 3)
        self.assertEqual(sum(r["size"] for u in mapped["units"] for r in u["ranges"]), 16)
        with self.assertRaisesRegex(ValueError, "cuts through"):
            build_code_map(elf, [(0x1000, 0x1004)])

    def test_symbol_outside_section_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "beyond"):
            build_code_map(FakeElf([symbol("bad", 0x105c, 8)]), [])

    def test_conflicting_marker_intervals_do_not_assign_globals(self):
        elf = FakeElf([file("a.cpp"), marker(0x1000), file("b.cpp"), marker(0x1040),
                       file("c.cpp"), marker(0x1020), file("d.cpp"), marker(0x1060),
                       symbol("conflict", 0x1028, 4)], size=128)
        mapped = build_code_map(elf, [])
        self.assertTrue(mapped["units"][0]["name"].startswith("Unknown file/"))

    def test_shared_symbol_evidence_cannot_extend_beyond_union(self):
        mapped = build_code_map(FakeElf([symbol("one", 0x1000, 12), symbol("two", 0x1004, 8)]), [])
        mapped["units"][0]["ranges"][0]["symbols"][1]["size"] = 16
        with self.assertRaisesRegex(ValueError, "shared entry-point"):
            validate_code_map(mapped, [], mapped["sections"])

    def test_historical_workstation_paths_are_not_published(self):
        mapped = build_code_map(FakeElf([file("D:/Users/someone/Temp/source.cpp"),
                                        symbol("local", 0x1000, 4, binding=0)]), [])
        self.assertEqual(mapped["units"][0]["file_name"], "source.cpp")
        self.assertNotIn("Users", str(mapped))

    def test_duplicate_file_names_keep_separate_groups(self):
        mapped = build_code_map(FakeElf([file("same.cpp"), symbol("first", 0x1000, 4, binding=0),
                                        file("same.cpp"), symbol("second", 0x1004, 4, binding=0)]), [])
        groups = [u for u in mapped["units"] if u.get("file_name") == "same.cpp"]
        self.assertEqual(len(groups), 2)
        self.assertNotEqual(groups[0]["name"], groups[1]["name"])

    def test_map_validator_rejects_duplicate_or_lost_ranges(self):
        valid = build_code_map(self.example(), [])
        for mutation in ("duplicate", "missing", "outside"):
            with self.subTest(mutation=mutation):
                mapped = copy.deepcopy(valid)
                ranges = mapped["units"][0]["ranges"]
                if mutation == "duplicate":
                    ranges.append(copy.deepcopy(ranges[0]))
                elif mutation == "missing":
                    mapped["units"].pop(0)
                else:
                    ranges[0]["address"] = "0xff0"
                with self.assertRaisesRegex(ValueError, "coverage"):
                    validate_code_map(mapped, [], valid["sections"])


if __name__ == "__main__":
    unittest.main()
