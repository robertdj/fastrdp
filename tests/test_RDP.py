import numpy as np
import pytest
from numpy.testing import assert_array_equal
from fastrdp import rdp


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

    def test_all_points_kept_when_epsilon_is_zero(self):
        n = 10
        x = np.random.normal(0, 1, n)
        y = np.random.normal(0, 1, n)

        x_new, y_new = rdp(x, y, 0.0)

        assert_array_equal(x_new, x)
        assert_array_equal(y_new, y)


class TestLineSegments:
    def test_only_endpoints_kept_on_a_line(self):
        n = 10
        x = np.random.uniform(0, 10, n)
        x.sort()
        a, b = np.random.normal(0, 1, 2)
        y = a * x + b

        x_new, y_new = rdp(x, y, 0.1)
        assert_array_equal(x_new, np.array([x[0], x[-1]]))
        assert_array_equal(y_new, np.array([y[0], y[-1]]))

    def test_vertical_segments(self):
        x = np.repeat(10.0, 4)
        y = np.array([10.0, 15, 15, 20])

        x_new, y_new = rdp(x, y, 0.1)
        assert_array_equal(x_new, np.array([x[0], x[-1]]))
        assert_array_equal(y_new, np.array([y[0], y[-1]]))


class TestErrorHandling:
    x = np.arange(1, 4)
    y = np.arange(1, 4)

    def test_error_when_epsilon_is_negative(self):
        with pytest.raises(ValueError, match="epsilon must be non-negative"):
            rdp(self.x, self.y, -1)

    def test_error_when_input_vectors_have_different_lengths(self):
        with pytest.raises(ValueError, match="Inputs have different lengths"):
            rdp(self.x[0:2], self.y, 1)

    def test_first_input_must_be_floats(self):
        with pytest.raises(TypeError, match="Input must be a Numpy array of type float"):
            rdp(self.x, self.y.astype(float), 1)

    def test_second_input_must_be_floats(self):
        with pytest.raises(TypeError, match="Input must be a Numpy array of type float"):
            rdp(self.x.astype(float), self.y, 1)