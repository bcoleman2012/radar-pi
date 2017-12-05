/* 
Copyright 2017, Benjamin Ray Coleman, (benjamin.ray.coleman@gmail.com) All rights reserved.
No part of this program may be copied, reproduced, modified, translated, or reduced
in whole or in part in any manner without the prior written permission of the copyright owner. 
Any other reproduction in any form without permission is prohibited. 
*/

#include "pyfmcw.h"


static struct audio_interface RADAR_INTERFACE = audio_interface_default;
static struct channel_data RADAR_RAW_CHANNELS = channel_data_default; 
static struct channel_data RADAR_CHANNELS = channel_data_default; 
static struct pulse_data RADAR_PULSE = pulse_data_default; 




int fmcw_setup(unsigned int scanN, unsigned int pulseN, unsigned int rate)
{
	unsigned int buffer_length = 2 * scanN * snd_pcm_format_width(SND_PCM_FORMAT_S32_LE) / 8; // size of interleaved buffer (Bytes)

	fmcw_shutdown(); 

	RADAR_INTERFACE.rate = rate; 
	radar_record_setup(&(RADAR_INTERFACE.capture_handle), &(RADAR_INTERFACE.rate) ); 

	RADAR_INTERFACE.N = scanN; 
	RADAR_INTERFACE.buffer = malloc(buffer_length); 

	// Set up the radar channels

	RADAR_CHANNELS.N = scanN; 
	RADAR_CHANNELS.left = malloc(scanN*sizeof(float)); 
	RADAR_CHANNELS.right = malloc(scanN*sizeof(float));

	RADAR_RAW_CHANNELS.N = scanN; 
	RADAR_RAW_CHANNELS.left = malloc(scanN*sizeof(float)); 
	RADAR_RAW_CHANNELS.right = malloc(scanN*sizeof(float));

	if (RADAR_PULSE.pulse != NULL)
		free(RADAR_PULSE.pulse);

	RADAR_PULSE.N = pulseN; 
	RADAR_PULSE.pulse = malloc(pulseN*sizeof(float)); 

	fprintf(stdout, "Audio interface set-up\n"); 

	return 0; 

}


int fmcw_shutdown()
{
	if (RADAR_INTERFACE.capture_handle != NULL)
		radar_record_shutdown(&capture_handle);
	if (RADAR_INTERFACE.buffer != NULL)
		free(RADAR_INTERFACE.buffer);

	if (RADAR_RAW_CHANNELS.left != NULL)
		free(RADAR_RAW_CHANNELS.left);
	if (RADAR_RAW_CHANNELS.left != NULL)
		free(RADAR_RAW_CHANNELS.right);

	if (RADAR_CHANNELS.left != NULL)
		free(RADAR_CHANNELS.left);
	if (RADAR_CHANNELS.left != NULL)
		free(RADAR_CHANNELS.right);
	
	if (RADAR_PULSE.pulse != NULL)
		free(RADAR_PULSE.pulse);

	RADAR_INTERFACE.rate = 0; 
	RADAR_INTERFACE.N = 0; 
	RADAR_CHANNELS.N = 0; 
	RADAR_PULSE.N = 0; 
	return 0; 
}

int fmcw_update_pulse(float thresh)
{
	int ret; 
	int scanN = RADAR_INTERFACE.N;
	int pulseN = RADAR_PULSE.N; 
	ret = radar_record_readi(&(RADAR_INTERFACE.capture_handle), RADAR_INTERFACE.capture_handle, scanN); 
	
	if (ret != 0)
		return ret; 

	fprintf(stdout, "Read %d samples\n",scanN); 

	// Turn our interleaved int32_t buffer into two float buffers 
	radar_record_float_channels(RADAR_INTERFACE.buffer, RADAR_RAW_CHANNELS.left, RADAR_RAW_CHANNELS.right, scanN); 
	
	fprintf(stdout, "Channels converted to float buffers\n"); 

	// high pass filter to get rid of the DC/low frequency offset 
	radar_proc_HPF( RADAR_RAW_CHANNELS.left, RADAR_CHANNELS.left, scanN ); 
	radar_proc_HPF( RADAR_RAW_CHANNELS.right, RADAR_CHANNELS.right, scanN ); 

	fprintf(stdout, "High-pass filtering complete\n"); 
	
	// get the pulse 
	ret = radar_proc_parse_pulse(RADAR_CHANNELS.left, RADAR_CHANNELS.right, scanN, thresh, RADAR_PULSE.pulse, pulseN); 
	
	fprintf(stdout, "Pulse taken, ret = %d.\n",ret); 

	return ret; 
}

int fmcw_print_buffers()
{
	int scanN = RADAR_INTERFACE.N;
	int pulseN = RADAR_PULSE.N; 
	fprintf(stdout, "RADAR_CHANNELS\nLEFT\tRIGHT\n");
	for (int i = 0; i < scanN; ++i)
	{
		fprintf (stdout, "%f\t%f\n",RADAR_CHANNELS.left[i],RADAR_CHANNELS.right[i]);
	}

	fprintf(stdout, "RADAR_PULSE");
	for (int i = 0; i < scanN; ++i)
	{
		fprintf (stdout, "%f\n",RADAR_PULSE.pulse[i]);
	}

	return 0; 
}


























































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













