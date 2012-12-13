#define PI 3.14159265358979323846264338327950288

float lambda(int k);
float g(int k, int j, int n, int m);
float my_round(float i);

float lambda(int k){
    if(k==0) return 0.707106781186547524f;
    else return 1;
}

float g(int k, int j, int n, int m){ 
    return (lambda(k)*lambda(j)*2/8*cos(k*PI/8*((float)n+0.5f))*
            cos(j*PI/8*((float)m+0.5f)));
}

float my_round(float i){
    float truncated = convert_float(convert_int(i));
    float diff = i-truncated;
    if(diff < 0.5f && diff > -0.5f) return truncated;
    if(diff >= 0.5f) return truncated + 1.0f;
    else return truncated - 1.0f;
}

__kernel void dct8x8 ( __global float * in, __global float * out, __global int * table ) {
    int k = get_global_id(0);
    int j = get_global_id(1);

    if(k < 8 && j < 8){
        int index = k*8+j;
        out[index] = in[0]*g(k,j,0,0);
        out[index] += in[1]*g(k,j,0,1);
        out[index] += in[2]*g(k,j,0,2);
        out[index] += in[3]*g(k,j,0,3);
        out[index] += in[4]*g(k,j,0,4);
        out[index] += in[5]*g(k,j,0,5);
        out[index] += in[6]*g(k,j,0,6);
        out[index] += in[7]*g(k,j,0,7);
        out[index] += in[8]*g(k,j,1,0);
        out[index] += in[9]*g(k,j,1,1);
        out[index] += in[10]*g(k,j,1,2);
        out[index] += in[11]*g(k,j,1,3);
        out[index] += in[12]*g(k,j,1,4);
        out[index] += in[13]*g(k,j,1,5);
        out[index] += in[14]*g(k,j,1,6);
        out[index] += in[15]*g(k,j,1,7);
        out[index] += in[16]*g(k,j,2,0);
        out[index] += in[17]*g(k,j,2,1);
        out[index] += in[18]*g(k,j,2,2);
        out[index] += in[19]*g(k,j,2,3);
        out[index] += in[20]*g(k,j,2,4);
        out[index] += in[21]*g(k,j,2,5);
        out[index] += in[22]*g(k,j,2,6);
        out[index] += in[23]*g(k,j,2,7);
        out[index] += in[24]*g(k,j,3,0);
        out[index] += in[25]*g(k,j,3,1);
        out[index] += in[26]*g(k,j,3,2);
        out[index] += in[27]*g(k,j,3,3);
        out[index] += in[28]*g(k,j,3,4);
        out[index] += in[29]*g(k,j,3,5);
        out[index] += in[30]*g(k,j,3,6);
        out[index] += in[31]*g(k,j,3,7);
        out[index] += in[32]*g(k,j,4,0);
        out[index] += in[33]*g(k,j,4,1);
        out[index] += in[34]*g(k,j,4,2);
        out[index] += in[35]*g(k,j,4,3);
        out[index] += in[36]*g(k,j,4,4);
        out[index] += in[37]*g(k,j,4,5);
        out[index] += in[38]*g(k,j,4,6);
        out[index] += in[39]*g(k,j,4,7);
        out[index] += in[40]*g(k,j,5,0);
        out[index] += in[41]*g(k,j,5,1);
        out[index] += in[42]*g(k,j,5,2);
        out[index] += in[43]*g(k,j,5,3);
        out[index] += in[44]*g(k,j,5,4);
        out[index] += in[45]*g(k,j,5,5);
        out[index] += in[46]*g(k,j,5,6);
        out[index] += in[47]*g(k,j,5,7);
        out[index] += in[48]*g(k,j,6,0);
        out[index] += in[49]*g(k,j,6,1);
        out[index] += in[50]*g(k,j,6,2);
        out[index] += in[51]*g(k,j,6,3);
        out[index] += in[52]*g(k,j,6,4);
        out[index] += in[53]*g(k,j,6,5);
        out[index] += in[54]*g(k,j,6,6);
        out[index] += in[55]*g(k,j,6,7);
        out[index] += in[56]*g(k,j,7,0);
        out[index] += in[57]*g(k,j,7,1);
        out[index] += in[58]*g(k,j,7,2);
        out[index] += in[59]*g(k,j,7,3);
        out[index] += in[60]*g(k,j,7,4);
        out[index] += in[61]*g(k,j,7,5);
        out[index] += in[62]*g(k,j,7,6);
        out[index] += in[63]*g(k,j,7,7);
        out[index] = my_round(out[index]/table[index])*table[index];
    }
}

