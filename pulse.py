from _pyfmcw import *
import matplotlib.pyplot as plt
import numpy as np

Fs = 48000
Tp = 5.0*10**-3
# c = 3.0*10**8
pulseN = int(Fs*Tp/2.0)
scanN = int(2.5*pulseN)
print(pulseN)
print(scanN)


setup(scanN, pulseN, Fs)
# p = rawPulse(0.5)
sr, sl = twoChannels()


plt.ion()
lr,	 = plt.plot(sr)
ll, = plt.plot(sl)
plt.show()
plt.ylim(-1.0,1.0)
for i in range(1000): 
	setup(scanN, pulseN, Fs)
	# p = rawPulse(0.5)
	# ln.set_ydata(p)
	sr, sl = twoChannels()
	
	# sr = np.abs(sr)
	lr.set_ydata(sr)
	ll.set_ydata(sl)
	plt.draw()
	np.fft.fft(sr)
	plt.pause(0.001)


