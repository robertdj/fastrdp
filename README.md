# fastrdp

<a href="https://pypi.org/project/fastrdp/">
<img src="https://img.shields.io/pypi/v/fastrdp" alt="PyPI Latest Release"/>
</a>

The *fastrdp* package contains an implementation of the
[Ramer–Douglas–Peucker
algorithm](https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm)
for reducing the number of points in a curve.

# Example

There is a single function in the *fastrdp* package. Here is an example
from the Wikipedia page linked to above with a description of the
algorithm. The original line is black and the approximating line is red.

``` python
import matplotlib.pyplot as plt
import numpy as np
import fastrdp

x = np.linspace(0, 5, 10_000)
y = np.exp(-x) * np.cos(2 * np.pi * x)
x_new, y_new = fastrdp.rdp(x, y, 0.06)

fig, ax = plt.subplots()
ax.plot(x, y, color='black', linewidth=2.0)
ax.plot(x_new, y_new, linestyle='dashed', color='red')
plt.show()
```

![](https://github.com/robertdj/fastrdp/raw/main/README_files/figure-commonmark/cell-2-output-1.png)

# Performance

Here we compare the performance of *fastrdp* with that of a pure Python
implementation. The example above is executed with *fastrdp* in less
than a millisecond on my machine

``` python
from timeit import timeit
timeit(lambda: fastrdp.rdp(x, y, 0.1), number=10_000)
```

    0.9878715319791809

The pure Python implementation in the [*rdp*
package](https://pypi.org/project/rdp) takes more than a second to
finish the same computation

``` python
import rdp
z = np.column_stack((x, y))
timeit(lambda: rdp.rdp(z, epsilon=0.1), number=1)
```

    1.6936232760199346

# Compilation

To specify package metadata *fastrdp* is using the contemporary
`pyproject.toml`. However, to control compilation of the C++ code a
legacy `setup.py` is used.

Execute the following commands to build and install *fastrdp*

``` bash
pip install .
```

If you want an editable installation, then add a `-e` to the last
command. To also install the optional dependencies used for development
add a little extra:

``` bash
pip install -e '.[dev]'
```

The single quotes are needed in zsh, but is perhaps superfluous in other
shells.

# Acknowledgements

Setting up the package build using `pyproject.toml` was not easy, but
these two blog posts helped tremendeously:

- <https://opensource.com/article/22/11/extend-c-python>
- <https://opensource.com/article/23/1/packaging-python-modules-wheels>

In `setup.py` there is a [bootstrap dependency on
numpy](https://stackoverflow.com/questions/76678675/modulenotfounderror-no-module-named-numpy-problem-installing-it).
