/* 
Copyright 2017, Benjamin Ray Coleman, (benjamin.ray.coleman@gmail.com) All rights reserved.
No part of this program may be copied, reproduced, modified, translated, or reduced
in whole or in part in any manner without the prior written permission of the copyright owner. 
Any other reproduction in any form without permission is prohibited. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <alsa/asoundlib.h>



#ifndef __RADAR_RECORD__
#define __RADAR_RECORD__


int radar_record_setup(
	snd_pcm_t **capture_handle, // Returned so that you can call other functions 
	unsigned int *Fs // input the desired Fs, and this will return with the actual Fs 
	); 
// Usage: 
// unsigned int rate = 48000; 
// snd_pcm_t *capture_handle; // <alsa/asoundlib.h>
// radar_record_setup(&rate, &capture_handle);
// now use capture_handle with the others 


int radar_record_readi(
	snd_pcm_t **capture_handle, 
	int32_t * buf, // MUST ALREADY BE ALLOCATED!
	int N // Number of samples. Equal to length of *buf / 2  
	); 

int radar_record_shutdown(
	snd_pcm_t **capture_handle
	); 

// Turns an int32_t buffer into two sets of float buffers, for L and R channels
int radar_record_float_channels( 
	int32_t * buf,
	float * left, 
	float * right, 
	int N // number of samples. Equal to size of left and right. Number in Buf is 2*N
	);

#endif // __RADAR_RECORD__