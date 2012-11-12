__kernel void jpeg_block ( __global int * in, __global int * out, int dataSize ) {
    int gx = get_global_id(0);

    if(gx < dataSize){
        out[gx] = in[gx];
    }
}
