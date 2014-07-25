
__kernel void to_ycbcr(__global unsigned char* pixels, uint width, uint height, __global unsigned char* group_result){
  unsigned char y, cb, cr;
  size_t index_j = get_global_id(0);
  size_t index_i = get_global_id(1);

  if(index_i <= (height-1)*3 && index_i <= (height-1)*3){
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
}

__kernel void to_rgb(__global unsigned char* pixels, uint width, uint height, __global unsigned char* group_result){
  float y, cb, cr;
  size_t index_j = get_global_id(0);
  size_t index_i = get_global_id(1);

  if(index_i <= (height-1)*3 && index_i <= (height-1)*3){
      unsigned char r, g, b;

      int y_index  = index_j * width  + index_i;
      int cb_index = index_j * width  + index_i +     width * height;
      int cr_index = index_j * width  + index_i + 2 * width * height;

       y = pixels[ y_index];
      cb = pixels[cb_index];
      cr = pixels[cr_index];

      r = y                       + 1.402   * (cr - 128);
      g = y -0.34414 * (cb - 128) - 0.71414 * (cr - 128);
      b = y +1.772   * (cb - 128);

      int r_index = index_j * width * 3 + index_i * 3;
      int g_index = index_j * width * 3 + index_i * 3 + 1;
      int b_index = index_j * width * 3 + index_i * 3 + 2;

      group_result[r_index] = (unsigned char) r;
      group_result[g_index] = (unsigned char) g;
      group_result[b_index] = (unsigned char) b;
  }
}
