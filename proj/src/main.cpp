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
void test_compress_decompress(const char *src, const char *dst);

int main(int argc, char* argv[])
{
    load_table("../stuff/chrominace_table",chrominace_table);
    load_table("../stuff/luminace_table",luminace_table);

    initOpenCL();

    test_compress_decompress("../stuff/sample3.tga", "../stuff/sample3_out.tga");
    //measure_color_transform("../stuff/sample1.tga");
    //measure_dct();

    /*unsigned char pic[100];
    unsigned char pic2[100];

    for(int i = 0; i < 100; i++){
        pic[i] = i;
    }

    JpegPicture* jpeg = new JpegPicture(pic,10,10);
    jpeg->save_data(pic2);

    for(int i = 0; i < 100; i++){
        if(i%10==0) std::cout << "\n";
        cout << (int)pic2[i];
    }*/

    return 0;
}

void test_compress_decompress(const char *src, const char *dst){
    pixmap *image = loadTGAdata(src);
    
    pixmap *Y, *Cb, *Cr;
    Y = createPixmap(image->width, image->height, 1);
    Cb = createPixmap(image->width, image->height, 1);
    Cr = createPixmap(image->width, image->height, 1);

    rgb_to_ycbcr(Y, Cb, Cr, image);

    JpegPicture* jpeg_y = new JpegPicture(Y->pixels, (int)(image->height), (int)(image->width));
    JpegPicture* jpeg_y_tmp = new JpegPicture(Y->pixels, (int)(image->height), (int)(image->width));
    JpegPicture* jpeg_cb = new JpegPicture(Cb->pixels, (int)(image->height), (int)(image->width));
    JpegPicture* jpeg_cb_tmp = new JpegPicture(Cb->pixels, (int)(image->height), (int)(image->width));
    JpegPicture* jpeg_cr = new JpegPicture(Cr->pixels, (int)(image->height), (int)(image->width));
    JpegPicture* jpeg_cr_tmp = new JpegPicture(Cr->pixels, (int)(image->height), (int)(image->width));
   
    

    for(int i = 0; i < jpeg_y->block_count; i++){
        dct8x8(jpeg_y->get_block(i),jpeg_y_tmp->get_block(i),luminace_table);
        inv_dct8x8(jpeg_y_tmp->get_block(i),jpeg_y->get_block(i));
       
        dct8x8(jpeg_cb->get_block(i),jpeg_cb_tmp->get_block(i),chrominace_table); 
        inv_dct8x8(jpeg_cb_tmp->get_block(i),jpeg_cb->get_block(i));
       
        dct8x8(jpeg_cr->get_block(i),jpeg_cr_tmp->get_block(i),chrominace_table); 
        inv_dct8x8(jpeg_cr_tmp->get_block(i),jpeg_cr->get_block(i));
    }

    jpeg_y->save_data(Y->pixels);
    jpeg_cb->save_data(Cb->pixels);
    jpeg_cr->save_data(Cr->pixels);

    pixmap *output = ycbcr_to_rgb(Y, Cb, Cr);
    
    saveTruecolorPixmap(output, dst);
}

void measure_color_transform(const char *image_name) {
    // load image
    pixmap *image = loadTGAdata(image_name);
    if (image == NULL) return;
    int size = image->width * image->height;
    int width = image->width;
    int height = image->height;

    // create pixmaps for Y, Cb and Cr
    pixmap *Y, *Cb, *Cr;
    Y  = createPixmap(image->width, image->height, 1); // 1 byte per pixel
    Cb = createPixmap(image->width, image->height, 1);
    Cr = createPixmap(image->width, image->height, 1);

    // compute YCbCr on cpu
    double t1 = get_time();
    rgb_to_ycbcr(Y, Cb, Cr, image);
    double t2 = get_time();
    // save results from cpu
    saveGrayscalePixmap( Y,  "Y_cpu.tga");
    saveGrayscalePixmap(Cb, "Cb_cpu.tga");
    saveGrayscalePixmap(Cr, "Cr_cpu.tga");
    // and back to rgb
    pixmap *image_transf = ycbcr_to_rgb(Y, Cb, Cr);
    saveTruecolorPixmap(image_transf, "transformed.tga");


    // create buffer for results from gpu computing
    unsigned char *dst = new unsigned char[size*3];

    // compute YCbCr on gpu
    double t3 = get_time();
    to_ycbcr_gpu(image->pixels, image->width, image->height, dst);
    double t4 = get_time();

    // copy results  to pixmaps to save it
    memcpy ( Y->pixels,            dst, size);
    memcpy (Cb->pixels,     dst + size, size);
    memcpy (Cr->pixels, dst + size * 2, size);

    //save results from gpu
    saveGrayscalePixmap( Y,  "Y_gpu.tga");
    saveGrayscalePixmap(Cb, "Cb_gpu.tga");
    saveGrayscalePixmap(Cr, "Cr_gpu.tga");


    // create truecolor picture from given Y, Cb, Cr
    unsigned char *dst2 = new unsigned char[size*3];
    to_rgb_gpu(dst, width, height, dst2);

    pixmap *rgb;
    rgb  = createPixmap(width, height, 3);
    memcpy(rgb->pixels, dst2, size*3);
    saveTruecolorPixmap(rgb, "transformed_gpu.tga");

    cout << "\n\nmethod             time [s]\n";
    cout << "color transform:     " << t2-t1 << "\n";
    cout << "color transform GPU: " << t4-t3 << "\n";


    cout << "\n\nmethod             time [s]\n";
    cout << "color transform:     " << t2-t1 << "\n";
    cout << "color transform GPU: " << t4-t3 << "\n";

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
