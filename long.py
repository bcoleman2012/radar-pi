from _pyfmcw import *
import matplotlib.pyplot as plt
import numpy as np
import scipy.signal
import scipy.ndimage

Fs = 33000 #48000
Tp = 10.0*10**-3

c = 3.0*10**8
fstart = 2361.0*10**6
fstop = 2657.0*10**6
BW = fstop-fstart
dfdt = BW/Tp; 

seconds = 0.5 # recording length, obviously in seconds


thresh = 0.8
frmax = 43
scanN = int(seconds*Fs)
pulseN = int(Fs*Tp/2.0)
sifN = int(pulseN/2)
hamming_window = scipy.signal.hamming(sifN)


Rmax = Fs/2.0*(c/(dfdt*4.0))
print("Rmax:",Rmax)
print("PulseN:",pulseN)
print("ScanN:",scanN)

NRuns = int(scanN/(pulseN))-1
sif = np.zeros((2*NRuns,sifN))



i = 0
while 2**i < sifN:
	i += 1
nfft = 2**(i+2)

f = np.linspace(0,Fs/2.0,nfft/2)
R = (c/(dfdt*4.0))*f
R_root = R**(1.5)

print("SIF Shape:",sif.shape)

# vmin = -15 when no FIR vertical mean'ing
plt.ion()
data = np.empty([2*NRuns-1,nfft/2])
image_plot = plt.imshow(data, vmin = -20, vmax = 0,
						aspect = 'auto', 
						extent=[0,10,seconds,0],
						cmap = 'viridis')#,vmin = -20)




#for i in range(1000): 
while True: 
	print("Doin the thing")
	setup(scanN, pulseN, Fs)
	trigger, signal = twoChannels()
	#shutdown()
	#print("Signal read")

	idx_prev = 0
	run = 0
	i = 0
	while i < (scanN - pulseN): 
		#print("Checking ",i)
		if trigger[i] > thresh: 
			if i - idx_prev > 10: 
				# if here, we have a pulse
				sif[run,:] = signal[i:i+sifN]*hamming_window
				run += 1
				sif[run,:] = signal[i+sifN:i+2*sifN]*hamming_window
				run += 1
				i = i + pulseN
			else: 
				idx_prev = i
				i += 1
			if run >= 2*NRuns: 
				break
		else: 
			i += 1
	
	#print("2")
	f = np.linspace(0,Fs/2.0,nfft/2)
	R = (c/(dfdt*4.0))*f
	#print("3")
	fr = (sif[0:NRuns-2,:] + sif[1:NRuns-1,:])*0.5

	r = np.fft.fft(fr, n = nfft)

	rdb = 20*np.log10(1+np.abs((r[:,0:nfft//2])*R_root))
	#print("4")	
	#frmax = np.max(rdb)
	#print(frmax)
	rclutter_reduced = rdb-frmax
	rclutter_reduced = scipy.ndimage.uniform_filter1d(rclutter_reduced, 4, axis = 1)

	#plt.imshow(rclutter_reduced, aspect = 'auto', vmin = -40)
	#plt.colorbar()
	#plt.show()

	image_plot.set_data(rclutter_reduced)
	plt.draw()
	plt.pause(0.01)
























'''
plt.ion()
#rectUp = plt.axvspan(0, pulseN, ymin=-1, ymax=1, alpha = 0.2, color = 'b')
#rectDown = plt.axvspan(0, pulseN, ymin=-1, ymax=1, alpha = 0.2, color = 'purple')
#lr, = plt.plot(trigger)
#ll, = plt.plot(signal)
#plt.ylim(-1.0,1.0)

plt.figure()
plotRows = 80
data = np.empty([plotRows,nfft/2])
image_plot = plt.imshow(data, vmin = -15, vmax = 0,
						aspect = 'auto', 
						extent=[0,20,plotRows,0],
						cmap = 'plasma')#,vmin = -20)
#plt.axis('off')
plt.xlabel("Range (m)")
plt.ylabel("Time [n]")
plt.colorbar()
currentRow = 0
plt.show()

max_val = 1

while True: 
	setup(scanN, pulseN, Fs)
	# p = rawPulse(0.5)
	#ln.set_ydata(p)
	trigger, signal = twoChannels()
	trigger = np.abs(trigger)
	#lr.set_ydata(trigger)
	#ll.set_ydata(signal)
	#plt.draw()


	# np.fft.fft(sr)
	# Find the sif pulses 
	idx_prev = 0
	for i in range(scanN): 
		if trigger[i] > thresh: 
			if i - idx_prev > 5: 
				# pulse = signal[i:i+pulseN]
				break
			idx_prev = i
	# rectW.set_bounds(i, -1, pulseN, 2)# (left, bottom, width, height)
	# print(type(rectW))
	#coords = ((i+sifN,-1),(i+sifN,1),(i+2*sifN,1),(i+2*sifN,-1)) #i+pulseN),(-1,1,1,1)
	#rectUp.set_xy(coords)
	#coords = ((i+3*sifN,-1),(i+3*sifN,1),(i+4*sifN,1),(i+4*sifN,-1))
	#rectDown.set_xy(coords)

	try: 
		# print("ScanN: ",scanN)
		# print(i+sifN,i+2*sifN)
		# print(i+3*sifN,i+4*sifN)

		sifUp = signal[i+sifN:i+2*sifN]*hamming_window
		sifDown = signal[i+sifN:i+2*sifN]*hamming_window
	except: 
		print("Insufficient scan period. This could be due to a chance occurrance, but if it occurs enough, increase ScanN")
		continue

	# fr = (sif[0:NRuns-2,:] + sif[1:NRuns-1,:])*0.5
	fr = (sifUp + sifDown)*0.5
	# print("made it.") np.pad(A, (0, 1024 - len(A)%1024), 'constant')
	# SIFUP_FFT = numpy.fft.fft(sifUp, n=sifPadN)
	# SIFDOWN_FFT = numpy.fft.fft(sifUp, n=sifPadN)
	r = np.fft.fft(fr, n = nfft)
	rhalf = np.abs(r[0:nfft//2])

	data[currentRow] = rhalf*R_root #.^(2.5/2)
	max_val = np.max(data)

	# plt.figure()
	# plt.plot(np.abs(r))
	# plt.plot(np.abs(rhalf))
	

	# data[currentRow] = rdb[0:pulseN/2]
	framerow = 20*np.log10(data) - 20*np.log10(max_val)
	image_plot.set_data(framerow)
	plt.draw()

	currentRow += 1
	if (currentRow >= plotRows):
		currentRow = currentRow - plotRows


	# plt.show()
	plt.pause(0.001)

'''




