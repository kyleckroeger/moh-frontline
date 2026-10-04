"""Checks of the local source build; skipped until reconstruct.py has run.

Rising Sun's unit mutation tests (data ownership, small-data anchors, vtables,
discarded code) were specific to its accepted units. Add equivalent mutations
here alongside the first accepted Frontline source units.
"""
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from audit import load_target
from project_build import progress
from setup import CONFIG


class SourceVerification(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = ROOT / 'build/reconstruction'
        if not (cls.build / 'report.json').exists():
            raise unittest.SkipTest('Run reconstruct.py first')
        cls.original_path, cls.original = load_target()
        cls.project = json.loads((CONFIG / 'project.json').read_text())
        cls.units = [json.loads((CONFIG / n).read_text()) for n in cls.project['units']]

    def test_complete_image_identical(self):
        report = json.loads((self.build / 'report.json').read_text())
        self.assertEqual(report['status'], 'identical')
        self.assertEqual(report['progress'], progress(self.original, self.units, self.project))

    def test_denominator_is_every_executable_byte(self):
        result = progress(self.original, self.units, self.project)
        self.assertEqual(result['total_executable_code_bytes'], 1414572)

    def test_smaller_denominator_rejected(self):
        project = json.loads(json.dumps(self.project))
        project['progress']['executable_sections']['.text'] -= 4
        project['progress']['code_bytes'] -= 4
        with self.assertRaisesRegex(ValueError, 'denominator'):
            progress(self.original, self.units, project)


if __name__ == '__main__':
    unittest.main()
