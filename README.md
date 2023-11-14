# PyRDP

The RDP package contains an implementation of the [Ramer–Douglas–Peucker
algorithm](https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm)
for reducing the number of points in a curve.

# Example

There is a single function in the RDP package. Here is an example from
the Wikipedia page linked to above with a description of the algorithm.
The original line is black and the approximating line is red.

``` python
import matplotlib.pyplot as plt
import numpy as np
from RDP import rdp

x = np.arange(0, 5, 0.01)
y = np.exp(-x) * np.cos(2 * np.pi * x)
x_new, y_new = rdp(x, y, 0.06)

fig, ax = plt.subplots()
ax.plot(x, y, color='black', linewidth=2.0)
ax.plot(x_new, y_new, linestyle='dashed', color='red')
plt.show()
```

![](README_files/figure-commonmark/cell-2-output-1.png)

# Compilation

Run the following command to install the package from source

``` bash
python setup.py install
```
