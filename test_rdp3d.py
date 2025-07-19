import matplotlib.pyplot as plt
import numpy as np
import fastrdp


# 2D test
# x = np.linspace(0, 5, 10_000)
# y = np.exp(-x) * np.cos(2 * np.pi * x)
# z = x
# x_new, y_new, z_new = fastrdp.rdp(x, y, z, 0.1)

# fig, ax = plt.subplots()
# ax.plot(x, y, color='black', linewidth=2.0)
# ax.plot(x_new, y_new, linestyle='dashed', color='red')
# plt.show()

# 3D spiral
t = np.linspace(0, 10 * np.pi, 10_000)
x = t * np.cos(t)
y = t * np.sin(t)
z = t

x_new, y_new, z_new = fastrdp.rdp(x, y, z, 0.1)  # Adjust epsilon as needed

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot(x, y, z, color='black', linewidth=2.0, label='Original spiral')
ax.plot(x_new, y_new, z_new, '.--', color='red', label='Simplified spiral')

ax.legend()
plt.show()