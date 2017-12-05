#include "pyfmcw.h"

int main(int argc, char *argv[]) {


	int scanN = 4096; 
	int pulseN = 2048; 
	unsigned int rate = 48000; 
	float thresh = 0.3; 

	// float * pulse = malloc(pulseN*sizeof(float)); 
	// float * reFFT = malloc(pulseN*sizeof(float)); 
	// float * imFFT = malloc(pulseN*sizeof(float)); 

	fmcw_setup(scanN,pulseN,rate); 
	fmcw_update_pulse(thresh); 
	fmcw_print_buffers();
	fmcw_shutdown();
	
	// int ret = 0; 
	// ret = fmcw_getframe(scanN, rate, pulse, reFFT, imFFT, pulseN);
	// fprintf(stdout,"fmcw_getframe: \t returned with code = %d\n",ret);


	// free(pulse); 

	return 0; 

}


