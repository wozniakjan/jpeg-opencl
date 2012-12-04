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
        void init(int length);
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
        //char length(char nl = 0);
        DQT(int type);
};

// Baseline DCT, Start of Frame
class SOF0 : public Marker {
    public:
        int length(int set_len=-1);
        char precision(char set_prec=-1);
        int lines(int set_l=-1);
        int rows(int set_r=-1);
        char component_count(char set_count=-1);
        char sampling_factor(int component_id, char set_factor=-1);
        char quant_table_id(int component_id, char set_qaunt_table_id=-1);
        SOF0(char line_count0, char line_count1, char rows_count0, char rows_count1);
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
        SOS(char* components, int component_count, char* payload, int payload_length);
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
