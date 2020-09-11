import pytest
import unittest


class ClassTest(unittest.TestCase):

     @pytest.mark.xfail
     def test_feature_a(self):
        self.assertEqual(2, 3)

     def test_feature_b(self):
        self.assertTrue(True)
