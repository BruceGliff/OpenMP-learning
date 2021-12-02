#!/bin/python3

import numpy  as np
import matplotlib.pyplot as plt

data = np.loadtxt("timing.dat")
threads = data[:, 0]
time = data[:, 1]

first_time = time[0]
acceleration = first_time / time
efficienty = acceleration / threads

plt.plot(threads, time)
plt.plot(threads, acceleration)
plt.plot(threads, efficienty)
plt.legend(['time', 'acc', 'eff'])

plt.show()
