#include "jpeg_util.h"

int main(int argc, char* argv[])
{
    //initOpenCL();
    float pic[64];
    float d[64];
    float inv_d[64];

    for(int i=0; i<64; i++){
        pic[i]=i;
    }

    dct8x8(pic,d);
    inv_dct8x8(d,inv_d);

    //JpegPicture *p = new JpegPicture(pic, 8, 8);
    

    for(int i=0; i<64; i++){
        if(i%8==0) std::cout << "\n";
        std::cout << (int)inv_d[i] << " ";
    }
    
    //p->save_to_file("test.jpg");
    return 0;
}

