#!/bin/python3

import numpy  as np
import matplotlib.pyplot as plt

data = np.loadtxt("timing.dat")
threads = data[:, 0]
time = data[:, 1]
systime = data[:, 2]

nosys_time = time - systime
first_time = nosys_time[0]
acceleration = first_time / nosys_time
efficienty = acceleration / threads

plt.plot(threads, nosys_time)
plt.plot(threads, acceleration)
plt.plot(threads, efficienty)
plt.legend(['nosys time', 'acc', 'eff'])

plt.show()
