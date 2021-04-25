#!/usr/bin/python3

import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_table("solution.txt", sep='\t', header=None)

fig = plt.figure(figsize=(3, 3), dpi=200)
ax = fig.gca(projection='3d')

ax.plot_trisurf(data[0], data[1], data[2], cmap='viridis', edgecolor='none')
plt.xlabel("t")
plt.ylabel("x")
ax.set_zlabel('u')
plt.grid()
plt.show()
# plt.savefig("aloha.png", dpi=250)

