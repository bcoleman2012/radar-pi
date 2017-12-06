#define NPY_NO_DEPRECATED_API NPY_1_9_API_VERSION
#include <Python.h>
#include <numpy/arrayobject.h>
//http://dfm.io/posts/python-c-extensions/
//http://adamlamers.com/post/NUBSPFQJ50J1
//http://acooke.org/cute/ExampleCod0.html
#include "pyfmcw.h"

static char module_docstring[] =
	"This module provides an interface for an FMCW radar on a Linux platform, using a sound card that supports the S32_LE format";
static char getFrame_docstring[] =
	"Get the next frame of the radar image (performs FFT on the Raspberry Pi GPU)\nAguments: scanN, pulseN, rate\nReturns: pulse, reFFT, imFFT";


// int fmcw_getframe(unsigned int scanN, unsigned int rate, float *pulse, float *reFFT, float * imFFT, unsigned int pulseN);

// int fmcw_setup(unsigned int scanN, unsigned int pulseN, unsigned int rate); 
// int fmcw_shutdown(); 
// int fmcw_update_pulse(float thresh); 
// int fmcw_print_buffers();

// Module Method Declarations  
static PyObject* _pyfmcw_test(PyObject *self, PyObject *args);
static PyObject *_pyfmcw_getFrame(PyObject *self, PyObject *args);

static PyObject *_pyfmcw_setup(PyObject *self, PyObject *args);
static PyObject *_pyfmcw_shutdown(PyObject *self, PyObject *args);
static PyObject *_pyfmcw_rawPulse(PyObject *self, PyObject *args);
static PyObject* _pyfmcw_twoChannels(PyObject *self, PyObject *args); 


static PyMethodDef pyfmcw_module_methods[] = 
{
	{"getFFTFrame", _pyfmcw_getFrame, METH_VARARGS, getFrame_docstring},
	{"test", _pyfmcw_test, METH_VARARGS, "docstring"},
	{"setup", _pyfmcw_setup, METH_VARARGS, "docstring"},
	{"shutdown", _pyfmcw_shutdown, METH_VARARGS, "docstring"},
	{"rawPulse", _pyfmcw_rawPulse, METH_VARARGS, "docstring"},
	{"twoChannels", _pyfmcw_twoChannels, METH_VARARGS, "docstring"},
	{NULL, NULL, 0, NULL}
};

// Module Definition 
// The arguments of this structure tell Python what to call your extension,
// what it's methods are and where to look for it's method definitions
static struct PyModuleDef pyfmcw_module_definition = {
	PyModuleDef_HEAD_INIT, 
	"_pyfmcw",
	"usage: (Not yet. Sorry)",
	-1,
	pyfmcw_module_methods
};

// Module initialization
// Python calls this function when importing your extension. It is important
// that this function is named PyInit_[[your_module_name]] exactly, and matches
// the name keyword argument in setup.py's setup() call.
PyMODINIT_FUNC PyInit__pyfmcw(void)
{
	PyObject * module = PyModule_Create(&pyfmcw_module_definition);
	import_array(); 
	return module; 
}

// Module Method Definitions 

static PyObject *_pyfmcw_getFrame(PyObject *self, PyObject *args)
{
	// fmcw_getframe(unsigned int scanN, unsigned int rate, float *pulse, float *reFFT, float * imFFT, unsigned int pulseN)
	// PyObject *pulse_obj, *reFFt, *imFFT; 
	// printf("Inside _pyfmcw_getFrame\n");
	unsigned int scanN, rate, pulseN; 
	// printf("Created scanN, rate, and pulseN\n");

	
	// Parse the input tuple
	if (!PyArg_ParseTuple(args, "III", &scanN, &pulseN, &rate))
		return NULL; 

	// printf("_pyfmcw_getFrame: \t Input Tuple Parsed\n");
	float *pulse = malloc(pulseN*sizeof(float)); 
	float *reFFT = malloc(pulseN*sizeof(float)); 
	float *imFFT = malloc(pulseN*sizeof(float)); 

	int ret = fmcw_getframe(scanN, rate, pulse, reFFT, imFFT, pulseN); 
	// fprintf(stdout,"fmcw_getframe: \t returned with code = %d\n",ret);
	if (ret != 0)
	{
		Py_RETURN_NONE; 		
	}

	// printf("_pyfmcw_getFrame: \t Internal C-Function has successfully returned\n");
	// fprintf(stdout, "_pyfmcw_getFrame: \t sizeof(float) = %d\n",sizeof(float));
	
	npy_intp dims[1] = {pulseN};
	// woah baby! Lotta segfaults. This has been a PSA: Check your pointers, people!
	PyObject * pulse_ret = PyArray_SimpleNewFromData(1,dims, NPY_FLOAT, pulse);
	PyObject * reFFT_ret = PyArray_SimpleNewFromData(1,dims, NPY_FLOAT, reFFT);
	PyObject * imFFT_ret = PyArray_SimpleNewFromData(1,dims, NPY_FLOAT, imFFT);
	PyArray_ENABLEFLAGS((PyArrayObject*)pulse_ret, NPY_ARRAY_OWNDATA);
	PyArray_ENABLEFLAGS((PyArrayObject*)reFFT_ret, NPY_ARRAY_OWNDATA);
	PyArray_ENABLEFLAGS((PyArrayObject*)imFFT_ret, NPY_ARRAY_OWNDATA);

	return Py_BuildValue("OOO", pulse_ret,reFFT_ret,imFFT_ret);
	// Py_RETURN_NONE;
}


