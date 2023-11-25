import pyperf

from fastrdp import rdp
import numpy as np

if __name__ == '__main__':
    size = 10_000

    x = np.random.rand(size)
    y = np.random.rand(size)

    runner = pyperf.Runner()
    runner.timeit('fastrdp', 'result = rdp(x, y, 0.06)', globals=locals())
