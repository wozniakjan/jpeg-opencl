#include "jpeg_util.h"

using namespace std;


// 0 table
char luminace_table[] = {
    16,  11,  10,  16,  24,  40,  51,  61, 
    12,  12,  14,  19,  26,  58,  60,  55, 
    14,  13,  16,  24,  40,  57,  69,  56, 
    14,  17,  22,  29,  51,  87,  80,  62, 
    18,  22,  37,  56,  68, 109, 103,  77, 
    24,  35,  55,  64,  81, 104, 113,  92, 
    49,  64,  78,  87, 103, 121, 120, 101, 
    72,  92,  95,  98, 112, 100, 103,  99, 
};

// 1 table
char chrominace_table[] = {
    17,  18,  24,  47,  99,  99,  99,  99, 
    18,  21,  26,  66,  99,  99,  99,  99, 
    24,  26,  56,  99,  99,  99,  99,  99, 
    47,  66,  99,  99,  99,  99,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99, 
};



// Basic constructor
JpegPicture::JpegPicture() {
    init_first_markers();
}

JpegPicture::~JpegPicture() {
}

// Initializes all the markers and headers to default values
void JpegPicture::init_first_markers() {
    markers.push_back(new SOI());
    markers.push_back(new APP0());
    markers.push_back(new DQT(0));
    markers.push_back(new DQT(1));


    markers.push_back(new EOI());
}

// Saves the picture from memory to the file
void JpegPicture::save_to_file(string file_name) {
    ofstream pic(file_name.c_str(), ios::out | ios::binary);
    
    for(int i=0; i<markers.size(); i++){
        pic.write(markers[i]->get_data(),markers[i]->get_len());
    }
}

Marker::Marker(){}

void Marker::init(int _len) {
    data_len = _len;
    data = (char*)malloc(sizeof(char)*data_len);
    data[0] = 0xFF;
}

Marker::~Marker() {
    if(data != NULL) {
        free(data);
    }
    data_len = 0;
}

char* Marker::get_data() {
    return data;
}

int Marker::get_len() {
    return data_len;
}

SOI::SOI() {
    init(2);
    data[1] = 0xD8;
}

EOI::EOI() {
    init(2);
    data[1] = 0xD9;
}

DQT::DQT(int type) {
    init(69);
    data[1] = 0xDB;

    // Length
    data[2] = 0x43;
    // Table ID
    data[3] = type;
    // Quantization size
    data[4] = 0; //could be 0 for 1 byte table or 1 for 2 byte table

    char *table;

    switch(type) {
        case 0:
            table = luminace_table; 
            break;
        case 1:
            table = chrominace_table;
            break;
        case 2:
            table = NULL;
            break;
        case 3:
            table = NULL;
            break;
        default:
            table = NULL;
            break;
    }

    if(table != NULL) {
        for(int i = 0; i<64; i++){
            data[i+5] = table[i];
        }
    }
}


APP0::APP0() {
    init(18);
    data[1]  = 0xE0;

    // Length of APP0 field
    data[2]  = 0x00; 
    data[3]  = 0x10;

    // Identification of JPEG - JFIF
    data[4]  = 0x4A;
    data[5]  = 0x46;
    data[6]  = 0x49;
    data[7]  = 0x46;
    data[8]  = 0x00;
    
    // Version 0100, 0101, 0102
    data[9]  = 0x01;
    data[10] = 0x01;
    
    // Units used for resolution - 00 pix
    data[11] = 0x00;
    
    // Density or resolution - 0001 for pix   
    data[12] = 0x00;
    data[13] = 0x01;
    data[14] = 0x00;
    data[15] = 0x01;
    
    // Horizontal resolution but this part of heared is usually not used - 00  
    data[16] = 0x00;
    data[17] = 0x00;
}


SOF0::SOF0(char l0, char l1, char r0, char r1) {
    init(19);
    data[1] = 0xC0;
    
    // Frame Header Length
    data[2] = 0x00;
    data[3] = 0x11;

    // Precision
    data[4] = 0x08;

    // Lines
    data[5] = l0;
    data[6] = l1;

    // Rows
    data[7] = r0;
    data[8] = r1;

    // Components Count
    data[9] = 0x03;
    // Y component
    data[10] = 0x01; //ID
    data[11] = 0x21; //SamplingFactor
    data[12] = 0x00; //QuantTable ID
    // Cb component
    data[13] = 0x02; //ID
    data[14] = 0x11; //SamplingFactor
    data[15] = 0x01; //QuantTable ID
    // Cr component
    data[16] = 0x03; //ID
    data[17] = 0x11; //SamplingFactor
    data[18] = 0x01; //QuantTable ID
}

DHT::DHT()
