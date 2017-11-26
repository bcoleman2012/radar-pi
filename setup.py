from distutils.core import setup, Extension
import numpy.distutils.misc_util

setup(
	ext_modules=[Extension(
		"_pyfmcw",
		[
		 "_pyfmcw.c","pyfmcw.c",
		 "radar_record.c","radar_proc.c",
		 "mailbox.c", "gpu_fft.c", "gpu_fft_base.c", "gpu_fft_twiddles.c", "gpu_fft_shaders.c"
		],
		extra_compile_args = ["-std=c99"], 
		# libraries = ["asound","t","m","dl"]
		extra_link_args = ["-lasound","-lrt","-lm","-ldl"]
		)], 
	include_dirs=[numpy.distutils.misc_util.get_numpy_include_dirs(),"/usr/include/python3.4"], 
	# include_dirs=["/usr/include/python3.4"], 
)