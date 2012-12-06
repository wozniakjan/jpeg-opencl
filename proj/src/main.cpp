#include "jpeg_util.h"
#include "cl_util.h"

#include <string.h>

int main(int argc, char* argv[])
{
    initOpenCL();
    float pic[64];
    float d[64];
    float d_gpu[64];
    float inv_d[64];
    float inv_d_gpu[64];

    for(int i=0; i<64; i++){
        pic[i]=i;
    }

    dct8x8(pic,d,chrominace_table);
    dct8x8_gpu(pic,d_gpu,&cl_chrominace_table);
    inv_dct8x8(d,inv_d);
    inv_dct8x8(d_gpu,inv_d_gpu);

    //JpegPicture *p = new JpegPicture(pic, 8, 8);
    for(int i=0; i<64; i++){
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
    }

    
    //p->save_to_file("test.jpg");
    return 0;
}

