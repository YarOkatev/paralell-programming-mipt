#!/usr/bin/python3

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy import stats


def confidenceInterval (serviceTimes, alpha = 0.01):
    n = len(serviceTimes)
    return np.sqrt(np.var(serviceTimes, ddof=1)) / np.sqrt(n) * stats.t.ppf(1-alpha/2, n-1)

df = pd.read_table("final-results-sorted.txt", sep='\t', index_col = None, header = 0)

colors = ['g', 'r', 'c', 'm', 'y', 'k', 'w']
k_arr = [1, 2, 4]

for i in range(len(k_arr)):
    qs = df.loc[df['Cores'] == k_arr[i]]
    qs = qs.groupby(['Size'])['Time'].agg([np.mean, confidenceInterval]).reset_index() #average over runs
    plt.errorbar(qs['Size'], qs['mean'], qs['confidenceInterval'], marker = 'x', linestyle = '-', color = colors[i], label = str(k_arr[i]) + ' core(s)')

# plt.ylim(0, 0.45)
plt.title('Sorting of sorted arrays')
plt.xlabel("Array size")
plt.ylabel("Sorting time, sec")
plt.legend(loc="upper left")
plt.grid()
plt.savefig("sorted.png", dpi=250)

