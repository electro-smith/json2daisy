
import unittest
import sys
from os import path

# Ensuring the json2daisy module we test is the local one here
sys.path.insert(0, path.join(path.dirname(path.dirname(path.abspath(__file__))), 'src'))
# Grabbing the absolute path to test data
data_path = path.join(path.dirname(path.abspath(__file__)), 'data')

import json2daisy  # noqa


class TestIntegration(unittest.TestCase):

    def test_patch(self):
        self.maxDiff = None
        header, info = json2daisy.generate_header_from_name('patch')
        with open(path.join(data_path, 'integration', 'expected_patch.h'), 'r') as file:
            self.assertEqual(header, file.read(), 'The output string should match "expected_patch.h" exactly')

    def test_patch_init(self):
        self.maxDiff = None
        header, info = json2daisy.generate_header_from_name('patch_init')
        with open(path.join(data_path, 'integration', 'expected_patch_init.h'), 'r') as file:
            self.assertEqual(header, file.read(), 'The output string should match "expected_patch_init.h" exactly')

    def test_petal(self):
        self.maxDiff = None
        header, info = json2daisy.generate_header_from_name('petal')
        with open(path.join(data_path, 'integration', 'expected_petal.h'), 'r') as file:
            self.assertEqual(header, file.read(), 'The output string should match "expected_petal.h" exactly')

    def test_petal_125b_sm(self):
        self.maxDiff = None
        header, info = json2daisy.generate_header_from_name('petal_125b_sm')
        with open(path.join(data_path, 'integration', 'expected_petal_125b_sm.h'), 'r') as file:
            self.assertEqual(header, file.read(), 'The output string should match "expected_petal_125b_sm.h" exactly')

    def test_pod(self):
        self.maxDiff = None
        header, info = json2daisy.generate_header_from_name('pod')
        with open(path.join(data_path, 'integration', 'expected_pod.h'), 'r') as file:
            self.assertEqual(header, file.read(), 'The output string should match "expected_pod.h" exactly')

    def test_field(self):
        self.maxDiff = None
        header, info = json2daisy.generate_header_from_name('field')
        with open(path.join(data_path, 'integration', 'expected_field.h'), 'r') as file:
            self.assertEqual(header, file.read(), 'The output string should match "expected_field.h" exactly')
