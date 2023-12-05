import os
from importlib.metadata import version

import polars as pl
import numpy as np


def get_results_folder():
    results_folder = os.path.join(os.path.dirname(__file__), 'results')
    return results_folder


def get_fastrdp_version():
    fastrdp_version = version('fastrdp')
    return fastrdp_version


def save_results(df: pl.DataFrame, prefix: str = ''):
    basename = f"{prefix}{df[0, 'Name']}_{df[0, 'Version']}.csv"
    filename = os.path.join(get_results_folder(), basename)

    if os.path.exists(filename):
        raise FileExistsError()

    df.write_csv(filename)


if __name__ == "__main__":
    results_folder = get_results_folder()
    if not os.path.exists(results_folder):
        os.makedirs(results_folder)

    max_exp = 6
    max_size = pow(10, max_exp)

    df1 = pl.DataFrame({
        'x': np.random.rand(max_size),
        'y': np.random.rand(max_size)
    })

    df1.write_parquet(os.path.join(get_results_folder(), 'random.parquet'))

    x = np.linspace(0, max_size / 100, max_size)
    df2 = pl.DataFrame({
        'x': x,
        'y': np.sin(x)
    })

    df2.write_parquet(os.path.join(get_results_folder(), 'structured.parquet'))