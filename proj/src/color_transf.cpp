
/* 
  RGB > YCbCr:  
    Y  =    0,299  R + 0,587  G + 0,114  B
    Cb =  - 0,1687 R - 0,3313 G + 0,5    B + 128
    Cr =    0,5    R - 0,4187 G - 0,0813 B + 128 

  ---------------------------------------------------------
  
  YCbCr > RGB:
    R = Y                    + 1.402   (Cr-128)
    G = Y - 0.34414 (Cb-128) - 0.71414 (Cr-128)
    B = Y + 1.772   (Cb-128)

Na vstupu mame soubor TGA typu 24 bpp - truecolor bez alfa kanalu.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

typedef struct {
  int   width;
  int   height;
  unsigned char *pixels; // 0-255
} pixmap;


void rgb_to_ycbcr(pixmap * Y, pixmap *Cb, pixmap *Cr, pixmap *data) {
    int i, j;
    
    // for all rows of the pixmap
    for (j=0; j < data->height; j++) {
        // move pointers
        unsigned char *p_rgb = data->pixels + 3 * j * data->width;
        unsigned char *p_y   =  Y->pixels + j * Y->width;
        unsigned char *p_cb  = Cb->pixels + j * Cb->width;
        unsigned char *p_cr  = Cr->pixels + j * Cr->width;

        // for all pixels in the row
        for (i=0; i < data->width; i++) {
            float r, g, b, y, cb, cr; 

            r = *p_rgb++;
            g = *p_rgb++;
            b = *p_rgb++;

            y  = (unsigned char) ( 0.299  * r +0.587  * g + 0.114  * b);
            cb = (unsigned char) (-0.1687 * r -0.3313 * g + 0.5    * b + 128);
            cr = (unsigned char) ( 0.5    * r -0.4187 * g - 0.0813 * b + 128);

            *p_y++  = y;
            *p_cb++ = cb;
            *p_cr++ = cr;
        }
    
    }
    
}


pixmap* createPixmap(unsigned int width, unsigned int height, unsigned int bytespp) {
    pixmap *p;
    unsigned int size;

    // malloc for pixmap header
    p = (pixmap*) malloc(sizeof(pixmap));
    if (p == NULL) {
      cerr << "malloc error (pixman header)" << endl;
      return NULL;
    }
    
    p->width  = width;
    p->height = height;
    size = width*height*bytespp;  // each pixel 3 bytes
    
    // malloc for pixels
    p->pixels = (unsigned char *) malloc(sizeof(unsigned char)*size);
    if (p == NULL) {
      cerr << "malloc error (pixels)" << endl;
      return NULL;
    }
    return p;
}


/* 
 * Load a pixmap from tga file
 */
