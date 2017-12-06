from _pyfmcw import *
import matplotlib.pyplot as plt
import numpy as np
import scipy.signal

Fs = 22100
scanN = 128
pulseN = 2
nfft = 128

setup(scanN, pulseN, Fs)

trigger, signal = twoChannels()

hamming_window = scipy.signal.hamming(scanN)




plt.ion()
# rectUp = plt.axvspan(0, pulseN, ymin=-1, ymax=1, alpha = 0.2, color = 'b')
# rectDown = plt.axvspan(0, pulseN, ymin=-1, ymax=1, alpha = 0.2, color = 'purple')
# lr, = plt.plot(trigger)
# ll, = plt.plot(signal)
# plt.ylim(-1.0,1.0)

plt.figure()
plotRows = 80
data = np.empty([plotRows,nfft/2])
image_plot = plt.imshow(data, vmin = 0, #vmax = 80,
						aspect = 'auto', 
						extent=[0,plotRows,0,Fs/2.0],
						cmap = 'jet')#,vmin = -20)
# plt.colorbar()
currentRow = 0
plt.show()

while True: 
	setup(scanN, pulseN, Fs)
	left, right = twoChannels()

	r = np.fft.fft(left*hamming_window, n = nfft)
	rhalf = np.abs(r[0:nfft//2])

	data[currentRow] = rhalf
	image_plot.set_data(np.rot90(data))
	plt.draw()

	currentRow += 1
	if (currentRow >= plotRows):
		currentRow = currentRow - plotRows


	# plt.show()
	plt.pause(0.001)

