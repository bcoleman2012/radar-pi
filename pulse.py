from _pyfmcw import *

Fs = 48000.0
Tp = 40.0*10**-3
c = 3.0*10**8
pulseN = int(Fs*Tp/2.0)

setup(Fs)
rawPulse()

