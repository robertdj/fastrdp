from typing import Sequence, overload

import numpy as np
import numpy.typing as npt

@overload
def rdp(x: Sequence[float], y: Sequence[float], epsilon: float) -> tuple[npt.NDArray[np.floating], npt.NDArray[np.floating]]:
    """
    rdp_index(x, y, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Select a subset of the points as a coarser approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
    """
    ...

@overload
def rdp(x: Sequence[float], y: Sequence[float], z: Sequence[float], epsilon: float) -> tuple[npt.NDArray[np.floating], npt.NDArray[np.floating], npt.NDArray[np.floating]]:
    """
    rdp_index(x, y, z, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Select a subset of the points as a coarser approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
    """
    ...

def rdp_index(x: Sequence[float], y: Sequence[float], y: Sequence[float], epsilon: float) -> npt.NDArray[np.int_]:
    """
    rdp_index(x, y, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Returns the indices of the elements that are kept in an approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
    """
    ...