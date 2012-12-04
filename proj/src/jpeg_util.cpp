#include "jpeg_util.h"
#include <string.h>

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



void move_right(int* r, int* c, int r_count, int c_count, int* t, vector<int>* z){
    int index = (*r)*c_count+(*c);
    //cout << "r  [" << (*r) << ":" << (*c) << "] -> " << index << endl;
    (*c)++;
    z->push_back(t[index]);
}

void move_down(int* r, int* c, int r_count, int c_count, int* t, vector<int>* z){ 
    int index = (*r)*c_count+(*c); 
    //cout << "d  [" << (*r) << ":" << (*c) << "] -> " << index << endl;
    (*r)++;
    z->push_back(t[index]);
}

void move_diagonal_down(int* r, int* c, int r_count, int c_count, int* t, vector<int>* z){
    int index = 0;
    for(; (*r)<(r_count-1) && (*c)>0; (*r)++, (*c)--){
        index = (*r)*c_count+(*c);
        //cout << "dd [" << (*r) << ":" << (*c) << "] -> " << index << endl;
        z->push_back(t[index]);
    }
}

void move_diagonal_up(int* r, int* c, int r_count, int c_count, int* t, vector<int>* z){ 
    int index = 0;
    for(; (*r)>0 && (*c)<(c_count-1); (*r)--, (*c)++){
        index = (*r)*c_count+(*c);
        //cout << "du [" << (*r) << ":" << (*c) << "] -> " << index << endl;
        z->push_back(t[index]);
    }
}

void zig_zag(int* table, int* ziged, int rows, int cols){
    if(ziged == NULL){
        ziged = (int*)malloc(sizeof(int)*rows*cols);
    }
    
    int max_index = rows+cols-2;
    int r = 0;
    int max_row = rows-1;
    int c = 0;
    int max_col = cols-1;
    vector<int> aux_ziged;
    
    while(r+c < max_index){
        if(c<max_col) {
            move_right(&r,&c,rows,cols,table,&aux_ziged);
        } else {
            move_down(&r,&c,rows,cols,table,&aux_ziged);
        }
        
        move_diagonal_down(&r,&c,rows,cols,table,&aux_ziged);
        
        if(r<max_row){
            move_down(&r,&c,rows,cols,table,&aux_ziged);
        } else {
            move_right(&r,&c,rows,cols,table,&aux_ziged);
        }
        
        move_diagonal_up(&r,&c,rows,cols,table,&aux_ziged);
    }
    
    //cout << "f  [" << r << ":" << c << "] -> " <<  endl;
    aux_ziged.push_back(table[r*cols+c]);

    for(int i = 0; i<=max_index; i++){
        ziged[i] = aux_ziged[i];
    }
}

void test_zigzag(){
    int test_table[] = {
        1,  2,  3,  4,  5,  6,  7,  8,
        9, 10, 11, 12, 13, 14, 15, 16,
        17,18, 19, 20, 21, 22, 23, 24,
        25,26, 27, 28, 29, 30, 31, 32,
        33,34, 35, 36, 37, 38, 39, 40,
        41,42, 43, 44, 45, 46, 47, 48,
        49,50, 51, 52, 53, 54, 55, 56,
        57,58, 59, 60, 61, 62, 63, 64,
    };
    

    int* z = NULL;
    zig_zag(test_table,z,8,8);
    for(int i =0; i<64; i++){
        std::cout << z[i] << " ";
    }
}

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


SOF0::SOF0(int l, int r) {
    init(19);
    data[1] = 0xC0;
    
    // Frame Header Length
    data[2] = 0x00;
    data[3] = 0x11;

    // Precision
    data[4] = 0x08;

    // Lines
    //data[5] = l0;
    //data[6] = l1;
    lines(l);
    rows(r);

    // Rows
    //data[7] = r0;
    //data[8] = r1;

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

int SOF0::length(int set_len){
    if(set_len > -1){
        data[2] = (char)(set_len >> 4);
        data[3] = (char)set_len;
    }
    int aux = data[2] << 4;
    aux += data[3];
    return aux;
}

int SOF0::precision(int set_prec){
    if(set_prec > -1){
        data[4] = (char)set_prec;
    }
    return (int)data[4];
}

int SOF0::lines(int set_lines){
    if(set_lines > -1){
        data[5] = (char)(set_lines >> 4);
        data[6] = (char)set_lines;
    }
    int aux = data[5] << 4;
    aux += data[6];
    return aux;
}

int SOF0::rows(int set_rows){
    if(set_rows > -1){
        data[7] = (char)(set_rows >> 4);
        data[8] = (char)set_rows;
    }
    int aux = data[7] << 4;
    aux += data[8];
    return aux;
}

int SOF0::component_count(int set_count){
    if(set_count > -1){
        data[9] = (char)set_count;
    }
    return (int)data[9];
}

int SOF0::sampling_factor(int c_id, int set_factor){
    int component_count = (int)data[9];
    if(c_id < component_count){
        // 11 offset, *3 because of three tables
        int index = 11 + (c_id*3);
        if(set_factor > -1){
            data[index] = (char)set_factor;
        }
        return (int)data[index];
    }
    return -1;
}

int SOF0::quant_table_id(int c_id, int set_quant_table_id){
    int component_count = (int)data[9];
    if(c_id < component_count){
        // 12 offset, *3 because of three tables
        int index = 12 + (c_id*3);
        if(set_quant_table_id > -1){
            data[index] = (char)set_quant_table_id;
        }
        return (int)data[index];
    }
    return -1;
}

DHT::DHT(){
    init(840);
    data[1] = 0xC4;

    //Header Length
    data[2] = 0x01;
    data[3] = 0xA2;

    //Table Class X Table Destination
    data[4] = 0x00;

    strcpy(data, "FFC401A200000105010101010101000000000000000001020304\
       05060708090A0B100002010303020403050504040000017D0102030004110512\
       2131410613516107227114328191A1082342B1C11552D1F02433627282090A161718191\
       A25262728292A3435363738393A434445464748494A535455565758595A63646566\
       6768696A737475767778797A838485868788898A92939495969798999AA2A3A4A5A6A7A8A9\
       AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE1E2E3E4E5E\
       6E7E8E9EAF1F2F3F4F5F6F7F8F9FA01000301010101010101010100000\
       00000000102030405060708090A0B1100020102040403040705040400010277000\
       102031104052131061241510761711322328108144291A1B1C109233352F0156272D\
       10A162434E125F11718191A262728292A35363738393A434445464748494A53545556\
       5758595A636465666768696A737475767778797A82838485868788898A92939495969798\
       999AA2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9D\
       AE2E3E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FA");
}

DRI::DRI(){
    init(6);
    data[1] = 0xDD;
    
    //Length
    data[2] = 0x00;
    data[3] = 0x04;

    //interval
    data[4] = 0x00;
    data[5] = 0x00;
}

SOS::SOS(char* comp, int comp_count, char* pay, int pay_len){
    init(5+comp_count*2+pay_len);
    data[1] = 0xDA;

    //length
    data[2] = 0x00;
    data[3] = 0x0C;

    //number of image components - equal to cs in scan header
    data[4] = 0x03;

    for(int i = 0; i<(comp_count*2); i+=2){
        //component i
        data[i+5] = comp[i]; //selector
        data[i+6] = comp[i+1]; //table (DC|AC)
    }

    int offset = comp_count*2+7;
    for(int i = 0; i<pay_len; i++){
        data[i+offset] = pay[i];
    }
}
