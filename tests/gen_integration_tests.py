"""
This will generate simple integration tests for all the included JSON. These should be
reviewed manually every time they're generated to ensure the output matches what's expected.
Following their generation, any changes to the code that changes the output will be caught here.
Therefore, this should _not_ be run via CI or other automated methods; it should only change
when you know the output should change.
"""

import sys
import shutil
from os import path, listdir, mkdir

# Ensuring the json2daisy module we test is the local one here
sys.path.insert(0, path.join(path.dirname(path.dirname(path.abspath(__file__))), 'src'))
abs_path = path.dirname(path.abspath(__file__))
# Grabbing the absolute path to test data
data_path = path.join(abs_path, 'data')
integration_path = path.join(data_path, 'integration')

import json2daisy

if path.isdir(integration_path):
  shutil.rmtree(integration_path)
mkdir(integration_path)

non_templates = ['component_defaults', 'component_defaults_patchsm', 'LICENSE']
json_path = path.abspath(path.join(abs_path, '../src/json2daisy/resources'))
included_json = [item.replace('.json', '') for item in listdir(json_path)]

test_file = """
import unittest
import sys
from os import path

# Ensuring the json2daisy module we test is the local one here
sys.path.insert(0, path.join(path.dirname(path.dirname(path.abspath(__file__))), 'src'))
# Grabbing the absolute path to test data
data_path = path.join(path.dirname(path.abspath(__file__)), 'data')

import json2daisy

class TestIntegration(unittest.TestCase):
"""

test_template = """
    def test_{name}(self):
        self.maxDiff = None
        header, name, components, aliases = json2daisy.generate_header_from_name('{name}')
        with open(path.join(data_path, 'integration', 'expected_{name}.h'), 'r') as file:
          self.assertEqual(header, file.read(), 'The output string should match "expected_{name}.h" exactly')
"""

for template_name in included_json:
  if template_name not in non_templates:
    header, name, components, aliases = json2daisy.generate_header_from_name(template_name)
    template_test_path = path.join('integration', f'expected_{name}.h')
    with open(path.join(data_path, template_test_path), 'w') as file:
      file.write(header)
    test_file += test_template.format_map({'name': name})

with open(path.join(abs_path, 'integration_test.py'), 'w') as file:
  file.write(test_file)
    