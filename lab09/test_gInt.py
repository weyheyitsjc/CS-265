# Python 3.5.2 , on
# 4.13.0-38-generic GNU/Linux

'''
Author: Jacy Yu / jy529
Date: 11/24/19
'''

import sys
import unittest

from gInt import gInt

class gIntTest(unittest.TestCase):
    def setUp(self):
        self.a = gInt(1, 1)
        self.b = gInt(-1, 1)
        self.c = gInt(-1, -1)
        self.d = gInt(2, 3)

    def test_add(self):
        self.assertEqual(self.a + self.b, gInt(0, 2), "Addition failed")
        self.assertEqual(self.a + self.c, gInt(0, 0), "Addition failed")
        self.assertEqual(self.a + self.d, gInt(3, 4), "Addition failed")

    def test_mul(self):
        self.assertEqual(self.a * self.d, gInt(-1, 5), "Multiplication failed")
        self.assertEqual(self.b * self.d, gInt(-5, -1), "Multiplication failed")
        self.assertEqual(self.c * self.d, gInt(1, -5), "Multiplication failed")


    def test_norm(self):
        self.assertEqual(self.a.norm(), 2, "norm failed")
        self.assertEqual(self.d.norm(), 13, "norm failed")
        self.assertEqual(self.c.norm(), 2, "norm failed")


if __name__=="__main__":
    sys.argv.append( '-v' )
    unittest.main()