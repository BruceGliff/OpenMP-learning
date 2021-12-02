#!/bin/python3

import numpy  as np
import matplotlib.pyplot as plt

data = np.loadtxt("timing.dat")
processes = data[:, 0]
time = data[:, 1]

first_time = time[0]
acceleration = first_time / time
efficienty = acceleration / processes

plt.plot(processes, time)
plt.plot(processes, acceleration)
plt.plot(processes, efficienty)
plt.legend(['time', 'acc', 'eff'])

plt.show()
