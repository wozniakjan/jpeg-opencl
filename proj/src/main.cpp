#include "jpeg_util.h"

int main(int argc, char* argv[])
{
    //initOpenCL();
    unsigned char pic[100];

    for(int i=0; i<100; i++){
        pic[i]=i;
    }

    JpegPicture *p = new JpegPicture(pic, 10, 10);

    for(int i=0; i<64; i++){
        if(i%8==0) std::cout << "\n";
        std::cout << p->get_block(1)[i] << " ";
    }
    
    p->save_to_file("test.jpg");
    return 0;
}

