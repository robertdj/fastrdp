import os
from timeit import timeit
from datetime import datetime

from setup import get_fastrdp_version, get_results_folder, save_results
import polars as pl
import numpy as np

from fastrdp import rdp


def measure_execution_times(df: pl.DataFrame) -> pl.DataFrame:
    max_size = df.height
    max_exp = int(np.log10(max_size))

    exponents = list(range(4, max_exp + 1))

    execution_times = []
    for exp in exponents:
        size = pow(10, exp)
        this_x = df.slice(0, size).get_column('x').to_numpy()
        this_y = df.slice(0, size).get_column('y').to_numpy()
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

    return execution_df


random_input = pl.read_parquet(os.path.join(get_results_folder(), 'random.parquet'))
random_execution_times = measure_execution_times(random_input)
save_results(random_execution_times, 'random_')

structured_input = pl.read_parquet(os.path.join(get_results_folder(), 'structured.parquet'))
structured_execution_times = measure_execution_times(structured_input)
save_results(structured_execution_times, 'structured_')
