#include "cl_util.h"
#include "jpeg_util.h"

int main(int argc, char* argv[])
{
    //initOpenCL();
    SOF0 *marker = new SOF0(1,2,3,4);
    std::cout << marker->length();
    return 0;
}

