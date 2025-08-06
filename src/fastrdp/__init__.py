import numpy as np
import numpy.typing as npt

from _fastrdp import rdp_index


def rdp(x: npt.NDArray[np.float64], y: npt.NDArray[np.float64], epsilon: float) -> tuple[npt.NDArray[np.float64], npt.NDArray[np.float64]]:
    index = rdp_index(x, y, epsilon)
    return x[index], y[index]


def rdpN(X: npt.NDArray[np.float64], epsilon: float) -> npt.NDArray[np.float64]:
    index = rdp_index(X, epsilon)
    return X[index]