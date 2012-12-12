#include "jpeg_util.h"
#include "cl_util.h"
#include "color_transf.h"

#include <time.h>
#include <string.h>

using namespace std;

void measure_dct();
void measure_rgb_to_ycbcr();
void measure_huffman();
void measure_color_transform(const char *image);

int main(int argc, char* argv[])
{
    load_table("../stuff/chrominace_table",chrominace_table);
    load_table("../stuff/luminace_table",luminace_table);

    //color_transform("../stuff/sample3.tga");

    measure_color_transform("../stuff/sample3.tga");
    //measure_dct();

    //p->save_to_file("test.jpg");
    return 0;
}

void measure_color_transform(const char *image) {

    double t1 = get_time();
    color_transform("../stuff/sample1.tga");
    double t2 = get_time();
    color_transform_gpu("../stuff/sample1.tga");
    double t3 = get_time();

    cout << "\n\nmethod             time [s]\n";
    cout << "color transform:     " << t2-t1 << "\n";
    cout << "color transform GPU: " << t3-t2 << "\n";

}

void measure_dct(){
    float pic[64];
    float d[64];
    float d_gpu[64];
    float inv_d[64];
    float inv_d_gpu[64];

    for(int i=0; i<64; i++){
        pic[i] = i;
    }

    double t1= get_time();
    dct8x8(pic,d,luminace_table);
    double t2= get_time();
    dct8x8_gpu(pic,d_gpu,&cl_luminace_table);
    double t3= get_time();
    inv_dct8x8(d,inv_d);
    double t4= get_time();
    inv_dct8x8_gpu(d_gpu,inv_d_gpu);
    double t5= get_time();

    for(int i=0; i<64; i++){
        if(i%8==0) std::cout << "\n";
        std::cout << (int)d[i] << " ";
    }
    cout << "\n";
    for(int i=0; i<64; i++){
        if(i%8==0) cout << "\n";
        cout << (int)d_gpu[i] << " ";
    }

    cout << "\n\ninverse";
    for(int i=0; i<64; i++){
        if(i%8==0) cout << "\n";
        cout << (int)inv_d[i] << " ";
    }
    cout << "\n";
    for(int i=0; i<64; i++){
        if(i%8==0) cout << "\n";
        cout << (int)inv_d_gpu[i] << " ";
    }

    cout << "\n\nmethod     time [s]\n";
    cout << "DCT:         " << t2-t1 << "\n";
    cout << "DCT GPU:     " << t3-t2 << "\n";
    cout << "INV DCT:     " << t4-t3 << "\n";
    cout << "INV DCT GPU: " << t5-t4 << "\n";
}
