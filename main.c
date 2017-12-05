#include "pyfmcw.h"

int main(int argc, char *argv[]) 
{

	int scanN = 4096; 
	int pulseN = 1024; 
	unsigned int rate = 48000; 
	float thresh = 0.3; 

	// float * pulse = malloc(pulseN*sizeof(float)); 
	// float * reFFT = malloc(pulseN*sizeof(float)); 
	// float * imFFT = malloc(pulseN*sizeof(float)); 

	fmcw_setup(scanN,pulseN,rate); 
	fmcw_setup(scanN,pulseN,rate); 

	fprintf(stdout, "Read 1\n");
	fmcw_update_pulse(thresh); 
	fmcw_update_pulse(thresh); 
	fmcw_update_pulse(thresh); 
	fmcw_update_pulse(thresh); 
	fprintf(stdout, "Read 2\n");
	fmcw_update_pulse(thresh); 
	fprintf(stdout, "Read 3\n");
	fmcw_update_pulse(thresh); 


	float *pulse = NULL; 
	unsigned int size; 
	fmcw_buffer_access(&pulse,&size);
	fprintf(stdout, "RawPulse: pulse at %d with length %d\n", pulse,size);

	// fmcw_print_buffers();
	fmcw_shutdown();
	
	// int ret = 0; 
	// ret = fmcw_getframe(scanN, rate, pulse, reFFT, imFFT, pulseN);
	// fprintf(stdout,"fmcw_getframe: \t returned with code = %d\n",ret);


	// free(pulse); 

	return 0; 

}
