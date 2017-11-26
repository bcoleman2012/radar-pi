/* 
Copyright 2017, Benjamin Ray Coleman, (benjamin.ray.coleman@gmail.com) All rights reserved.
No part of this program may be copied, reproduced, modified, translated, or reduced
in whole or in part in any manner without the prior written permission of the copyright owner. 
Any other reproduction in any form without permission is prohibited. 
*/

#include <alsa/asoundlib.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include "radar_record.h"
#include "radar_proc.h"

#include "mailbox.h"
#include "gpu_fft.h"


#ifndef __PYFMCW__
#define __PYFMCW__


int fmcw_getframe(unsigned int scanN, unsigned int rate, float *pulse, float *reFFT, float * imFFT, unsigned int pulseN);


#endif // __RADAR_RECORD__