#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable


__kernel void square ( __global float *inOut, int dataSize)
{
  int gx = (int)get_global_id(0);
//  int lx = (int)get_local_id(0);
//  int wx = (int)get_group_id(0);

  int gsize = (int)get_global_size(0);
  while(gx < dataSize)
  {
    float temp = inOut[gx];
	inOut[gx] = temp*temp;
    gx += gsize;
  }

}




