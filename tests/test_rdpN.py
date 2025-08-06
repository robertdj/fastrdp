
import numpy as np
import pytest
from numpy.testing import assert_array_equal
from fastrdp import rdpN, rdp_index


class TestPlaneCurves:
    x = np.array([0.0, 1, 3, 5])
    y = np.array([2.0, 1, 0, 1])
    z = np.array([0.0, 0, 0, 0])
    X = np.column_stack((x, y))
    Z = np.column_stack((x, y, z))

    def test_index_2D_array(self):
        idx = rdp_index(self.X, 0.5)
        assert_array_equal(idx, np.array([0, 2, 3]))

    def test_index_3D_array(self):
        idx = rdp_index(self.Z, 0.5)
        assert_array_equal(idx, np.array([0, 2, 3]))

    def test_2D_array_equal_two_vectors(self):
        idx1 = rdp_index(self.x, self.y, 0.5)
        idx2 = rdp_index(self.X, 0.5)
        assert_array_equal(idx1, idx2)


class TestVaryingEpsilon:
    x = np.array([0.0, 1, 3, 5])
    y = np.array([2.0, 1, 0, 1])
    z = np.array([9.0, 8, 7, 6])
    X = np.column_stack((x, y))
    Z = np.column_stack((x, y, z))

    def test_all_points_kept_when_epsilon_is_zero(self):
        n = 10
        X = np.random.normal(0, 1, (n, 3))

        X_new = rdpN(X, 0.0)

        assert_array_equal(X_new, X)


class TestEdgeCases:
    def test_empty_input(self):
        x = np.array([])
        y = np.array([])

        X = np.column_stack((x, y))
        X_new = rdpN(X, 0.1)
        assert_array_equal(X_new, X)

    def test_input_is_returned_with_single_point(self):
        x = np.array([0.0])
        y = np.array([0.0])
        X = np.column_stack((x, y))

        X_new = rdpN(X, 0.1)
        assert_array_equal(X_new, X)

        X_new = rdpN(X, 0.1)
        assert_array_equal(X_new, X)

    def test_input_is_returned_with_two_points(self):
        x = np.array([0.0, 1.0])
        y = np.array([0.0, 1.0])
        X = np.column_stack((x, y))

        X_new = rdpN(X, 0.1)
        assert_array_equal(X_new, X)

        X_new = rdpN(X, 0.1)
        assert_array_equal(X_new, X)


class TestErrorHandling:
    x = np.array([0.0, 1.0, 3.0])
    X = np.column_stack((x, x, x))

    def test_invalid_epsilon(self):
        with pytest.raises(ValueError, match="epsilon must be non-negative"):
            rdpN(self.X, -0.1)

    def test_invalid_input_shape(self):
        X = np.arange(24).reshape(2, 3, 4)
        with pytest.raises(ValueError, match="Input must be a 2D array"):
            rdpN(X, 0.1)