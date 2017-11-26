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

// Module Method Declarations  
static PyObject *_pyfmcw_getFrame(PyObject *self, PyObject *args);

static PyObject* _pyfmcw_test(PyObject *self, PyObject *args)
{
	npy_intp dims[1] = {10};
	int len = dims[0]; 
	float* tstarr = malloc(len*sizeof(float));
	for (int i = 0; i < len; ++i)
		tstarr[i] = i; 

	PyObject * ret = PyArray_SimpleNewFromData(1,dims, NPY_FLOAT, tstarr);
	PyArray_ENABLEFLAGS((PyArrayObject*)ret, NPY_ARRAY_OWNDATA);
	return ret; 
}

static PyMethodDef pyfmcw_module_methods[] = 
{
	{"getFrame", _pyfmcw_getFrame, METH_VARARGS, getFrame_docstring},
	{"test", _pyfmcw_test, METH_NOARGS, "getFrame_docstring"},
	{NULL, NULL, 0, NULL}
};

// Module Definition 
// The arguments of this structure tell Python what to call your extension,
// what it's methods are and where to look for it's method definitions
static struct PyModuleDef pyfmcw_module_definition = {
	PyModuleDef_HEAD_INIT, 
	"_pyfmcw",
	"usage: _pyfmcw.getFrame",
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



