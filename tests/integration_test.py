import unittest
from json2daisy import board_gen

class TestIntegration(unittest.TestCase):
  
  def test_integration(self):
    hpp, cpp, makefile = board_gen.generate_board(
      'patch', '', [], 'patch_test', 'patch_test', '', {}
    )
    