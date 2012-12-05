#include "jpeg_util.h"

int main(int argc, char* argv[])
{
    //initOpenCL();
    JpegPicture *p = new JpegPicture();
    p->save_to_file("test.jpg");
    return 0;
}

