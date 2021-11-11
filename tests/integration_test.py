import unittest
from json2daisy import board_gen

parameters = {
    "in": [
        (
            "Ctrl1",
            {
                "display": "Ctrl1",
                "hash": "0xCFD93C2D",
                "extern": "param",
                "attributes": {"min": 0.0, "max": 1.0, "default": 0.5, "type": "float"},
                "ids": ["GZ0yVz87"],
            },
        ),
        (
            "Ctrl2",
            {
                "display": "Ctrl2",
                "hash": "0xA6D27094",
                "extern": "param",
                "attributes": {"min": 0.0, "max": 1.0, "default": 0.5, "type": "float"},
                "ids": ["S58QZ4vW"],
            },
        ),
        (
            "Ctrl3",
            {
                "display": "Ctrl3",
                "hash": "0xB00C3479",
                "extern": "param",
                "attributes": {"min": 0.0, "max": 1.0, "default": 0.5, "type": "float"},
                "ids": ["yBtC9KfI"],
            },
        ),
        (
            "Ctrl4",
            {
                "display": "Ctrl4",
                "hash": "0x4BA4C205",
                "extern": "param",
                "attributes": {"min": 0.0, "max": 1.0, "default": 0.5, "type": "float"},
                "ids": ["sOSTeofW"],
            },
        ),
        (
            "EncSwitch",
            {
                "display": "EncSwitch",
                "hash": "0x7AAC371F",
                "extern": "param",
                "attributes": {"min": 0.0, "max": 1.0, "default": 0.5, "type": "float"},
                "ids": ["MR5TdE9N"],
            },
        ),
        (
            "Encoder",
            {
                "display": "Encoder",
                "hash": "0x39ADE514",
                "extern": "param",
                "attributes": {"min": 0.0, "max": 1.0, "default": 0.5, "type": "float"},
                "ids": ["F1cltcjM"],
            },
        ),
        (
            "Gate1",
            {
                "display": "Gate1",
                "hash": "0x378CB51F",
                "extern": "param",
                "attributes": {"min": 0.0, "max": 1.0, "default": 0.5, "type": "float"},
                "ids": ["hmkbqUZG"],
            },
        ),
        (
            "Gate2",
            {
                "display": "Gate2",
                "hash": "0x8924DDEB",
                "extern": "param",
                "attributes": {"min": 0.0, "max": 1.0, "default": 0.5, "type": "float"},
                "ids": ["R8zdadDJ"],
            },
        ),
    ],
    "out": [],
}

example_input = [
    "patch",
    "",
    parameters,
    "patch_test",
    "HeavyDaisy_patch_test",
    '',
    {"daisy": {"board": "patch", "linker_script": ""}},
]

class TestIntegration(unittest.TestCase):

    def test_integration(self):
        self.maxDiff = None
        hpp, cpp, makefile = board_gen.generate_board(*example_input)
        with open('data/expected_hpp.txt', 'r') as file:
          self.assertEqual(hpp, file.read(), 'The output string should match "expected_hpp.txt" exactly')
        with open('data/expected_cpp.txt', 'r') as file:
          self.assertEqual(cpp, file.read(), 'The output string should match "expected_cpp.txt" exactly')
        with open('data/expected_make.txt', 'r') as file:
          self.assertEqual(makefile, file.read(), 'The output string should match "expected_make.txt" exactly')
