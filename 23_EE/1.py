import numpy as np
import matplotlib.pyplot as plt
t = np.linspace(0, 4*np.pi, 1000)
x = np.sin(t) + np.sin(3*t)
y = (1/np.sqrt(2))*np.sin(t - np.pi/4) + (1/np.sqrt(10))*np.sin(3*t - np.arctan(3))
plt.figure()
plt.plot(t, x, label='x(t)')
plt.plot(t, y, label='y(t)')
plt.legend()
plt.grid()
plt.show()