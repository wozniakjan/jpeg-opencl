#ifndef JPEG_UTIL_H
#define JPEG_UTIL_H

#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

extern int luminace_table[];
extern int chrominace_table[];

void load_table(char* table_file, int* table);
void zig_zag(int* table, int* ziged, int rows, int cols);
void dct8x8(float* block, float* dct_block, int* table);
void inv_dct8x8(float* dct_block, float* block);

class Marker {
    protected:
        unsigned char *data;     // Head[2] + payload[n]
        int data_len;   // Length of Marker Head & payload

    public:
        Marker();
        ~Marker();
        void init(int length);
        unsigned char* get_data();
        int get_len();
};

// Start of Image
class SOI : public Marker {
    public:
        SOI();
};

// Application Specific
class APP0 : public Marker {
    public:
        APP0();
};


// Quantization Tables
class DQT : public Marker {
    public:
        //unsigned char length(unsigned char nl = 0);
        DQT();
};

// Baseline DCT, Start of Frame
class SOF0 : public Marker {
    public:
        SOF0(int lines, int rows);
        int length(int set_len=-1);
        int precision(int set_prec=-1);
        int lines(int set_l=-1);
        int rows(int set_r=-1);
        int component_count(int set_count=-1);
        int sampling_factor(int component_id, int set_factor=-1);
        int quant_table_id(int component_id, int set_qaunt_table_id=-1);
};

// Huffman Tables
class DHT : public Marker {
    public:
        DHT();
};

// Restart Interval
class DRI : public Marker {
    public:
        DRI();
};

// Start of Scan
class SOS : public Marker {
    public:
        SOS();
        SOS(unsigned char* components, int component_count, unsigned char* payload, int payload_length);
};



// End of Image
class EOI : public Marker {
    public:
        EOI();
};

class JpegPicture {
    std::vector<Marker*> markers;    // Markers

    unsigned char* src;
    int rows;
    int cols;

    //std::vector<float*> *blocks;

    float** blocks;

    public:
        int block_count;
        JpegPicture(unsigned char* pic, int rows, int cols);
        ~JpegPicture();

        void save_to_file(std::string file_name);
        float* get_block(int i);
        void save_data(unsigned char* data);
};

#endif
