from timeit import timeit
from datetime import datetime

from setup import get_fastrdp_version, save_results
import polars as pl
import numpy as np

from fastrdp import rdp

max_exp = 6
max_size = pow(10, max_exp)
x = np.random.rand(max_size)
y = np.random.rand(max_size)

exponents = list(range(4, max_exp + 1))

execution_times = []
for exp in exponents:
    size = pow(10, exp)
    this_x = x[0:size]
    this_y = y[0:size]
    repetitions = 1 if exp >= 6 else 10

    total_time = timeit(lambda: rdp(this_x, this_y, 0.1), number=repetitions)
    average_time = total_time / repetitions
    execution_times.append(average_time)


execution_df = pl.DataFrame({
    'Name': 'fastrdp-python',
    'Version': get_fastrdp_version(),
    'Exponents': exponents,
    'ExecutionTime': execution_times,
    'TimeOfExecution': datetime.now()
})

save_results(execution_df)
