/* 
Copyright 2017, Benjamin Ray Coleman, (benjamin.ray.coleman@gmail.com) All rights reserved.
No part of this program may be copied, reproduced, modified, translated, or reduced
in whole or in part in any manner without the prior written permission of the copyright owner. 
Any other reproduction in any form without permission is prohibited. 
*/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <alsa/asoundlib.h>

#include "mailbox.h"
#include "gpu_fft.h"

/* This library contains all of the radar proccessing steps. 


*/

#ifndef __RADAR_PROC__
#define __RADAR_PROC__


// int radar_proc_fft();


// radar_proc_fft()

// radar_proc_remove_dc

// radar_proc_hamming

// Turns a signal into a pulse 
int radar_proc_parse_pulse(
	float * trigger, // input float buffers - trigger (might be longer than the radar pulse)
	float * signal, // input float buffers - signal (might be longer than the radar pulse)
	int N, // length of left, right buffers
	float thresh, // trigger threshold
	float * pulse, // float buffer with length signalN 
	int pulseN // number of samples in signal
	);

// High pass filter with coefficients: 
// B =  [ 0.97803048 -1.95606096  0.97803048]
// A =  [ 1.         -1.95557824  0.95654368]
int radar_proc_HPF(
	float * in, 
	float * out, 
	int N
	);

#endif // __RADAR_PROC__