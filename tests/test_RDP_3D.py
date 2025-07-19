import numpy as np
import pytest
from numpy.testing import assert_array_equal
from fastrdp import rdp, rdp_index


class TestVaryingEpsilon3D:
    x = np.array([0.0, 1, 3, 5])
    y = np.array([2.0, 1, 0, 1])
    z = np.array([0.0, 0, 0, 0])

    def test_big_epsilon(self):
        x_new, y_new, z_new = rdp(self.x, self.y, self.z, 0.5)
        idx = rdp_index(self.x, self.y, self.z, 0.5)

        assert_array_equal(idx, np.array([0, 2, 3]))
        assert_array_equal(x_new, np.array([0.0, 3, 5]))
        assert_array_equal(y_new, np.array([2.0, 0, 1]))
        assert_array_equal(z_new, np.array([0.0, 0, 0]))

    def test_small_epsilon(self):
        x_new, y_new, z_new = rdp(self.x, self.y, self.z, 0.1)

        assert_array_equal(x_new, self.x)
        assert_array_equal(y_new, self.y)
        assert_array_equal(z_new, self.z)

    def test_all_points_kept_when_epsilon_is_zero(self):
        n = 10
        x = np.random.normal(0, 1, n)
        y = np.random.normal(0, 1, n)
        z = np.random.normal(0, 1, n)

        x_new, y_new, z_new = rdp(x, y, z, 0.0)

        assert_array_equal(x_new, x)
        assert_array_equal(y_new, y)
        assert_array_equal(z_new, z)


class TestLineSegments3D:
    def test_only_endpoints_kept_on_a_line(self):
        n = 10
        x = np.random.uniform(0, 10, n)
        x.sort()
        a, b = np.random.normal(0, 1, 2)
        y = a * x + b
        c, d = np.random.normal(0, 1, 2)
        z = c * x + d

        x_new, y_new, z_new = rdp(x, y, z, 0.1)
        assert_array_equal(x_new, np.array([x[0], x[-1]]))
        assert_array_equal(y_new, np.array([y[0], y[-1]]))
        assert_array_equal(z_new, np.array([z[0], z[-1]]))

    def test_vertical_segments(self):
        x = np.repeat(10.0, 4)
        y = np.array([10.0, 15, 15, 20])
        z = np.array([5.0, 5, 5, 5])

        x_new, y_new, z_new = rdp(x, y, z, 0.1)
        assert_array_equal(x_new, np.array([x[0], x[-1]]))
        assert_array_equal(y_new, np.array([y[0], y[-1]]))
        assert_array_equal(z_new, np.array([z[0], z[-1]]))


class TestEdgeCases3D:
    def test_input_is_returned_with_single_point(self):
        x = np.array([0.0])
        y = np.array([0.0])
        z = np.array([0.0])

        x_new, y_new, z_new = rdp(x, y, z, 1)

        assert_array_equal(x, x_new)
        assert_array_equal(y, y_new)
        assert_array_equal(z, z_new)

    def test_input_is_returned_with_two_points(self):
        x = np.array([0.0, 1])
        y = np.array([0.0, 1])
        z = np.array([0.0, 2])

        x_new, y_new, z_new = rdp(x, y, z, 1)

        assert_array_equal(x, x_new)
        assert_array_equal(y, y_new)
        assert_array_equal(z, z_new)


class TestCastingInputs3D:
    def test_inputs_are_casted_to_numpy(self):
        x = np.array([1.0, 2])
        y = np.array([1.0, 2])
        z = np.array([1.0, 2])

        x_new, _, _ = rdp(x.tolist(), y.tolist(), z.tolist(), 0)
        assert_array_equal(x, x_new)


class TestErrorHandling3D:
    x = np.array([1.0, 2, 3])
    y = np.array([1.0, 2, 3])
    z = np.array([1.0, 2, 3])

    def test_error_when_epsilon_is_negative(self):
        with pytest.raises(ValueError, match="epsilon must be non-negative"):
            rdp(self.x, self.y, self.z, -1)

    def test_error_when_input_vectors_have_different_lengths(self):
        with pytest.raises(ValueError, match="Inputs have different lengths"):
            rdp(self.x[1:2], self.y, self.z, 1)

    @pytest.mark.parametrize("x", ["foo", "bar"])
    def test_error_when_input_is_not_numpy_like(self, x):
        with pytest.raises(TypeError, match="incompatible function arguments"):
            rdp(x, self.y, self.z, 1)

    def test_error_when_input_is_not_1dim(self):
        with pytest.raises(ValueError, match="Inputs should be vectors"):
            z2 = np.column_stack((self.z, self.z))
            rdp(self.x, self.y, z2, 1)
