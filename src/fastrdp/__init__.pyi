from typing import Sequence

import numpy as np
import numpy.typing as npt

def rdp_index(x: npt.NDArray[np.float64], y: npt.NDArray[np.float64], epsilon: float) -> npt.NDArray[np.int_]:
    """
    rdp_index(x, y, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Returns the indices of the elements that are kept in an approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
    """
    ...