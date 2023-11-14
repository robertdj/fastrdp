import numpy as np
from numpy.testing import assert_array_equal
from RDP import rdp

class TestRDPVaryingEpsilon:
    x = np.array([0.0, 1, 3, 5])
    y = np.array([2.0, 1, 0, 1])

    def test_big_epsilon(self):
        x_new, y_new = rdp(self.x, self.y, 0.5)

        assert_array_equal(x_new, np.array([0.0, 3, 5]))
        assert_array_equal(y_new, np.array([2.0, 0, 1]))

    def test_small_epsilon(self):
        x_new, y_new = rdp(self.x, self.y, 0.1)

        assert_array_equal(x_new, self.x)
        assert_array_equal(y_new, self.y)