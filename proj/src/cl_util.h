#ifndef CL_UTIL_H
#define CL_UTIL_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#include <iostream>
#include <fstream>
//#include <ssteram>
#include <string>

int initOpenCL();
double getTime();
cl_int loadKernelFromFile(const char* cFilename);
void checkClError(cl_int err, char* debug);
const char *CLErrorString(cl_int _err);
void CL_CALLBACK contextCallback(const char *err_info, 
                                 const void *private_intfo,
                                 size_t cb,
                                 void *user_data);
#endif 
