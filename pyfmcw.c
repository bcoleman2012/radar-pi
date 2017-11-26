/* 
Copyright 2017, Benjamin Ray Coleman, (benjamin.ray.coleman@gmail.com) All rights reserved.
No part of this program may be copied, reproduced, modified, translated, or reduced
in whole or in part in any manner without the prior written permission of the copyright owner. 
Any other reproduction in any form without permission is prohibited. 
*/

#include "pyfmcw.h"



int fmcw_getframe(unsigned int scanN, unsigned int rate, float *pulse, float *reFFT, float * imFFT, unsigned int pulseN)
{
	int ret; 
	if ( !(pulseN && !(pulseN & (pulseN - 1))) ) // https://stackoverflow.com/questions/3638431/determine-if-an-int-is-a-power-of-2-or-not-in-a-single-line
	{
		printf("Pulse length must be a power of two\n");
		return -1; 
	}
	int log2pulseN = log2(pulseN);
	int32_t *buffer; // buffer to store 2-channel interleaved audio line-in data
	unsigned int buffer_length = 2 * scanN * snd_pcm_format_width(SND_PCM_FORMAT_S32_LE) / 8; // size of interleaved buffer (Bytes)
	buffer = malloc(buffer_length); 
	// float buffers for GPU_FFT
	float *left = malloc(scanN*sizeof(float)); 
	float *right = malloc(scanN*sizeof(float)); 
	// float *pulse = malloc(pulseN*sizeof(float)); 
	// float buffers for output of direct IIR implementation (to fix annoying DC offset) 
	float *left_out = malloc(scanN*sizeof(float)); 
	float *right_out = malloc(scanN*sizeof(float)); 
	// Recording parameters 
	snd_pcm_t *capture_handle;




	// Set up the audio interface
	radar_record_setup(&capture_handle, &rate); 
	
	#ifdef _DEBUG_
	fprintf(stdout, "Audio interface set-up\n"); 
	#endif

	// Read from the radar into buffer
	ret = radar_record_readi(&capture_handle, buffer, scanN); 
	
	#ifdef _DEBUG_
	fprintf(stdout, "Read %d samples\n",scanN); 
	#endif

	if (ret != 0)
	{
		radar_record_shutdown(&capture_handle); 
		free(buffer);
		free(left);
		free(left_out);
		free(right);
		free(right_out);
		return ret; 
	}

	// Turn our interleaved int32_t buffer into two float buffers 
	radar_record_float_channels(buffer, left, right, scanN); 
	
	#ifdef _DEBUG_
	fprintf(stdout, "Channels converted to float buffers\n"); 
	#endif

	// high pass filter to get rid of the DC/low frequency offset 
	radar_proc_HPF( left, left_out, scanN ); 
	radar_proc_HPF( right, right_out, scanN ); 
	
	#ifdef _DEBUG_
	fprintf(stdout, "High-pass filtering complete\n"); 
	#endif
	
	// for (int j = 0; j < scanN; ++j)
	// {
	// 	fprintf (stdout, "%f %f\n",left_out[j],right_out[j]);
	// }

	// get the pulse 
	ret = radar_proc_parse_pulse(left_out, right_out, scanN, 0.3, pulse, pulseN); 
	
	#ifdef _DEBUG_
	fprintf(stdout, "Pulse taken, ret = %d.\n",ret); 
	#endif 

	if (ret != 0)
	{
		radar_record_shutdown(&capture_handle); 
		free(buffer);
		free(left);
		free(left_out);
		free(right);
		free(right_out);
		return ret; 
	}


	// do the FFT
	struct GPU_FFT_COMPLEX *base; 
	struct GPU_FFT *fft; 

	int mb = mbox_open();
	ret = gpu_fft_prepare(mb, 13, GPU_FFT_FWD, 1, &fft); 
	#ifdef _DEBUG_
	fprintf(stdout, "GPU_FFT prepped.\n"); 
	#endif

	switch(ret) {
		case -1: printf("Unable to enable V3D. Please check your firmware is up to date.\n"); return -1;
		case -2: printf("log2_N=%d not supported.  Try between 8 and 22.\n", 10);             return -1;
		case -3: printf("Out of memory.  Try a smaller batch or increase GPU memory.\n");     return -1;
		case -4: printf("Unable to map Videocore peripherals into ARM memory space.\n");      return -1;
		case -5: printf("Can't open libbcm_host.\n");                                         return -1;
	}

	// set up the fft struct to have the data from the right-hand channel  
	for (int i = 0; i < pulseN; ++i)
	{
		(fft->in)[i].re = right[i]; 
		(fft->in)[i].im = 0;
	}

	gpu_fft_execute(fft); 
	
	#ifdef _DEBUG_
	fprintf(stdout, "GPU_FFT executed.\n"); 
	#endif


	// return the FFT'd data 
	for (int i = 0; i < pulseN; ++i)
	{
		reFFT[i] = (fft->out)[i].re; 
		imFFT[i] = (fft->out)[i].im; 
	}

	gpu_fft_release(fft); // Videocore memory lost if not freed !
	
	#ifdef _DEBUG_
	fprintf(stdout, "GPU_FFT released.\n"); 
	#endif

	// De-allocate things 
	radar_record_shutdown(&capture_handle); 
	free(buffer);
	free(left);
	free(left_out);
	free(right);
	free(right_out);
	
	#ifdef _DEBUG_
	fprintf(stdout, "Buffers freed.\n"); 
	#endif

	return 0; 
}


