#include "pyfmcw.h"

int main(int argc, char *argv[]) {


	int scanN = 4096; 
	int pulseN = 2048; 
	unsigned int rate = 48000; 

	float * pulse = malloc(pulseN*sizeof(float)); 
	float * reFFT = malloc(pulseN*sizeof(float)); 
	float * imFFT = malloc(pulseN*sizeof(float)); 

	int ret = 0; 
	ret = fmcw_getframe(scanN, rate, pulse, reFFT, imFFT, pulseN);
	fprintf(stdout,"fmcw_getframe: \t returned with code = %d\n",ret);

	// for (int j = 0; j < pulseN; ++j)
	// {
	// 	fprintf (stdout, "%f %f\n",pulse[j],pulse[j]);
	// }


	free(pulse); 

	return 0; 

}


