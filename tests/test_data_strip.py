"""Mutations of per-object data stripping and binding checks; skipped until reconstruct.py has run."""
import json
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from audit import load_target
from data_strip import strip_objects
from formats import Elf32
from project_build import validate_object, validate_units
from setup import CONFIG


def relocation_targets(elf, section_name):
    """Names of the symbols that relocations located in a section refer to."""
    symbols = list(elf.symbols())
    index = next(s["index"] for s in elf.sections if s["name"] == section_name)
    for rela in elf.sections:
        if rela["type"] == 4 and rela["info"] == index:
            for offset in range(0, rela["size"], 12):
                _, info, _ = struct.unpack_from(">IIi", elf.contents(rela), offset)
                yield symbols[info >> 8]


class DataStripping(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = ROOT / 'build/reconstruction'
        if not (cls.build / 'units/Pad/compiled.o').exists():
            raise unittest.SkipTest('Run reconstruct.py first')
        cls.original_path, cls.original = load_target()
        cls.unit = json.loads((CONFIG / 'Pad.json').read_text())
        cls.compiled = (cls.build / 'units/Pad/compiled.o').read_bytes()

    def mutated(self, section_name, objects):
        unit = json.loads(json.dumps(self.unit))
        next(s for s in unit['sections'] if s['name'] == section_name)['stripped_objects'] = objects
        return unit

    def test_stripped_objects_move_after_the_kept_slice(self):
        obj = Elf32(strip_objects(self.compiled, self.unit))
        section = next(s for s in obj.sections if s['name'] == '.sbss')
        manifest = next(s for s in self.unit['sections'] if s['name'] == '.sbss')
        for symbol in obj.symbols():
            if symbol['section'] == section['index'] and symbol['type'] == 1:
                stripped = symbol['name'].split('$')[0] in manifest['stripped_objects']
                self.assertEqual(symbol['address'] >= manifest['size'], stripped, symbol['name'])
        self.assertEqual(len(obj.data), len(self.compiled))

    def test_dead_data_no_longer_keeps_a_discarded_function(self):
        before = {s['name'] for s in relocation_targets(Elf32(self.compiled), '.data')}
        after = {s['name'] for s in relocation_targets(Elf32(strip_objects(self.compiled, self.unit)), '.data')}
        self.assertIn('OnReset', before)
        self.assertNotIn('OnReset', after)

    def test_stripping_a_referenced_object_fails(self):
        sbss = next(s for s in self.unit['sections'] if s['name'] == '.sbss')
        unit = self.mutated('.sbss', sbss['stripped_objects'] + ['EnabledBits'])
        with self.assertRaisesRegex(ValueError, 'Retained code or data references trimmed data'):
            validate_object(Elf32(strip_objects(self.compiled, unit)), unit)

    def test_stripping_an_object_present_in_the_target_fails(self):
        unit = self.mutated('.sbss', ['__PADSpec'])
        with self.assertRaisesRegex(ValueError, 'Cannot strip an object present in the target'):
            validate_units(self.original, [unit])

    def test_weak_binding_is_checked_before_linking(self):
        compiled = (self.build / 'units/ctype/compiled.o').read_bytes()
        unit = json.loads((CONFIG / 'ctype.json').read_text())
        self.assertEqual(unit['functions'][0]['binding'], 2)
        validate_object(Elf32(strip_objects(compiled, unit)), unit)
        unit['functions'][0]['binding'] = 1
        with self.assertRaisesRegex(ValueError, 'Compiled function binding differs'):
            validate_object(Elf32(strip_objects(compiled, unit)), unit)

    def test_input_alignment_relaxes_only_the_section_header(self):
        compiled = (self.build / 'units/smixfx/compiled.o').read_bytes()
        unit = json.loads((CONFIG / 'smixfx.json').read_text())
        before, after = Elf32(compiled), Elf32(strip_objects(compiled, unit))
        sbss = next(s for s in unit['sections'] if s['name'] == '.sbss')
        self.assertEqual((int(sbss['address'], 16) % 8, sbss['input_alignment']), (4, 4))
        for old, new in zip(before.sections, after.sections):
            expected = 4 if old['name'] == '.sbss' else old['alignment']
            self.assertEqual((old['alignment'], new['alignment']), (old['alignment'], expected), old['name'])
            self.assertEqual(dict(old, alignment=0), dict(new, alignment=0))

    def test_input_alignment_needs_a_4_mod_8_placement(self):
        unit = json.loads((CONFIG / 'smixfx.json').read_text())
        validate_units(self.original, [unit])
        mutated = json.loads(json.dumps(unit))
        next(s for s in mutated['sections'] if s['name'] == '.text')['input_alignment'] = 4
        with self.assertRaisesRegex(ValueError, 'Input alignment is not needed'):
            validate_units(self.original, [mutated])
        pad = json.loads(json.dumps(self.unit))
        aligned = next(s for s in pad['sections'] if s['name'] == '.sbss' and int(s['address'], 16) % 8 == 0)
        aligned['input_alignment'] = 4
        with self.assertRaisesRegex(ValueError, 'Input alignment is not needed'):
            validate_units(self.original, [pad])

    def test_input_alignment_only_lowers_8_to_4(self):
        compiled = (self.build / 'units/smixfx/compiled.o').read_bytes()
        unit = json.loads((CONFIG / 'smixfx.json').read_text())
        next(s for s in unit['sections'] if s['name'] == '.text')['input_alignment'] = 4
        with self.assertRaisesRegex(ValueError, 'only relax 8-aligned compiler data'):
            strip_objects(compiled, unit)

    def test_unknown_object_fails(self):
        with self.assertRaisesRegex(ValueError, 'not one compiler object'):
            strip_objects(self.compiled, self.mutated('.sbss', ['NoSuchObject']))


if __name__ == '__main__':
    unittest.main()
