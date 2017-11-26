/* 
Copyright 2017, Benjamin Ray Coleman, (benjamin.ray.coleman@gmail.com) All rights reserved.
No part of this program may be copied, reproduced, modified, translated, or reduced
in whole or in part in any manner without the prior written permission of the copyright owner. 
Any other reproduction in any form without permission is prohibited. 
*/

#include "radar_proc.h"


// int radar_proc_fft();

// turns a signal and a trigger into a pulse
// pulseN can be computed from the pulse time and sampling rate
// returns -1 if a pulse was not found 
// returns -2 if the entire pulse was not recorded
int radar_proc_parse_pulse( float * trigger, float * signal, int N, float thresh, float * pulse, int pulseN)
{
	int start_index = -1; 
	for (int i = 0; i < N; ++i)
	{
		if (trigger[i] > thresh) // we found a pulse!
		{
			start_index = i; 
			break; // break out of the loop 
		}
	}

	// check that we found a start index 
	if (start_index == -1)
		return -1; 
	// check that the entire pulse has been recorded 
	else if (start_index + pulseN > N)
		return -2; 

	// copy over the pulse into *pulse 
	for (int i = 0; i < pulseN; ++i)
	{
		pulse[i] = signal[start_index+i]; 
	}

	return 0; 
}


int radar_proc_HPF( float * in, float *out, int N )
{
	if (N < 3)
		return -1; 

	// N is number of samples in *in or *out 
	float B0 = 0.97803048;
	float B1 = -1.95606096; 
	float B2 = 0.97803048; 
	// float A0 = 1.0; 
	float A1 = -1.95557824; 
	float A2 = 0.95654368; 

	// y[n] = B0*x[n] + B1*x[n-1] + B2*x[n-2] + B3*x[n-3] - A1*y[n-1] - A2*y[n-2]
	out[0] = B0*(in[0]);
	out[1] = B0*(in[1]) + B1*(in[0]) - A1*(out[0]);

	for (int i = 2; i < N; ++i)
		out[i] = B0*(in[i]) + B1*(in[i-1]) + B2*(in[i-2]) - A1*(out[i-1]) - A2*(out[i-2]); 

	return 0; 
}




