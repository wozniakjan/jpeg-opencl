#ifndef COLOR_TRANSF_H
#define COLOR_TRANSF_H

#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

typedef struct {
  unsigned int   width;
  unsigned int   height;
  unsigned char *pixels; // 0-255
} pixmap;

pixmap * loadTGAdata (const char * imgname);
void saveGrayscalePixmap(pixmap *data, const char *imgname);
void saveTruecolorPixmap(pixmap *data, const char *imgname);

void rgb_to_ycbcr(pixmap * Y, pixmap *Cb, pixmap *Cr, pixmap *data);
pixmap* ycbcr_to_rgb(pixmap * Y, pixmap *Cb, pixmap *Cr);
pixmap* createPixmap(unsigned int width, unsigned int height, unsigned int bytespp);

#endif