pixmap * loadTGAdata (const char * imgname) {
    pixmap *data;
    FILE  *f;
    unsigned int width = 0;
    unsigned int height = 0;
    int bpp = 0;
    int pallength=0;

    int count;
    
    unsigned char tgaHeader[18]={
                        0x00,                   // type of TGA header
                        0x00,                   // we do not use palette
                        0x02,                   // image type - RGB Truecolor
                        0x00, 0x00,             // palette length
                        0x00, 0x00, 0x00,       // do not care about position in palette
                        0x00, 0x00, 0x00, 0x00, // image on position [0, 0]
                        0x00, 0x00, 0x00, 0x00, // width and height (2B each)
                        0x18,                   // 24 bits/px
                        0x20                    // bitmap orientation
    };
    
    f = fopen (imgname, "rb");
    if (f == NULL) {
      cerr << "cannot open image" << endl;
      return NULL;
    }
    
    count=fread(tgaHeader, 18, 1, f);  // load header
    if (count != 1){
      cerr << "cannot load the image header" << endl;
      return NULL;
    }
    
    memcpy(&width, tgaHeader+12, 2);    // copy width
    memcpy(&height, tgaHeader+14, 2);   // copy height
    if (width == 0 || height == 0){
      cerr << "dimensions error" << endl;
      return NULL;
    }    
    
    memcpy(&bpp, tgaHeader+16, 1);      // copy bits per pixel
    if (bpp != 24){
      cerr << "bits per pixel error" << endl;
      return NULL;
    }
    memcpy(&pallength, tgaHeader+5, 2); // copy palette length
    
    cout << " name:  " << imgname << endl << " width:  " << width << endl << " height: " << height << endl << " bpp:    " << bpp << endl;
    
    data = createPixmap(width, height, 3);  // 3 bytes per pixel

    unsigned int i, j;
    unsigned char *b, *r;
        
    // load data into pixmap
    // read from 
    for (i=0; i < height; i++) {
        int offset = i * 3 * data->width; 
        
        // reads a row from image and writes it to the pixmap
        count = fread(data->pixels+offset, 3*data->width, 1, f);
        if (count != 1){
          cerr << "cannot read pixels at offset " << offset << endl;
          return NULL;
        }
        
        b = data->pixels+offset;
        r = b+2;
        
        // from BGR
        //  to  RGB
        for (j=0; j < width; j++, b+=3, r+=3) {
            unsigned char pom = *b;
            *b = *r; 
            *r = pom;
        }
    }
    
    fclose(f);
    cout << "Yaaay! pixmap loaded" << endl << endl;
    return data;
}

/* 
 *  Jen pro ukazku ulozeni do souboru. 
 *  Vyleze z toho intenzita jednotlivych slozek (Y, Cb, Cr).
 */
void saveGrayscalePixmap(pixmap *data, const char *imgname) {
    FILE *f;
    unsigned int i;
    unsigned char tgaHeader[18]={
                        0x00,                   // type of TGA header
                        0x00,                   // we do not use palette
                        0x03,                   // image type - GrayScale
                        0x00, 0x00,             // palette length
                        0x00, 0x00, 0x00,       // do not care about position in palette
                        0x00, 0x00, 0x00, 0x00, // image is on position [0, 0]
                        0x00, 0x00, 0x00, 0x00, // width and height (2B each)
                        0x08,                   // 24 bits/px
                        0x20                    // bitmap orientation
    };

        cout << " name:  " << imgname << endl << " width:  " << data->width << endl << " height: " << data->height << endl << " bpp:    " << 8 << endl;
        
    memcpy(tgaHeader+12, &(data->width), 2);  
    memcpy(tgaHeader+14, &(data->height), 2);

    f = fopen(imgname, "wb");
    if (f == NULL) {
      cerr << "cannot open file" << endl;
      return;
    }
    
    fwrite(tgaHeader, 18, 1, f);  // write the header

    for (i = data->height; i; i--) {                 // radky zapisovat v opacnem poradi
        unsigned int yoff = (i-1) * data->width;       // y-ovy offset v poli
        fwrite((const void *)(data->pixels+yoff), (size_t)data->width, (size_t)1, f); // the whole row
    }
    
    fclose(f);
    cout << "pixmap saved. image created!" << endl;    
}



int main(int argc, char **argv)
{
  if (argc < 2) {
    cerr << "not enough arguments" << endl;
    return 1;
  }
  
  pixmap *data; 
  pixmap *Y;
  pixmap *Cb;
  pixmap *Cr;
  
  data = loadTGAdata(argv[1]);

  cout << "w: " << data->width << " h: " << data->height << endl;
  
  
  Y  = createPixmap(data->width, data->height, 1); // 1 byte per pixel
  Cb = createPixmap(data->width, data->height, 1);
  Cr = createPixmap(data->width, data->height, 1);
  
  cout << "w: " << Y->width << " h: " << Y->height << endl;
  
  rgb_to_ycbcr(Y, Cb, Cr, data);
  
  saveGrayscalePixmap( Y, "Y.tga");
  saveGrayscalePixmap(Cb, "Cb.tga");
  saveGrayscalePixmap(Cr, "Cr.tga");
  
  return 0;
}




