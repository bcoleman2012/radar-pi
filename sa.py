import _pyfmcw as radar
import numpy as np
import matplotlib.pyplot as plt
import time

Fs = 48000
scanN = 4200 #4096
pulseN = 1024

plotRows = 80
data = np.zeros([plotRows,pulseN/2])

c = 3.0*10**8
Tp = 40.0*10**-3
fstart = 2361*10**6
fstop = 2657*10**6
BW = fstop-fstart
dfdt = BW/Tp; 


f = np.arange(0, pulseN/2, Fs/2.0)
R = (c/(dfdt*4.0))*f

print("Plot rows:"+str(plotRows))

i = 0
while (i < plotRows):
	try: 
		pulse, reFFT, imFFT = radar.getFrame(scanN, pulseN, Fs)
	except: 
		print("getFrame failed to, er, get frame. Trying again.")
		continue
	magFFT = np.sqrt(reFFT**2 + imFFT**2)
	rdb = 20*np.log10(np.abs(magFFT))
	data[i] = rdb[0:pulseN/2]
	i = i + 1
	# time.sleep(0.001)


image_plot = plt.imshow(data, aspect = 'auto', cmap = 'jet',vmin = -20)

currentRow = 0

while True: 
	try: 
		try: 
			pulse, reFFT, imFFT = radar.getFrame(scanN, pulseN, Fs)
		except: 
			print("getFrame failed to, er, get frame. Trying again.")
			continue

		magFFT = np.sqrt(reFFT**2 + imFFT**2)
		rdb = 20*np.log10(np.abs(magFFT))
		data[currentRow] = rdb[0:pulseN/2]

		image_plot.set_data(data)
		plt.draw()

		currentRow += 1
		if (currentRow >= plotRows):
			currentRow = currentRow - plotRows
		plt.pause(0.001)

	except KeyboardInterrupt: 
		break

plt.ioff()
