import numpy as np
import numpy.typing as npt

from _fastrdp import rdp_index


def rdp(x: npt.NDArray[np.float64], y: npt.NDArray[np.float64], epsilon: float) -> tuple[npt.NDArray[np.float64], npt.NDArray[np.float64]]:
    """
    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Select a subset of the points as a coarser approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
    """
    index = rdp_index(x, y, epsilon)
    return x[index], y[index]


def rdpN(X: npt.NDArray[np.float64], epsilon: float) -> npt.NDArray[np.float64]:
    """
    The input is a 2D array `X` where each row represents a point in a curve.
    Select a subset of the points as a coarser approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
    """
    index = rdp_index(X, epsilon)
    return X[index]