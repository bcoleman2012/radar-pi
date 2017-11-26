S = hex/shader_256.hex \
    hex/shader_512.hex \
    hex/shader_1k.hex \
    hex/shader_2k.hex \
    hex/shader_4k.hex \
    hex/shader_8k.hex \
    hex/shader_16k.hex \
    hex/shader_32k.hex \
    hex/shader_64k.hex \
    hex/shader_128k.hex \
    hex/shader_256k.hex \
    hex/shader_512k.hex \
    hex/shader_1024k.hex \
    hex/shader_2048k.hex \
    hex/shader_4096k.hex


# CC = gcc
CC = arm-linux-gnueabihf-gcc

CFLAGS = -std=c99
LCFLAGS = -lasound -lrt -lm -ldl
C_OBJ = radar_proc.o radar_record.o pyfmcw.o main.o

C_INC = -I/usr/include/python3.4

TARGET = main

GPU_DEPS = gpu_fft.o gpu_fft_shaders.o gpu_fft_twiddles.o gpu_fft_base.o mailbox.o

.PHONY: all clean

all: radar_proc.o radar_record.o main.o $(GPU_DEPS)
	$(CC) $(GPU_DEPS) $(C_OBJ) -o $(TARGET) $(LCFLAGS)

python: 
	python setup.py build_ext --inplace


radar_proc.o: radar_proc.c radar_proc.h 
	$(CC) -c radar_proc.c -o radar_proc.o $(CFLAGS) $(LCFLAGS)

radar_record.o: radar_record.c radar_record.h 
	$(CC) -c radar_record.c -o radar_record.o $(CFLAGS) $(LCFLAGS) 

pyfmcw.o: pyfmcw.c pyfmcw.h 
	$(CC) -c pyfmcw.c -o pyfmcw.o $(CFLAGS) $(LCFLAGS)

# mailbox.c gpu_fft.c gpu_fft_base.c gpu_fft_twiddles.c gpu_fft_shaders.c

mailbox.o: mailbox.c 
	$(CC) -c mailbox.c -o mailbox.o 
gpu_fft_base.o: gpu_fft_base.c 
	$(CC) -c gpu_fft_base.c -o gpu_fft_base.o 
gpu_fft_twiddles.o: gpu_fft_twiddles.c 
	$(CC) -c gpu_fft_twiddles.c -o gpu_fft_twiddles.o 
gpu_fft_shaders.o: gpu_fft_shaders.c 
	$(CC) -c gpu_fft_shaders.c -o gpu_fft_shaders.o 

main.o: main.c radar_proc.o radar_record.o pyfmcw.o
	$(CC) -c main.c -o main.o $(C_INC) $(CFLAGS) $(LCFLAGS)

clean: 
	rm *.o || true
	rm *.so || true
	rm $(TARGET) || true
	rm -rf build
