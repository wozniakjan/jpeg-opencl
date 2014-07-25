
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <map>

using namespace std;

string to_bits(int num, unsigned int nBit);
short *dc_Y(short bytes[], int size);
short *dc_Cb(short bytes[], int size);
short *dc_Cr(short bytes[], int size);
vector< pair<uint8_t,short> > rlc_AC(short bytes[], int *size);

void huffEncode_1AC(vector< pair<uint8_t,short> > bytes, int size);
void huffEncode_0AC(vector< pair<uint8_t,short> > bytes, int size);

void huffEncode_1DC(short bytes[], int size);
void huffEncode_0DC(short bytes[], int size);

int get_category_DC(short n);
int get_category_AC(short n);

string to_bits(int num, unsigned int nBit);