static PyObject* _pyfmcw_test(PyObject *self, PyObject *args)
{
	// npy_intp dims[1] = {10};
	// int len = dims[0]; 
	// float* tstarr = malloc(len*sizeof(float));
	// for (int i = 0; i < len; ++i)
	// 	tstarr[i] = i; 

	// PyObject * ret = PyArray_SimpleNewFromData(1,dims, NPY_FLOAT, tstarr);
	// PyArray_ENABLEFLAGS((PyArrayObject*)ret, NPY_ARRAY_OWNDATA);
	// return ret;

	unsigned int scanN = 4200; 
	unsigned int pulseN = 1024; 
	unsigned int rate = 48000; 

	// fmcw_setup(scanN, pulseN, rate); 
	fmcw_update_pulse(0.3); 
	// fmcw_shutdown();
	Py_RETURN_NONE; 
}


static PyObject* _pyfmcw_setup(PyObject *self, PyObject *args)
{
	unsigned int scanN, rate, pulseN; 
	if (!PyArg_ParseTuple(args, "III", &scanN, &pulseN, &rate))
		return NULL; 
	fmcw_setup(scanN, pulseN, rate); 
	Py_RETURN_NONE; 
}
static PyObject* _pyfmcw_shutdown(PyObject *self, PyObject *args)
{
	fmcw_shutdown(); 
	Py_RETURN_NONE; 
}
static PyObject* _pyfmcw_rawPulse(PyObject *self, PyObject *args)
{

	float thresh; 
	float *pulse = NULL; 
	fprintf(stdout, "RAWPULSE: NULL pulse = %d\n", pulse);
	float *ret_buffer = NULL; 
	unsigned int size; 
	
	if (!PyArg_ParseTuple(args, "f", &thresh))
		return NULL; 

	fmcw_update_pulse(thresh);
	fmcw_buffer_access(&pulse,&size);
	fprintf(stdout, "RAWPULSE: pulse = %d\n", pulse);

	// ret_buffer = malloc(size*sizeof(float)); 
	// for (int i = 0; i < size; ++i)
	// 	ret_buffer[i] = pulse[i];

	npy_intp dims[1] = {size};
	PyObject * pulse_ret = PyArray_SimpleNewFromData(1,dims, NPY_FLOAT, pulse);
	// PyArray_ENABLEFLAGS((PyArrayObject*)pulse_ret, NPY_ARRAY_OWNDATA);
	return pulse_ret; 
}

static PyObject* _pyfmcw_twoChannels(PyObject *self, PyObject *args)
{
	float *left = NULL; 
	float *right = NULL;
	fmcw_two_channels(&left, &right); 

	unsigned int size = fmcw_channel_size(); 
	npy_intp dims[1] = {size};
	PyObject * left_ret = PyArray_SimpleNewFromData(1, dims, NPY_FLOAT, left);
	PyObject * right_ret = PyArray_SimpleNewFromData(1, dims, NPY_FLOAT, right);

	return Py_BuildValue("OO", left_ret,right_ret);

}



// int fmcw_setup(unsigned int scanN, unsigned int pulseN, unsigned int rate); 
// int fmcw_shutdown(); 
// int fmcw_update_pulse(float thresh); 
// int fmcw_print_buffers();