__kernel void inv_dct8x8 ( __global float * in, __global float * out ) {
    int m = get_global_id(0);
    int n = get_global_id(1);

    if(m < 8 && n < 8){
        int index = n*8+m;
        out[index] = in[0]*g(0,0,n,m);
        out[index] += in[1]*g(0,1,n,m);
        out[index] += in[2]*g(0,2,n,m);
        out[index] += in[3]*g(0,3,n,m);
        out[index] += in[4]*g(0,4,n,m);
        out[index] += in[5]*g(0,5,n,m);
        out[index] += in[6]*g(0,6,n,m);
        out[index] += in[7]*g(0,7,n,m);
        out[index] += in[8]*g(1,0,n,m);
        out[index] += in[9]*g(1,1,n,m);
        out[index] += in[10]*g(1,2,n,m);
        out[index] += in[11]*g(1,3,n,m);
        out[index] += in[12]*g(1,4,n,m);
        out[index] += in[13]*g(1,5,n,m);
        out[index] += in[14]*g(1,6,n,m);
        out[index] += in[15]*g(1,7,n,m);
        out[index] += in[16]*g(2,0,n,m);
        out[index] += in[17]*g(2,1,n,m);
        out[index] += in[18]*g(2,2,n,m);
        out[index] += in[19]*g(2,3,n,m);
        out[index] += in[20]*g(2,4,n,m);
        out[index] += in[21]*g(2,5,n,m);
        out[index] += in[22]*g(2,6,n,m);
        out[index] += in[23]*g(2,7,n,m);
        out[index] += in[24]*g(3,0,n,m);
        out[index] += in[25]*g(3,1,n,m);
        out[index] += in[26]*g(3,2,n,m);
        out[index] += in[27]*g(3,3,n,m);
        out[index] += in[28]*g(3,4,n,m);
        out[index] += in[29]*g(3,5,n,m);
        out[index] += in[30]*g(3,6,n,m);
        out[index] += in[31]*g(3,7,n,m);
        out[index] += in[32]*g(4,0,n,m);
        out[index] += in[33]*g(4,1,n,m);
        out[index] += in[34]*g(4,2,n,m);
        out[index] += in[35]*g(4,3,n,m);
        out[index] += in[36]*g(4,4,n,m);
        out[index] += in[37]*g(4,5,n,m);
        out[index] += in[38]*g(4,6,n,m);
        out[index] += in[39]*g(4,7,n,m);
        out[index] += in[40]*g(5,0,n,m);
        out[index] += in[41]*g(5,1,n,m);
        out[index] += in[42]*g(5,2,n,m);
        out[index] += in[43]*g(5,3,n,m);
        out[index] += in[44]*g(5,4,n,m);
        out[index] += in[45]*g(5,5,n,m);
        out[index] += in[46]*g(5,6,n,m);
        out[index] += in[47]*g(5,7,n,m);
        out[index] += in[48]*g(6,0,n,m);
        out[index] += in[49]*g(6,1,n,m);
        out[index] += in[50]*g(6,2,n,m);
        out[index] += in[51]*g(6,3,n,m);
        out[index] += in[52]*g(6,4,n,m);
        out[index] += in[53]*g(6,5,n,m);
        out[index] += in[54]*g(6,6,n,m);
        out[index] += in[55]*g(6,7,n,m);
        out[index] += in[56]*g(7,0,n,m);
        out[index] += in[57]*g(7,1,n,m);
        out[index] += in[58]*g(7,2,n,m);
        out[index] += in[59]*g(7,3,n,m);
        out[index] += in[60]*g(7,4,n,m);
        out[index] += in[61]*g(7,5,n,m);
        out[index] += in[62]*g(7,6,n,m);
        out[index] += in[63]*g(7,7,n,m);
    }
}
