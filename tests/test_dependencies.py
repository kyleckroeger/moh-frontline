"""Protect the research ranking from branch-decoding and ownership mistakes."""
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from dependencies import branch, dependency_graph


class FakeElf:
    def __init__(self, functions, words, base=0x1000, size=0x80):
        self.records = [dict(name=n, address=a, size=s, type=2, binding=1, section=1)
                        for n, a, s in functions]
        self.sections = [dict(index=1, address=base, size=size, flags=6)]
        self.data = bytearray(size)
        for address, word in words.items():
            struct.pack_into('>I', self.data, address - base, word)

    def symbols(self):
        return iter(self.records)

    def contents(self, section):
        return self.data


class Dependencies(unittest.TestCase):
    def test_signed_relative_absolute_and_conditional_branches(self):
        self.assertEqual(branch(0x4bfffff1, 0x1020)['target'], 0x1010)  # bl -16
        self.assertEqual(branch(0x48001003, 0x80000000)['target'], 0x1000)  # bla
        self.assertEqual(branch(0x4bfffff3, 0x1000)['target'], 0xfffffff0)  # negative absolute
        result = branch(0x4182fff1, 0x1020)  # beql -16
        self.assertEqual(result, dict(target=0x1010, linked=True, conditional=True))
        self.assertFalse(branch(0x42800009, 0x1000)['conditional'])  # bcl 20,0,+8
        self.assertEqual(branch(0x48000021, 0xfffffff0)['target'], 0x10)  # address wrap
        self.assertIsNone(branch(0x60000000, 0x1000))  # nop

    def test_indirect_calls_are_not_guessed_and_returns_are_ignored(self):
        for word in (0x4e800421, 0x4e800021):  # bctrl / blrl
            self.assertIsNone(branch(word, 0x1000)['target'])
        for word in (0x4e800420, 0x4e800020):  # bctr / blr
            self.assertIsNone(branch(word, 0x1000))

    def test_multiple_calls_count_one_caller_and_separate_accepted_callers(self):
        elf = FakeElf([('game', 0x1000, 12), ('accepted', 0x1010, 4), ('helper', 0x1020, 4)],
                      {0x1000: 0x48000021, 0x1004: 0x4800001d, 0x1010: 0x48000011})
        graph = dependency_graph(elf, [('accepted', 0x1010, 4)])
        row = graph['ranked'][0]
        self.assertEqual((row['callers'], row['unfinished_callers'], row['sites']), (2, 1, 3))
        self.assertEqual(row['unfinished_caller_bytes'], 12)

    def test_loops_interior_targets_and_unknown_targets(self):
        elf = FakeElf([('one', 0x1000, 16), ('two', 0x1020, 8)],
                      {0x1000: 0x48000008, 0x1004: 0x48000021,
                       0x1008: 0x48000039, 0x100c: 0x48000014})
        graph = dependency_graph(elf)
        self.assertEqual(len(graph['edges']), 1)
        self.assertEqual(graph['edges'][0]['kind'], 'branch')
        self.assertEqual({e['reason'] for e in graph['unresolved']},
                         {'interior-function-target', 'no-function-entry'})

    def test_overlapping_aliases_do_not_duplicate_instructions_or_fan_in(self):
        elf = FakeElf([('one', 0x1000, 8), ('alias', 0x1000, 8),
                       ('helper', 0x1020, 8), ('helper_inner', 0x1024, 4)],
                      {0x1000: 0x48000025, 0x1004: 0x4e800421})
        graph = dependency_graph(elf)
        self.assertEqual(len(graph['nodes']), 2)
        self.assertTrue(graph['nodes'][0]['shared_range'])
        self.assertEqual(graph['edges'][0]['target_names'], ['helper_inner'])
        self.assertEqual(graph['ranked'][0]['callers'], 1)
        self.assertEqual(graph['summary']['indirect_call_sites'], 1)

    def test_recursive_calls_are_recorded_but_do_not_inflate_fan_in(self):
        graph = dependency_graph(FakeElf([('recursive', 0x1000, 8)], {0x1004: 0x4bfffffd}))
        self.assertEqual(len(graph['edges']), 1)
        self.assertEqual(graph['ranked'], [])

    def test_invalid_function_boundaries_are_rejected(self):
        for address, size in [(0x1002, 4), (0x1000, 3), (0x107c, 8)]:
            with self.subTest(address=address, size=size), self.assertRaises(ValueError):
                dependency_graph(FakeElf([('bad', address, size)], {}))


if __name__ == '__main__':
    unittest.main()
