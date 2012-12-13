#include "jpeg_util.h"
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846264338327950288f
#define BLOCK_SIZE 8

using namespace std;


// 0 table
int luminace_table[] = {
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
int chrominace_table[] = {
    17,  18,  24,  47,  99,  99,  99,  99, 
    18,  21,  26,  66,  99,  99,  99,  99, 
    24,  26,  56,  99,  99,  99,  99,  99, 
    47,  66,  99,  99,  99,  99,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99, 
};

void load_table(char* table_file, int* table){
    ifstream f;
    f.open(table_file);
    
    string val;
    int val_i = 0;

    if(f.is_open()){
        for(int index = 0; index < 64 && !f.eof(); index++){
            f >> val;
            val_i = atoi(val.c_str());
            table[index] = val_i;
        }
    }
    f.close();
}

float lambda(int k){
    if(k==0) return 1.0f/sqrtf(2.0f);
    else return 1;
}

float g(int k, int j, int n, int m){
    return (lambda(k)*lambda(j)*2/8*cosf(k*PI/8*((float)n+0.5f))*
            cosf(j*PI/8*((float)m+0.5f)));
}

//MUL 4cv, Barina
void dct8x8(float* block, float* dst, int* table){
    for(int k=0; k<BLOCK_SIZE; k++){
        for(int j=0; j<BLOCK_SIZE; j++){
            int index = k*8+j;
            dst[index] = 0;
            for(int n=0; n<8; n++){
                for(int m=0; m<8; m++){
                    int index2 = n*8+m;
                    dst[index] += block[index2]*g(k,j,n,m);
                }
            }
            //quantization
            dst[index] = roundf((dst[index]/table[index]))*table[index]; 
        }
    }
}

void inv_dct8x8(float* block, float* dst){
    for(int n=0; n<BLOCK_SIZE; n++){
        for(int m=0; m<BLOCK_SIZE; m++){
            int index = n*8+m;
            dst[index] = 0;
            for(int k=0; k<8; k++){
                for(int j=0; j<8; j++){
                    int index2 = k*8+j;
                    dst[index] += block[index2]*g(k,j,n,m);
                }
            }
        }
    }
}

void move_right(int* r, int* c, int r_count, int c_count, int* t, vector<int>* z){
    int index = (*r)*c_count+(*c);
    (*c)++;
    z->push_back(t[index]);
}

void move_down(int* r, int* c, int r_count, int c_count, int* t, vector<int>* z){ 
    int index = (*r)*c_count+(*c); 
    (*r)++;
    z->push_back(t[index]);
}

void move_diagonal_down(int* r, int* c, int r_count, int c_count, int* t, vector<int>* z){
    int index = 0;
    for(; (*r)<(r_count-1) && (*c)>0; (*r)++, (*c)--){
        index = (*r)*c_count+(*c);
        z->push_back(t[index]);
    }
}

void move_diagonal_up(int* r, int* c, int r_count, int c_count, int* t, vector<int>* z){ 
    int index = 0;
    for(; (*r)>0 && (*c)<(c_count-1); (*r)--, (*c)++){
        index = (*r)*c_count+(*c);
        z->push_back(t[index]);
    }
}

void zig_zag(int* table, int* ziged, int rows, int cols){ 
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
    
    aux_ziged.push_back(table[r*cols+c]);
    int max = rows*cols;
    for(int i = 0; i<max; i++){
        ziged[i] = aux_ziged[i];
    }
}

int get_blocks(int x){
    if(x%8 == 0) return x/8;
    else return x/8 + 1;
}

// Basic constructor
JpegPicture::JpegPicture(unsigned char* p, int r, int c) {
    src = (unsigned char*)malloc(sizeof(unsigned char)*r*c);
    memcpy(src, p, r*c);
    rows = r;
    cols = c;


    int pic_y = 0, pic_x = 0;
    int max_y = r-1, max_x = c-1;
    int block_index, index;

    float* block;
    block_count = get_blocks(r)*get_blocks(c);
    blocks = (float**)malloc(sizeof(float*)*block_count);
    int block_id = 0;
    for(int offset_y = 0; offset_y<r; offset_y+=8){
        for(int offset_x = 0; offset_x<c; offset_x+=8){
            blocks[block_id] = (float*)malloc(sizeof(float)*64);
            for(int y = 0; y<BLOCK_SIZE; y++){
                for(int x = 0; x<BLOCK_SIZE; x++){
                    pic_y = min(offset_y+y,max_y);
                    pic_x = min(offset_x+x,max_x);
                    index = pic_y*c+pic_x;
                    block_index = y*BLOCK_SIZE+x;
                    blocks[block_id][block_index] = (float)src[index];
                }
            }
            block_id++;
        }
    }
    markers.push_back(new SOI());
    markers.push_back(new APP0());
    markers.push_back(new DQT());
    markers.push_back(new SOF0(144,200));
    markers.push_back(new DHT());
    markers.push_back(new DRI());
    markers.push_back(new SOS());
    markers.push_back(new EOI());
}

JpegPicture::~JpegPicture() {
    free(src);
    for(int i=0; i<block_count; i++)
        free(blocks[i]);
    free(blocks);
}

float* JpegPicture::get_block(int i){
    if(i<block_count){
        return blocks[i];
    }
    return NULL;
}

void JpegPicture::save_data(unsigned char* data){
    int x, y;
    int block_x, block_y, block;
    int block_i_x, block_i_y, block_i; 
    int max_block = ceil((double)cols/8.0);

    for(int i = 0; i<rows*cols; i++){
        x = i % cols;
        y = i / cols;
        block_x = x / 8;
        block_y = y / 8;
        block = block_y*max_block + block_x;
        block_i_x = x % 8;
        block_i_y = y % 8;
        block_i = block_i_y*8+block_i_x; 
        data[i] = (unsigned char)blocks[block][block_i]; 
    }
}

// Saves the picture from memory to the file
void JpegPicture::save_to_file(string file_name) {
    ofstream pic(file_name.c_str(), ios::out | ios::binary);
    
    for(int i=0; i<markers.size(); i++){
        pic.write((char*)markers[i]->get_data(),markers[i]->get_len());
    }
}

Marker::Marker(){}

void Marker::init(int _len) {
    data_len = _len;
    data = (unsigned char*)malloc(sizeof(unsigned char)*data_len);
    data[0] = 0xFF;
}

Marker::~Marker() {
    if(data != NULL) {
        free(data);
    }
    data_len = 0;
}

unsigned char* Marker::get_data() {
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

DQT::DQT() {
    init(134);
    data[1] = 0xDB;

    // Length
    data[2] = 0x00;
    data[3] = 0x84;

    int table[64];
    int offset = 5;
    for(int type = 0; type < 2; type++){
        if(type == 0) {
            zig_zag(luminace_table, table, 8, 8);
            data[offset-1] = 0x00; //(precision | table_id)
        } else {
            offset = 70;
            zig_zag(chrominace_table, table, 8, 8);
            data[offset-1] = 0x01; //(precision | table_id)
        }
        
        for(int i = 0; i<64; i++){
            data[i+offset] = (unsigned char)table[i];
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
    lines(l);
    rows(r);
    //data[5] = 0x00;
    //data[6] = 0x90;

    // Rows
    //data[7] = 0x00;
    //data[8] = 0xC8;

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
        data[2] = (unsigned char)(set_len >> 8);
        data[3] = (unsigned char)set_len & 0xFF;
    }
    int aux = data[2] << 8;
    aux += data[3];
    return aux;
}

int SOF0::precision(int set_prec){
    if(set_prec > -1){
        data[4] = (unsigned char)set_prec;
    }
    return (int)data[4];
}

int SOF0::lines(int set_lines){
    if(set_lines > -1){
        data[5] = (unsigned char)(set_lines >> 8);
        data[6] = (unsigned char)set_lines & 0xFF;
    }
    int aux = data[5] << 8;
    aux += data[6];
    return aux;
}

int SOF0::rows(int set_rows){
    if(set_rows > -1){
        data[7] = (unsigned char)(set_rows >> 8);
        data[8] = (unsigned char)set_rows & 0xFF;
    }
    int aux = data[7] << 8;
    aux += data[8];
    return aux;
}

int SOF0::component_count(int set_count){
    if(set_count > -1){
        data[9] = (unsigned char)set_count;
    }
    return (int)data[9];
}

int SOF0::sampling_factor(int c_id, int set_factor){
    int component_count = (int)data[9];
    if(c_id < component_count){
        // 11 offset, *3 because of three tables
        int index = 11 + (c_id*3);
        if(set_factor > -1){
            data[index] = (unsigned char)set_factor;
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
            data[index] = (unsigned char)set_quant_table_id;
        }
        return (int)data[index];
    }
    return -1;
}

DHT::DHT(){
    init(420);
    data[1] = 0xC4;

    //Header Length
    data[2] = 0x01;
    data[3] = 0xA2;

    //Table Class X Table Destination
    data[4] = 0x00;
    
    string aux = "FFC401A20000010501010101010100000000000000000102030" 
        "405060708090A0B100002010303020403050504040000017D01020300041" 
        "105122131410613516107227114328191A1082342B1C11552D1F02433627" 
        "282090A161718191A25262728292A3435363738393A434445464748494A5" 
        "35455565758595A636465666768696A737475767778797A8384858687888" 
        "98A92939495969798999AA2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C" 
        "3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE1E2E3E4E5E6E7E8E9EAF1F2F3F" 
        "4F5F6F7F8F9FA01000301010101010101010100000000000001020304050" 
        "60708090A0B1100020102040403040705040400010277000102031104052" 
        "131061241510761711322328108144291A1B1C109233352F0156272D10A1" 
        "62434E125F11718191A262728292A35363738393A434445464748494A535" 
        "455565758595A636465666768696A737475767778797A828384858687888" 
        "98A92939495969798999AA2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C" 
        "3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE2E3E4E5E6E7E8E9EAF2F3F4F5F" 
        "6F7F8F9FA";

    char x[2];
    for(int i = 0; i<aux.length(); i+=2){
        x[0] = aux[i];
        x[1] = aux[i+1];
        data[i/2] = (unsigned char)strtol(x,NULL,16);
    }
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

SOS::SOS(){
    string aux = "FFDA000C03010002110311003F00E028A0028A0028A0028A002" 
      "8A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A" 
      "0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A00" 
      "28A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028" 
      "A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0" 
      "028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A002" 
      "8A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A" 
      "0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A00" 
      "28A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028" 
      "A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0" 
      "028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A002" 
      "8A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A" 
      "0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A00" 
      "28A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028" 
      "A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0" 
      "028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A002" 
      "8A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A" 
      "0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A00" 
      "28A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028A0028" 
      "A0028A0028A0028A0028A0028A0028A00";
    
    init(aux.length()/2);
    char x[2];
    for(int i = 0; i<aux.length(); i+=2){
        x[0] = aux[i];
        x[1] = aux[i+1];
        data[i/2] = (unsigned char)strtol(x,NULL,16);
    }
}

SOS::SOS(unsigned char* comp, int comp_count, unsigned char* pay, int pay_len){
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
