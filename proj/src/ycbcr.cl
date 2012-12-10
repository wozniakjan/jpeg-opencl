__kernel void y(){

}

__kernel void cb(){

}

__kernel void cr(){

}

/*
//obavam se, ze kompilator OpenCL nebude cl_uchar* znat, pokud mu nereknes, jak vypada
//radeji bych zkusil pouzivat nejake normalni ceckovske typy, treba unsigned char

__kernel void y(__global cl_uchar* pixels, __global unsigned int width, __global cl_uchar* group_result) {
  unsigned char y;
	size_t index_x = get_global_id(0);
	size_t index_y = get_global_id(1);  
  float r, g, b;

  r = pixels[index_y * width * 3 + index_x];
  g = pixels[index_y * width * 3 + index_x+1];
  b = pixels[index_y * width * 3 + index_x+2];
  
  y  = (unsigned char) ( 0.299  * r + 0.587  * g + 0.114  * b);

  group_result[index_y*width+index_x] = y;

}

__kernel void cb(__global cl_uchar* pixels, __local cl_uchar* local_result, __global cl_uchar* group_result) {
  unsigned char cb;
	size_t index_x = get_global_id(0);
	size_t index_y = get_global_id(1);  
  float r, g, b;

  r = pixels[index_y * width * 3 + index_x];
  g = pixels[index_y * width * 3 + index_x+1];
  b = pixels[index_y * width * 3 + index_x+2];
  
  cb = (unsigned char) (-0.1687 * r -0.3313 * g + 0.5    * b + 128);

  group_result[index_y*width+index_x] = cb;      

}

__kernel void cr(__global cl_uchar* pixels, __local cl_uchar* local_result, __global cl_uchar* group_result) {
  unsigned char cr;
	size_t index_x = get_global_id(0);
	size_t index_y = get_global_id(1);  
  float r, g, b;

  r = pixels[index_y * width * 3 + index_x];
  g = pixels[index_y * width * 3 + index_x+1];
  b = pixels[index_y * width * 3 + index_x+2];
  
  cr = (unsigned char) ( 0.5    * r -0.4187 * g - 0.0813 * b + 128);

  group_result[index_y*width+index_x] = cr;      

}*/
