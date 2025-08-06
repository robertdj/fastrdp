from timeit import timeit
from importlib.metadata import version
import pathlib

import polars as pl
import numpy as np
import numpy.typing as npt
import plotnine as pn

from fastrdp import rdp, rdpN

def measure_execution_time(x: npt.NDArray, y: npt.NDArray, epsilon: float = 0.05) -> float:
    repetitions = 2 if len(x) >= 1_000_000 else 4
    total_time1 = timeit(lambda: rdp(x, y, epsilon), number=repetitions)
    average_time1 = total_time1 / repetitions

    X = np.column_stack((x, y))
    total_time2 = timeit(lambda: rdpN(X, epsilon), number=repetitions)
    average_time2 = total_time2 / repetitions

    return average_time1, average_time2

def measure_execution_times(max_exp: int, f) -> pl.DataFrame:
    exponents = list(range(4, max_exp + 1))
    times1 = []
    times2 = []
    for exp in exponents:
        print(exp)
        x, y = f(exp)
        time1, time2 = measure_execution_time(x, y)
        times1.append(time1)
        times2.append(time2)

    df_execution_times = pl.DataFrame({
        'Name': 'fastrdp-python',
        'Version': version('fastrdp'),
        'Exponent': exponents,
        'ExecutionTime': times1,
        'ExecutionTimeN': times2,
    }).with_columns(
        pl.lit(10).pow(pl.col('Exponent')).alias('NumberOfObs')
    )
    return df_execution_times

def make_random_data(exp: int) -> tuple[np.ndarray, np.ndarray]:
    size = 10 ** exp
    x = np.random.rand(size)
    y = np.random.rand(size)
    return x, y


def make_structured_data(exp: int) -> tuple[np.ndarray, np.ndarray]:
    size = 10 ** exp
    x = np.linspace(0, size // 1_000, size)
    frequency = (2_000 * np.pi) / size
    y = np.sin(frequency * x)
    return x, y


def plot_results(df: pl.DataFrame) -> pn.ggplot:
    p = (
        pn.ggplot(df.to_pandas(), pn.aes(x='NumberOfObs', y='ExecutionTime', group='data type', color='data type'))
        + pn.scale_x_log10()
        + pn.xlab('Number of points')
        + pn.scale_y_log10()
        + pn.ylab('Execution time [seconds]')
        + pn.geom_point()
        + pn.geom_line()
        + pn.ggtitle(f'Execution times for fastrdp version {df[0, "Version"]}')
    )

    return p


if __name__ == "__main__":
    max_exp = 7

    df_structured_execution_times = measure_execution_times(max_exp, make_structured_data).with_columns(
        pl.lit('structured').alias('data type')
    )
    print(df_structured_execution_times)

    df_random_execution_times = measure_execution_times(max_exp, make_random_data).with_columns(
        pl.lit('random').alias('data type')
    )
    print(df_random_execution_times)

    df = pl.concat([df_structured_execution_times, df_random_execution_times])
    p = plot_results(df)

    performance_folder = pathlib.Path(__file__).parent
    p.save(performance_folder / 'performance.png', width=20, height=12, units='cm')