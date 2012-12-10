#ifndef CL_UTIL_H
#define CL_UTIL_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#include <iostream>
#include <fstream>
#include <string>


extern cl_mem cl_luminace_table; 
extern cl_mem cl_chrominace_table;

int initOpenCL();
double getTime();
cl_int loadDctKernelFromFile(const char* cFilename);
cl_int loadInvDctKernelFromFile(const char* cFilename);
void checkClError(cl_int err, char* debug);
const char *CLErrorString(cl_int _err);
void CL_CALLBACK contextCallback(const char *err_info, 
                                 const void *private_intfo,
                                 size_t cb,
                                 void *user_data);
void dct8x8_gpu(float* src, float* dst, cl_mem* table);
void inv_dct8x8_gpu(float* src, float* dst);
double get_time();
#endif 
