import os

import polars as pl
import plotnine as pn

from performance.setup import get_readme_image_folder, get_results_folder


results_folder = get_results_folder()
results_df = (
    pl.read_csv(os.path.join(results_folder, 'random_*.csv'))
    .with_columns(
        pl.lit(10).pow(pl.col('Exponent')).alias('NumberOfObs')
    )
)

p = (
    pn.ggplot(results_df.to_pandas(), pn.aes(x='NumberOfObs', y='ExecutionTime', group='Version', color='Version'))
    + pn.scale_x_log10()
    + pn.xlab('Number of points')
    + pn.scale_y_log10()
    + pn.ylab('Execution time')
    + pn.geom_point()
    + pn.geom_line()
)

save_file = get_readme_image_folder().joinpath('performance.png')
p.save(save_file, width=20, height=12, units='cm')
