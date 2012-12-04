#ifndef JPEG_UTIL_H
#define JPEG_UTIL_H

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>


class Marker { 
    protected:
        char *data;     // Head[2] + payload[n]
        int data_len;   // Length of Marker Head & payload

    public:
        Marker();
        ~Marker();
        void init(int _len);
        char* get_data();
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
        DQT(int type);
};

// Baseline DCT, Start of Frame
class SOF0 : public Marker {
    public:
        SOF0(char l0, char l1, char r0, char r1);
};

// Huffman Tables
class DHT : public Marker {
    public:
        DHT();
};

// Start of Scan
class SOS : public Marker {
    public:
        SOS();
};



// End of Image
class EOI : public Marker {
    public:
        EOI();
};

class JpegPicture {
    std::vector<Marker*> markers;    // Markers
    void init_first_markers();

    public:
        JpegPicture();
        ~JpegPicture();

        void save_to_file(std::string file_name);
};

#endif
