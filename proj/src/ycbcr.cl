
__kernel void ycbcr(__global unsigned char* pixels, uint width, uint height, __global unsigned char* group_result){
  unsigned char y, cb, cr;
  size_t index_j = get_global_id(0);
  size_t index_i = get_global_id(1);
  float r, g, b;

  int r_index = index_j * width * 3 + index_i * 3;
  int g_index = index_j * width * 3 + index_i * 3 + 1;
  int b_index = index_j * width * 3 + index_i * 3 + 2;

  r = pixels[r_index];
  g = pixels[g_index];
  b = pixels[b_index];

  y  = ( 0.299  * r + 0.587 * g + 0.114  * b);
  cb = (-0.1687 * r -0.3313 * g + 0.5    * b + 128);
  cr = ( 0.5    * r -0.4187 * g - 0.0813 * b + 128);

  int y_index  = index_j * width + index_i;
  int cb_index = index_j * width + index_i +     width * height;
  int cr_index = index_j * width + index_i + 2 * width * height;

  group_result[ y_index] = (unsigned char) y;
  group_result[cb_index] = (unsigned char) cb;
  group_result[cr_index] = (unsigned char) cr;

}
