#include "jpeg_util.h"
#include "cl_util.h"

#include <string.h>

int main(int argc, char* argv[])
{
    initOpenCL();
    unsigned char a[100];
    float pic[64];
    float d[64];
    float d_gpu[64];
    float inv_d[64];
    float inv_d_gpu[64];

    for(int i=0; i<100; i++){
        a[i] = i;
    }

    /*dct8x8(pic,d,luminace_table);
    dct8x8_gpu(pic,d_gpu,&cl_luminace_table);
    inv_dct8x8(d,inv_d);
    inv_dct8x8(d_gpu,inv_d_gpu);*/

    JpegPicture *p = new JpegPicture(a, 10, 10);
    std::cout << p->block_count;
    for(int i=0; i<64; i++){
        if(i%8==0) std::cout << "\n";
        std::cout << (int)(p->get_block(0)[i]) << " ";
    }

    /*for(int i=0; i<64; i++){
        if(i%8==0) std::cout << "\n";
        std::cout << (int)d[i] << " ";
    }
    std::cout << "\n";
    for(int i=0; i<64; i++){
        if(i%8==0) std::cout << "\n";
        std::cout << (int)d_gpu[i] << " ";
    }

    std::cout << "\n\ninverse";
    for(int i=0; i<64; i++){
        if(i%8==0) std::cout << "\n";
        std::cout << (int)inv_d[i] << " ";
    }
    std::cout << "\n";
    for(int i=0; i<64; i++){
        if(i%8==0) std::cout << "\n";
        std::cout << (int)inv_d_gpu[i] << " ";
    }*/

    
    //p->save_to_file("test.jpg");
    return 0;
}

