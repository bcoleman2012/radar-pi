/* 
Copyright 2017, Benjamin Ray Coleman, (benjamin.ray.coleman@gmail.com) All rights reserved.
No part of this program may be copied, reproduced, modified, translated, or reduced
in whole or in part in any manner without the prior written permission of the copyright owner. 
Any other reproduction in any form without permission is prohibited. 
*/
#include "radar_record.h"

int radar_record_setup(snd_pcm_t **capture_handle,  unsigned int *Fs )
{
	int i;
	int err;
	snd_pcm_hw_params_t *hw_params; // parameters for reading
	snd_pcm_format_t format = SND_PCM_FORMAT_S32_LE; // this is the "correct" format
	// fprintf(stdout, "Size of a buffer chunk: %d bits \n",snd_pcm_format_width(format));
	// fprintf(stdout, "Size of int32_t: %d bits \n",8*sizeof(int32_t));

	if ((err = snd_pcm_open (capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0)) < 0) { return -1; }

	#ifdef _DEBUG_
	fprintf(stdout, "Audio interface opened.\n");
	#endif 

	// allocate an invalid snd_pcm_hw_params_t using standard malloc
	// input parameter: ** snd_pcm_hw_params_t (pointer to pointer to the params struct) 
	// output parameter: error code, 0 on success
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) { return -1; }

	#ifdef _DEBUG_
	fprintf(stdout, "hw_params allocated.\n");
	#endif 
	
	// Fill params with a full configuration space for a PCM.
	// Parameters:
	// pcm	PCM handle (capture_handle)
	// params	Configuration space (hw_params)
	// The configuration space will be filled with all possible ranges for the PCM device.
	if ((err = snd_pcm_hw_params_any (*capture_handle, hw_params)) < 0) { return -1; }

	#ifdef _DEBUG_
	fprintf(stdout, "hw_params initialized.\n");
	#endif 

	// Restrict a configuration space to contain only one access type.
	// Parameters:
	// pcm	PCM handle
	// params	Configuration space
	// access	access type
	// Returns:
	// 0 otherwise a negative error code if configuration space would become empty
	if ((err = snd_pcm_hw_params_set_access (*capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) { return -1; }

	#ifdef _DEBUG_
	fprintf(stdout, "hw_params access set.\n");
	#endif

	if ((err = snd_pcm_hw_params_set_format (*capture_handle, hw_params, format)) < 0) { return -1; }

	#ifdef _DEBUG_
	fprintf(stdout, "hw_params format set.\n");
	#endif 

	if ((err = snd_pcm_hw_params_set_rate_near (*capture_handle, hw_params, Fs, 0)) < 0) { return -1; }

	// Set us up to read only 2 channels - This is important so we don't fuck up the 
	// buffer snd_pcm_readn later!!!
	#ifdef _DEBUG_
	fprintf(stdout, "hw_params rate set.\n");
	#endif

	if ((err = snd_pcm_hw_params_set_channels (*capture_handle, hw_params, 2)) < 0) { return -1; }

	#ifdef _DEBUG_
	fprintf(stdout, "hw_params channels set.\n");
	#endif

	if ((err = snd_pcm_hw_params (*capture_handle, hw_params)) < 0) { return -1; }

	#ifdef _DEBUG_
	fprintf(stdout, "hw_params set.\n");
	#endif

	snd_pcm_hw_params_free (hw_params);

	#ifdef _DEBUG_
	fprintf(stdout, "hw_params freed.\n");
	#endif

	if ((err = snd_pcm_prepare (*capture_handle)) < 0) { return -1; }

	#ifdef _DEBUG_
	fprintf(stdout, "audio interface prepared.\n");
	#endif

	return 0; 

}

int radar_record_readi( snd_pcm_t **capture_handle, int32_t * buf, int N )
{
	int err = 0; 
	int ret = snd_pcm_readi (*capture_handle, buf, N);
	if (ret != N) {
		fprintf(stdout, "radar_record_readi: read %d bytes from %d, not %d \n",ret, buf, N);
		fprintf (stderr, "read from audio interface failed (%s)\n",
						 err, snd_strerror (err));
		return -1; 
	}
	
	#ifdef _DEBUG_
	fprintf(stdout, "Read %d samples into buffer.\n", N);
	#endif 

	return 0; 

}

int radar_record_shutdown(snd_pcm_t **capture_handle)
{
	snd_pcm_close (*capture_handle);
	
	#ifdef _DEBUG_
	fprintf(stdout, "audio interface closed\n");
	#endif 
	
	return 0; 
} 

int radar_record_float_channels( int32_t * buf, float * left, float * right, int N )
{
	int32_t maxL = 1; 
	int32_t maxR = 1; 

	float fmaxL = 1.0; 
	float fmaxR = 1.0; 

	// Find the max in each channel, so that we space 
	// the floats between -1 and +1
	for (int i = 0; i < N; ++i )
	{
		if (abs(buf[2*i]) > maxL)
			maxL = abs(buf[2*i]);
		if (abs(buf[2*i+1]) > maxR)
			maxR = abs(buf[2*i+1]);
		// if (abs(buf[i]) > max)
		// {
		// 	max = abs(buf[i]);
		// }
	}

	fmaxL = (float)maxL;
	fmaxR = (float)maxR;
	// fmax = (float)max;

	for (int i = 0; i < N; ++i)
	{
		// left
		left[i] = (float)buf[2*i]/fmaxL;

		// right
		right[i] = (float)buf[2*i+1]/fmaxR;
	}

	return 0; 
}







