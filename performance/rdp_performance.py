import pyperf

from rdp import rdp
import numpy as np

if __name__ == '__main__':
    size = 10_000

    x = np.random.rand(size)
    y = np.random.rand(size)
    z = np.column_stack((x, y))

    runner = pyperf.Runner()
    runner.timeit('rdp', 'result = rdp(z, 0.1)', globals=locals())
