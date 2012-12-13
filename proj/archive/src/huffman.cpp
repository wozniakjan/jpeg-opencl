/*

AC/DC coefficients taken from CCITT recommendation T.81 (DIGITAL COMPRESSION AND CODING OF CONTINUOUS-TONE STILL IMAGES), Annex K.

DHT:
FF C4 00 1F 00 00 01 05 01 01 01 01 01 01 00 00 00 00 00 00 00 00 01 02 03 04 05 06 07 08 09 0A 0B

FF C4 00 B5 10 00 02 01 03 03 02 04 03 05 05 04 04 00 00 01 7D 01 02 03 00 04 11 05 12 21 31 41 06 13 51 61 07 22 71 14 32 81 91 A1 08 23 42 B1 C1 15 52 D1 F0 24 33 62 72 82 09 0A 16 17 18 19 1A 25 26 27 28 29 2A 34 35 36 37 38 39 3A 43 44 45 46 47 48 49 4A 53 54 55 56 57 58 59 5A 63 64 65 66 67 68 69 6A 73 74 75 76 77 78 79 7A 83 84 85 86 87 88 89 8A 92 93 94 95 96 97 98 99 9A A2 A3 A4 A5 A6 A7 A8 A9 AA B2 B3 B4 B5 B6 B7 B8 B9 BA C2 C3 C4 C5 C6 C7 C8 C9 CA D2 D3 D4 D5 D6 D7 D8 D9 DA E1 E2 E3 E4 E5 E6 E7 E8 E9 EA F1 F2 F3 F4 F5 F6 F7 F8 F9 FA

FF C4 00 IF 01 00 03 01 01 01 01 01 01 01 01 01 00 00 00 00 00 00 01 02 03 04 05 06 07 08 09 0A 0B

FF C4 00 B5 11 00 02 01 02 04 04 03 04 07 05 04 04 00 01 02 77 00 01 02 03 11 04 05 21 31 06 12 41 51 07 61 71 13 22 32 81 08 14 42 91 A1 B1 C1 09 23 33 52 F0 15 62 72 D1 0A 16 24 34 E1 25 F1 17 18 19 1A 26 27 28 29 2A 35 36 37 38 39 3A 43 44 45 46 47 48 49 4A 53 54 55 56 57 58 59 5A 63 64 65 66 67 68 69 6A 73 74 75 76 77 78 79 7A 82 83 84 85 86 87 88 89 8A 92 93 94 95 96 97 98 99 9A A2 A3 A4 A5 A6 A7 A8 A9 AA B2 B3 B4 B5 B6 B7 B8 B9 BA C2 C3 C4 C5 C6 C7 C8 C9 CA D2 D3 D4 D5 D6 D7 D8 D9 DA E2 E3 E4 E5 E6 E7 E8 E9 EA F2 F3 F4 F5 F6 F7 F8 F9 FA

 */

#include "dpcm_rle.h"


/*
      delka     |~> pocty symbolu pro delky kodovych slov 1-16  ~|
FF C4 00 1F 00   00 01 05 01 01 01 01 01 01 00 00 00 00 00 00 00   00 01 02 03 04 05 06 07 08 09 0A 0B
             l~> "000" musi byt vzdy + "0"=DC + "0000"=tabulka pro barvove slozky Y
 */
void huffmanTable_0DC(map<short, string> &ht ) {

  ht.insert(pair <short, string> (0x00, "00"));
  ht.insert(pair <short, string> (0x01, "010"));
  ht.insert(pair <short, string> (0x02, "011"));
  ht.insert(pair <short, string> (0x03, "100"));
  ht.insert(pair <short, string> (0x04, "101"));
  ht.insert(pair <short, string> (0x05, "110"));
  ht.insert(pair <short, string> (0x06, "1110"));
  ht.insert(pair <short, string> (0x07, "11110"));
  ht.insert(pair <short, string> (0x08, "111110"));
  ht.insert(pair <short, string> (0x09, "1111110"));
  ht.insert(pair <short, string> (0x0A, "11111110"));
  ht.insert(pair <short, string> (0x0B, "111111110"));

}

/*

      "000" musi byt vzdy + "1"=AC + "0000"=tabulka pro barvove slozky Y

      delka     |~> pocty symbolu pro delky kodovych slov 1-16  ~|
FF C4 00 B5 10   00 02 01 03 03 02 04 03 05 05 04 04 00 00 01 7D   01 02 03 00 04 11 05 12 21 31 41 06 13 51 61 07 22 71 14 32 81 91 A1 08 23 42 B1 C1 15 52 D1 F0 24 33 62 72 82 09 0A 16 17 18 19 1A 25 26 27 28 29 2A 34 35 36 37 38 39 3A 43 44 45 46 47 48 49 4A 53 54 55 56 57 58 59 5A 63 64 65 66 67 68 69 6A 73 74 75 76 77 78 79 7A 83 84 85 86 87 88 89 8A 92 93 94 95 96 97 98 99 9A A2 A3 A4 A5 A6 A7 A8 A9 AA B2 B3 B4 B5 B6 B7 B8 B9 BA C2 C3 C4 C5 C6 C7 C8 C9 CA D2 D3 D4 D5 D6 D7 D8 D9 DA E1 E2 E3 E4 E5 E6 E7 E8 E9 EA F1 F2 F3 F4 F5 F6 F7 F8 F9 FA

 */

void huffmanTable_0AC(map<unsigned char, string> &ht ) {  
  ht.insert(pair <unsigned char, string> (0x00, "1010"));
  ht.insert(pair <unsigned char, string> (0x01, "00"));
  ht.insert(pair <unsigned char, string> (0x02, "01")); 
  ht.insert(pair <unsigned char, string> (0x03, "100"));
  ht.insert(pair <unsigned char, string> (0x04, "1011"));
  ht.insert(pair <unsigned char, string> (0x05, "11010"));  
  ht.insert(pair <unsigned char, string> (0x06, "1111000"));
  ht.insert(pair <unsigned char, string> (0x07, "11111000"));
  ht.insert(pair <unsigned char, string> (0x08, "1111110110"));
  ht.insert(pair <unsigned char, string> (0x09, "1111111110000010"));
  ht.insert(pair <unsigned char, string> (0x0A, "1111111110000011"));
    
  ht.insert(pair <unsigned char, string> (0x11, "1100"));
  ht.insert(pair <unsigned char, string> (0x12, "11011"));
  ht.insert(pair <unsigned char, string> (0x13, "1111001"));
  ht.insert(pair <unsigned char, string> (0x14, "111110110"));
  ht.insert(pair <unsigned char, string> (0x15, "11111110110"));
  ht.insert(pair <unsigned char, string> (0x16, "1111111110000100"));
  ht.insert(pair <unsigned char, string> (0x17, "1111111110000101"));
  ht.insert(pair <unsigned char, string> (0x18, "1111111110000110"));
  ht.insert(pair <unsigned char, string> (0x19, "1111111110000111"));
  ht.insert(pair <unsigned char, string> (0x1A, "1111111110001000"));  

  ht.insert(pair <unsigned char, string> (0x21, "11100"));
  ht.insert(pair <unsigned char, string> (0x22, "11111001"));
  ht.insert(pair <unsigned char, string> (0x23, "1111110111"));
  ht.insert(pair <unsigned char, string> (0x24, "111111110100"));
  ht.insert(pair <unsigned char, string> (0x25, "1111111110001001"));
  ht.insert(pair <unsigned char, string> (0x26, "1111111110001010"));
  ht.insert(pair <unsigned char, string> (0x27, "1111111110001011"));
  ht.insert(pair <unsigned char, string> (0x28, "1111111110001100"));
  ht.insert(pair <unsigned char, string> (0x29, "1111111110001101"));
  ht.insert(pair <unsigned char, string> (0x2A, "1111111110001110"));
  
  
  ht.insert(pair <unsigned char, string> (0x31, "111010"));
  ht.insert(pair <unsigned char, string> (0x32, "111110111"));
  ht.insert(pair <unsigned char, string> (0x33, "111111110101"));
  ht.insert(pair <unsigned char, string> (0x34, "1111111110001111"));    
  ht.insert(pair <unsigned char, string> (0x35, "1111111110010000"));
  ht.insert(pair <unsigned char, string> (0x36, "1111111110010001"));
  ht.insert(pair <unsigned char, string> (0x37, "1111111110010010"));
  ht.insert(pair <unsigned char, string> (0x38, "1111111110010011"));
  ht.insert(pair <unsigned char, string> (0x39, "1111111110010100"));
  ht.insert(pair <unsigned char, string> (0x3A, "1111111110010101"));
  
  
  ht.insert(pair <unsigned char, string> (0x41, "111011"));
  ht.insert(pair <unsigned char, string> (0x42, "1111111000"));
  ht.insert(pair <unsigned char, string> (0x43, "1111111110010110"));
  ht.insert(pair <unsigned char, string> (0x44, "1111111110010111"));
  ht.insert(pair <unsigned char, string> (0x45, "1111111110011000"));
  ht.insert(pair <unsigned char, string> (0x46, "1111111110011001"));
  ht.insert(pair <unsigned char, string> (0x47, "1111111110011010"));
  ht.insert(pair <unsigned char, string> (0x48, "1111111110011011"));
  ht.insert(pair <unsigned char, string> (0x49, "1111111110011100"));
  ht.insert(pair <unsigned char, string> (0x4A, "1111111110011101"));


  ht.insert(pair <unsigned char, string> (0x51, "1111010"));
  ht.insert(pair <unsigned char, string> (0x52, "11111110111"));
  ht.insert(pair <unsigned char, string> (0x53, "1111111110011110"));
  ht.insert(pair <unsigned char, string> (0x54, "1111111110011111"));    
  ht.insert(pair <unsigned char, string> (0x55, "1111111110100000"));
  ht.insert(pair <unsigned char, string> (0x56, "1111111110100001"));
  ht.insert(pair <unsigned char, string> (0x57, "1111111110100010"));
  ht.insert(pair <unsigned char, string> (0x58, "1111111110100011"));
  ht.insert(pair <unsigned char, string> (0x59, "1111111110100100"));
  ht.insert(pair <unsigned char, string> (0x5A, "1111111110100101"));
  
  ht.insert(pair <unsigned char, string> (0x61, "1111011"));
  ht.insert(pair <unsigned char, string> (0x62, "111111110110"));
  ht.insert(pair <unsigned char, string> (0x63, "1111111110100110"));
  ht.insert(pair <unsigned char, string> (0x64, "1111111110100111"));
  ht.insert(pair <unsigned char, string> (0x65, "1111111110101000"));
  ht.insert(pair <unsigned char, string> (0x66, "1111111110101001"));
  ht.insert(pair <unsigned char, string> (0x67, "1111111110101010"));
  ht.insert(pair <unsigned char, string> (0x68, "1111111110101011"));
  ht.insert(pair <unsigned char, string> (0x69, "1111111110101100"));
  ht.insert(pair <unsigned char, string> (0x6A, "1111111110101101"));


  ht.insert(pair <unsigned char, string> (0x71, "11111010"));
  ht.insert(pair <unsigned char, string> (0x72, "111111110111"));  
  ht.insert(pair <unsigned char, string> (0x73, "1111111110101110"));
  ht.insert(pair <unsigned char, string> (0x74, "1111111110101111")); 
  ht.insert(pair <unsigned char, string> (0x75, "1111111110110000"));
  ht.insert(pair <unsigned char, string> (0x76, "1111111110110001"));
  ht.insert(pair <unsigned char, string> (0x77, "1111111110110010"));
  ht.insert(pair <unsigned char, string> (0x78, "1111111110110011"));
  ht.insert(pair <unsigned char, string> (0x79, "1111111110110100"));
  ht.insert(pair <unsigned char, string> (0x7A, "1111111110110101"));


  ht.insert(pair <unsigned char, string> (0x81, "111111000"));
  ht.insert(pair <unsigned char, string> (0x82, "111111111000000"));
  ht.insert(pair <unsigned char, string> (0x83, "1111111110110110"));
  ht.insert(pair <unsigned char, string> (0x84, "1111111110110111"));
  ht.insert(pair <unsigned char, string> (0x85, "1111111110111000"));
  ht.insert(pair <unsigned char, string> (0x86, "1111111110111001"));
  ht.insert(pair <unsigned char, string> (0x87, "1111111110111010"));
  ht.insert(pair <unsigned char, string> (0x88, "1111111110111011"));
  ht.insert(pair <unsigned char, string> (0x89, "1111111110111100"));
  ht.insert(pair <unsigned char, string> (0x8A, "1111111110111101"));
  
  ht.insert(pair <unsigned char, string> (0x91, "111111001"));
  ht.insert(pair <unsigned char, string> (0x92, "1111111110111110"));
  ht.insert(pair <unsigned char, string> (0x93, "1111111110111111"));
  ht.insert(pair <unsigned char, string> (0x94, "1111111111000000"));
  ht.insert(pair <unsigned char, string> (0x95, "1111111111000001"));
  ht.insert(pair <unsigned char, string> (0x96, "1111111111000010"));
  ht.insert(pair <unsigned char, string> (0x97, "1111111111000011"));
  ht.insert(pair <unsigned char, string> (0x98, "1111111111000100"));
  ht.insert(pair <unsigned char, string> (0x99, "1111111111000101"));
  ht.insert(pair <unsigned char, string> (0x9A, "1111111111000110"));
  
  ht.insert(pair <unsigned char, string> (0xA1, "111111010"));
  ht.insert(pair <unsigned char, string> (0xA2, "1111111111000111"));
  ht.insert(pair <unsigned char, string> (0xA3, "1111111111001000"));
  ht.insert(pair <unsigned char, string> (0xA4, "1111111111001001"));
  ht.insert(pair <unsigned char, string> (0xA5, "1111111111001010"));
  ht.insert(pair <unsigned char, string> (0xA6, "1111111111001011"));
  ht.insert(pair <unsigned char, string> (0xA7, "1111111111001100"));
  ht.insert(pair <unsigned char, string> (0xA8, "1111111111001101"));
  ht.insert(pair <unsigned char, string> (0xA9, "1111111111001110"));
  ht.insert(pair <unsigned char, string> (0xAA, "1111111111001111"));

  ht.insert(pair <unsigned char, string> (0xB1, "1111111001"));
  ht.insert(pair <unsigned char, string> (0xB2, "1111111111010000"));
  ht.insert(pair <unsigned char, string> (0xB3, "1111111111010001"));
  ht.insert(pair <unsigned char, string> (0xB4, "1111111111010010"));
  ht.insert(pair <unsigned char, string> (0xB5, "1111111111010011"));
  ht.insert(pair <unsigned char, string> (0xB6, "1111111111010100"));
  ht.insert(pair <unsigned char, string> (0xB7, "1111111111010101"));
  ht.insert(pair <unsigned char, string> (0xB8, "1111111111010110"));
  ht.insert(pair <unsigned char, string> (0xB9, "1111111111010111"));
  ht.insert(pair <unsigned char, string> (0xBA, "1111111111011000"));
  
  ht.insert(pair <unsigned char, string> (0xC1, "1111111010"));
  ht.insert(pair <unsigned char, string> (0xC2, "1111111111011001"));
  ht.insert(pair <unsigned char, string> (0xC3, "1111111111011010"));
  ht.insert(pair <unsigned char, string> (0xC4, "1111111111011011"));
  ht.insert(pair <unsigned char, string> (0xC5, "1111111111011100"));
  ht.insert(pair <unsigned char, string> (0xC6, "1111111111011101"));
  ht.insert(pair <unsigned char, string> (0xC7, "1111111111011110"));
  ht.insert(pair <unsigned char, string> (0xC8, "1111111111011111"));
  ht.insert(pair <unsigned char, string> (0xC9, "1111111111100000"));
  ht.insert(pair <unsigned char, string> (0xCA, "1111111111100001"));
  
  ht.insert(pair <unsigned char, string> (0xD1, "11111111000"));
  ht.insert(pair <unsigned char, string> (0xD2, "1111111111100010"));
  ht.insert(pair <unsigned char, string> (0xD3, "1111111111100011"));
  ht.insert(pair <unsigned char, string> (0xD4, "1111111111100100"));
  ht.insert(pair <unsigned char, string> (0xD5, "1111111111100101"));
  ht.insert(pair <unsigned char, string> (0xD6, "1111111111100110"));
  ht.insert(pair <unsigned char, string> (0xD7, "1111111111100111"));
  ht.insert(pair <unsigned char, string> (0xD8, "1111111111101000"));
  ht.insert(pair <unsigned char, string> (0xD9, "1111111111101001"));
  ht.insert(pair <unsigned char, string> (0xDA, "1111111111101010"));
  
  ht.insert(pair <unsigned char, string> (0xE1, "1111111111101011"));
  ht.insert(pair <unsigned char, string> (0xE2, "1111111111101100"));
  ht.insert(pair <unsigned char, string> (0xE3, "1111111111101101"));
  ht.insert(pair <unsigned char, string> (0xE4, "1111111111101110"));
  ht.insert(pair <unsigned char, string> (0xE5, "1111111111101111"));
  ht.insert(pair <unsigned char, string> (0xE6, "1111111111110000"));
  ht.insert(pair <unsigned char, string> (0xE7, "1111111111110001"));
  ht.insert(pair <unsigned char, string> (0xE8, "1111111111110010"));
  ht.insert(pair <unsigned char, string> (0xE9, "1111111111110011"));
  ht.insert(pair <unsigned char, string> (0xEA, "1111111111110100"));
  
  ht.insert(pair <unsigned char, string> (0xF0, "11111111001"));
  ht.insert(pair <unsigned char, string> (0xF1, "1111111111110101"));
  ht.insert(pair <unsigned char, string> (0xF2, "1111111111110110"));
  ht.insert(pair <unsigned char, string> (0xF3, "1111111111110111"));
  ht.insert(pair <unsigned char, string> (0xF4, "1111111111111000"));
  ht.insert(pair <unsigned char, string> (0xF5, "1111111111111001"));
  ht.insert(pair <unsigned char, string> (0xF6, "1111111111111010"));
  ht.insert(pair <unsigned char, string> (0xF7, "1111111111111011"));
  ht.insert(pair <unsigned char, string> (0xF8, "1111111111111100"));
  ht.insert(pair <unsigned char, string> (0xF9, "1111111111111101"));
  ht.insert(pair <unsigned char, string> (0xFA, "1111111111111110"));
   
}

/* 
 
      delka     |~> pocty symbolu pro delky kodovych slov 1-16  ~|
FF C4 00 IF 01   00 03 01 01 01 01 01 01 01 01 01 00 00 00 00 00   00 01 02 03 04 05 06 07 08 09 0A 0B
             l~> "000" musi byt vzdy + "0"=DC + "0001"=barvove slozky CbCr
 */
void huffmanTable_1DC(map<short, string> &ht ) {  

  ht.insert(pair <short, string> (0x00, "00"));
  ht.insert(pair <short, string> (0x01, "01"));
  ht.insert(pair <short, string> (0x02, "10"));
  ht.insert(pair <short, string> (0x03, "110"));
  ht.insert(pair <short, string> (0x04, "1110"));
  ht.insert(pair <short, string> (0x05, "11110"));
  ht.insert(pair <short, string> (0x06, "111110"));
  ht.insert(pair <short, string> (0x07, "1111110"));
  ht.insert(pair <short, string> (0x08, "11111110"));
  ht.insert(pair <short, string> (0x09, "111111110"));
  ht.insert(pair <short, string> (0x0A, "1111111110"));
  ht.insert(pair <short, string> (0x0B, "11111111110"));

}

/* 
       "000" musi byt vzdy + "1"=AC + "0001"=barvove slozky CbCr
 
      delka     |~> pocty symbolu pro delky kodovych slov 1-16  ~|
FF C4 00 B5 11   00 02 01 02 04 04 03 04 07 05 04 04 00 01 02 77   00 01 02 03 11 04 05 21 31 06 12 41 51 07 61 71 13 22 32 81 08 14 42 91 A1 B1 C1 09 23 33 52 F0 15 62 72 D1 0A 16 24 34 E1 25 F1 17 18 19 1A 26 27 28 29 2A 35 36 37 38 39 3A 43 44 45 46 47 48 49 4A 53 54 55 56 57 58 59 5A 63 64 65 66 67 68 69 6A 73 74 75 76 77 78 79 7A 82 83 84 85 86 87 88 89 8A 92 93 94 95 96 97 98 99 9A A2 A3 A4 A5 A6 A7 A8 A9 AA B2 B3 B4 B5 B6 B7 B8 B9 BA C2 C3 C4 C5 C6 C7 C8 C9 CA D2 D3 D4 D5 D6 D7 D8 D9 DA E2 E3 E4 E5 E6 E7 E8 E9 EA F2 F3 F4 F5 F6 F7 F8 F9 FA
 */
void huffmanTable_1AC(map<unsigned char, string> &ht ) {  

  ht.insert(pair <unsigned char, string> (0x00, "00"));
  ht.insert(pair <unsigned char, string> (0x01, "01"));
  ht.insert(pair <unsigned char, string> (0x02, "100")); 
  ht.insert(pair <unsigned char, string> (0x03, "1010"));
  ht.insert(pair <unsigned char, string> (0x04, "11000"));
  ht.insert(pair <unsigned char, string> (0x05, "11001"));  
  ht.insert(pair <unsigned char, string> (0x06, "111000"));
  ht.insert(pair <unsigned char, string> (0x07, "1111000"));
  ht.insert(pair <unsigned char, string> (0x08, "111110100"));
  ht.insert(pair <unsigned char, string> (0x09, "1111110110"));
  ht.insert(pair <unsigned char, string> (0x0A, "111111110100"));

  ht.insert(pair <unsigned char, string> (0x11, "1011"));
  ht.insert(pair <unsigned char, string> (0x12, "111001"));
  ht.insert(pair <unsigned char, string> (0x13, "11110110"));
  ht.insert(pair <unsigned char, string> (0x14, "111110101"));
  ht.insert(pair <unsigned char, string> (0x15, "11111110110"));
  ht.insert(pair <unsigned char, string> (0x16, "111111110101"));
  ht.insert(pair <unsigned char, string> (0x17, "1111111110001000"));
  ht.insert(pair <unsigned char, string> (0x18, "1111111110001001"));
  ht.insert(pair <unsigned char, string> (0x19, "1111111110001010"));
  ht.insert(pair <unsigned char, string> (0x1A, "1111111110001011"));  

  ht.insert(pair <unsigned char, string> (0x21, "11010"));
  ht.insert(pair <unsigned char, string> (0x22, "11110111"));
  ht.insert(pair <unsigned char, string> (0x23, "1111110111"));
  ht.insert(pair <unsigned char, string> (0x24, "111111110110"));
  ht.insert(pair <unsigned char, string> (0x25, "111111111000010"));
  ht.insert(pair <unsigned char, string> (0x26, "1111111110001100"));
  ht.insert(pair <unsigned char, string> (0x27, "1111111110001101"));
  ht.insert(pair <unsigned char, string> (0x28, "1111111110001110"));
  ht.insert(pair <unsigned char, string> (0x29, "1111111110001111"));
  ht.insert(pair <unsigned char, string> (0x2A, "1111111110010000"));
  
  ht.insert(pair <unsigned char, string> (0x31, "11011"));
  ht.insert(pair <unsigned char, string> (0x32, "11111000"));
  ht.insert(pair <unsigned char, string> (0x33, "1111111000"));
  ht.insert(pair <unsigned char, string> (0x34, "111111110111"));    
  ht.insert(pair <unsigned char, string> (0x35, "1111111110010001"));
  ht.insert(pair <unsigned char, string> (0x36, "1111111110010010"));
  ht.insert(pair <unsigned char, string> (0x37, "1111111110010011"));
  ht.insert(pair <unsigned char, string> (0x38, "1111111110010100"));
  ht.insert(pair <unsigned char, string> (0x39, "1111111110010101"));
  ht.insert(pair <unsigned char, string> (0x3A, "1111111110010110"));
  
  ht.insert(pair <unsigned char, string> (0x41, "111010"));
  ht.insert(pair <unsigned char, string> (0x42, "111110110"));
  ht.insert(pair <unsigned char, string> (0x43, "1111111110010111"));
  ht.insert(pair <unsigned char, string> (0x44, "1111111110011000"));
  ht.insert(pair <unsigned char, string> (0x45, "1111111110011001"));
  ht.insert(pair <unsigned char, string> (0x46, "1111111110011010"));
  ht.insert(pair <unsigned char, string> (0x47, "1111111110011011"));
  ht.insert(pair <unsigned char, string> (0x48, "1111111110011100"));
  ht.insert(pair <unsigned char, string> (0x49, "1111111110011101"));
  ht.insert(pair <unsigned char, string> (0x4A, "1111111110011110"));

  ht.insert(pair <unsigned char, string> (0x51, "111011"));
  ht.insert(pair <unsigned char, string> (0x52, "1111111001"));
  ht.insert(pair <unsigned char, string> (0x53, "1111111110011111"));
  ht.insert(pair <unsigned char, string> (0x54, "1111111110100000"));    
  ht.insert(pair <unsigned char, string> (0x55, "1111111110100001"));
  ht.insert(pair <unsigned char, string> (0x56, "1111111110100010"));
  ht.insert(pair <unsigned char, string> (0x57, "1111111110100011"));
  ht.insert(pair <unsigned char, string> (0x58, "1111111110100100"));
  ht.insert(pair <unsigned char, string> (0x59, "1111111110100101"));
  ht.insert(pair <unsigned char, string> (0x5A, "1111111110100110"));
  
  ht.insert(pair <unsigned char, string> (0x61, "1111001"));
  ht.insert(pair <unsigned char, string> (0x62, "11111110111"));
  ht.insert(pair <unsigned char, string> (0x63, "1111111110100111"));
  ht.insert(pair <unsigned char, string> (0x64, "1111111110101000"));
  ht.insert(pair <unsigned char, string> (0x65, "1111111110101001"));
  ht.insert(pair <unsigned char, string> (0x66, "1111111110101010"));
  ht.insert(pair <unsigned char, string> (0x67, "1111111110101011"));
  ht.insert(pair <unsigned char, string> (0x68, "1111111110101100"));
  ht.insert(pair <unsigned char, string> (0x69, "1111111110101101"));
  ht.insert(pair <unsigned char, string> (0x6A, "1111111110101110"));

  ht.insert(pair <unsigned char, string> (0x71, "1111010"));
  ht.insert(pair <unsigned char, string> (0x72, "11111111000"));  
  ht.insert(pair <unsigned char, string> (0x73, "1111111110101111"));
  ht.insert(pair <unsigned char, string> (0x74, "1111111110110000")); 
  ht.insert(pair <unsigned char, string> (0x75, "1111111110110001"));
  ht.insert(pair <unsigned char, string> (0x76, "1111111110110010"));
  ht.insert(pair <unsigned char, string> (0x77, "1111111110110011"));
  ht.insert(pair <unsigned char, string> (0x78, "1111111110110100"));
  ht.insert(pair <unsigned char, string> (0x79, "1111111110110101"));
  ht.insert(pair <unsigned char, string> (0x7A, "1111111110110110"));

  ht.insert(pair <unsigned char, string> (0x81, "11111001"));
  ht.insert(pair <unsigned char, string> (0x82, "1111111110110111"));
  ht.insert(pair <unsigned char, string> (0x83, "1111111110111000"));
  ht.insert(pair <unsigned char, string> (0x84, "1111111110111001"));
  ht.insert(pair <unsigned char, string> (0x85, "1111111110111010"));
  ht.insert(pair <unsigned char, string> (0x86, "1111111110111011"));
  ht.insert(pair <unsigned char, string> (0x87, "1111111110111100"));
  ht.insert(pair <unsigned char, string> (0x88, "1111111110111101"));
  ht.insert(pair <unsigned char, string> (0x89, "1111111110111110"));
  ht.insert(pair <unsigned char, string> (0x8A, "1111111110111111"));

  ht.insert(pair <unsigned char, string> (0x91, "111110111"));
  ht.insert(pair <unsigned char, string> (0x92, "1111111111000000"));
  ht.insert(pair <unsigned char, string> (0x93, "1111111111000001"));
  ht.insert(pair <unsigned char, string> (0x94, "1111111111000010"));
  ht.insert(pair <unsigned char, string> (0x95, "1111111111000011"));
  ht.insert(pair <unsigned char, string> (0x96, "1111111111000100"));
  ht.insert(pair <unsigned char, string> (0x97, "1111111111000101"));
  ht.insert(pair <unsigned char, string> (0x98, "1111111111000110"));
  ht.insert(pair <unsigned char, string> (0x99, "1111111111000111"));
  ht.insert(pair <unsigned char, string> (0x9A, "1111111111001000"));


  ht.insert(pair <unsigned char, string> (0xA1, "111111000"));
  ht.insert(pair <unsigned char, string> (0xA2, "1111111111001001"));
  ht.insert(pair <unsigned char, string> (0xA3, "1111111111001010"));
  ht.insert(pair <unsigned char, string> (0xA4, "1111111111001011"));
  ht.insert(pair <unsigned char, string> (0xA5, "1111111111001100"));
  ht.insert(pair <unsigned char, string> (0xA6, "1111111111001101"));
  ht.insert(pair <unsigned char, string> (0xA7, "1111111111001110"));
  ht.insert(pair <unsigned char, string> (0xA8, "1111111111001111"));
  ht.insert(pair <unsigned char, string> (0xA9, "1111111111010000"));
  ht.insert(pair <unsigned char, string> (0xAA, "1111111111010001"));

  ht.insert(pair <unsigned char, string> (0xB1, "111111001"));
  ht.insert(pair <unsigned char, string> (0xB2, "1111111111010010"));
  ht.insert(pair <unsigned char, string> (0xB3, "1111111111010011"));
  ht.insert(pair <unsigned char, string> (0xB4, "1111111111010100"));
  ht.insert(pair <unsigned char, string> (0xB5, "1111111111010101"));
  ht.insert(pair <unsigned char, string> (0xB6, "1111111111010110"));
  ht.insert(pair <unsigned char, string> (0xB7, "1111111111010111"));
  ht.insert(pair <unsigned char, string> (0xB8, "1111111111011000"));
  ht.insert(pair <unsigned char, string> (0xB9, "1111111111011001"));
  ht.insert(pair <unsigned char, string> (0xBA, "1111111111011010"));

  ht.insert(pair <unsigned char, string> (0xC1, "111111010"));
  ht.insert(pair <unsigned char, string> (0xC2, "1111111111011011"));
  ht.insert(pair <unsigned char, string> (0xC3, "1111111111011100"));
  ht.insert(pair <unsigned char, string> (0xC4, "1111111111011101"));
  ht.insert(pair <unsigned char, string> (0xC5, "1111111111011110"));
  ht.insert(pair <unsigned char, string> (0xC6, "1111111111011111"));
  ht.insert(pair <unsigned char, string> (0xC7, "1111111111100000"));
  ht.insert(pair <unsigned char, string> (0xC8, "1111111111100001"));
  ht.insert(pair <unsigned char, string> (0xC9, "1111111111100010"));
  ht.insert(pair <unsigned char, string> (0xCA, "1111111111100011"));

  ht.insert(pair <unsigned char, string> (0xD1, "11111111001"));
  ht.insert(pair <unsigned char, string> (0xD2, "1111111111100100"));
  ht.insert(pair <unsigned char, string> (0xD3, "1111111111100101"));
  ht.insert(pair <unsigned char, string> (0xD4, "1111111111100110"));
  ht.insert(pair <unsigned char, string> (0xD5, "1111111111100111"));
  ht.insert(pair <unsigned char, string> (0xD6, "1111111111101000"));
  ht.insert(pair <unsigned char, string> (0xD7, "1111111111101001"));
  ht.insert(pair <unsigned char, string> (0xD8, "1111111111101010"));
  ht.insert(pair <unsigned char, string> (0xD9, "1111111111101011"));
  ht.insert(pair <unsigned char, string> (0xDA, "1111111111101100"));

  ht.insert(pair <unsigned char, string> (0xE1, "11111111100000"));
  ht.insert(pair <unsigned char, string> (0xE2, "1111111111101101"));
  ht.insert(pair <unsigned char, string> (0xE3, "1111111111101110"));
  ht.insert(pair <unsigned char, string> (0xE4, "1111111111101111"));
  ht.insert(pair <unsigned char, string> (0xE5, "1111111111110000"));
  ht.insert(pair <unsigned char, string> (0xE6, "1111111111110001"));
  ht.insert(pair <unsigned char, string> (0xE7, "1111111111110010"));
  ht.insert(pair <unsigned char, string> (0xE8, "1111111111110011"));
  ht.insert(pair <unsigned char, string> (0xE9, "1111111111110100"));
  ht.insert(pair <unsigned char, string> (0xEA, "1111111111110101"));

  ht.insert(pair <unsigned char, string> (0xF0, "1111111010"));
  ht.insert(pair <unsigned char, string> (0xF1, "111111111000011"));
  ht.insert(pair <unsigned char, string> (0xF2, "1111111111110110"));
  ht.insert(pair <unsigned char, string> (0xF3, "1111111111110111"));
  ht.insert(pair <unsigned char, string> (0xF4, "1111111111111000"));
  ht.insert(pair <unsigned char, string> (0xF5, "1111111111111001"));
  ht.insert(pair <unsigned char, string> (0xF6, "1111111111111010"));
  ht.insert(pair <unsigned char, string> (0xF7, "1111111111111011"));
  ht.insert(pair <unsigned char, string> (0xF8, "1111111111111100"));
  ht.insert(pair <unsigned char, string> (0xF9, "1111111111111101"));
  ht.insert(pair <unsigned char, string> (0xFA, "1111111111111110"));

}

 
int get_category_AC(short n) {
  int c;
  
  if (n == 1) c = 1;
  else if (n == 2 || n == 3) c = 2;
  else if (n >= 4 && n <= 7) c = 3;
  else if (n >= 8 && n <= 15) c = 4;
  else if (n >= 16 && n <= 31) c = 5;
  else if (n >= 32 && n <= 63) c = 6;
  else if (n >= 64 && n <= 127) c = 7;
  else if (n >= 128 && n <= 255) c = 8;
  else if (n >= 256 && n <= 511) c = 9;
  else if (n >= 512 && n <= 1023) c = 10;
  else return -1;
      
  return c;
} 
 
int get_category_DC(short n) {
  int c;
  
  if (n == 0) c = 0;
  else if (n == 1) c = 1;
  else if (n == 2 || n == 3) c = 2;
  else if (n >= 4 && n <= 7) c = 3;
  else if (n >= 8 && n <= 15) c = 4;
  else if (n >= 16 && n <= 31) c = 5;
  else if (n >= 32 && n <= 63) c = 6;
  else if (n >= 64 && n <= 127) c = 7;
  else if (n >= 128 && n <= 255) c = 8;
  else if (n >= 256 && n <= 511) c = 9;
  else if (n >= 512 && n <= 1023) c = 10;
  else if (n >= 1024 && n <= 2047) c = 11;
  else return -1;
      
  return c;
}


int encode_AC (vector< pair<uint8_t,short> > bytes, int size, map<uint8_t, string> huff) {
  int i, j;
  int k = 0;
  int category;
  string encoded;
  short abs_n;
  
  vector< pair<uint8_t,short> >::iterator it;

  FILE *outputFile;
  outputFile = fopen ("test","w");
  if (outputFile == NULL) {
    return 1;
  }


  for (it = bytes.begin(); it != bytes.end(); ++it) {

    encoded = encoded + huff[it->first];
    
    // if we are dealing with non-zero values 
    if (it->first != 0x00 && it->first != 0xF0) {
      abs_n = abs(it->second);
      category = get_category_AC(abs_n);

      // the same as with DC coeficients
      //  negative value:  to_bits(value-1, category-bits)
      //  positive value:  to_bits(value, category-bits)
      if (it->second < 0) {
        encoded = encoded + to_bits(it->second-1, category);
      }
      // otherwise, the same with "difference"
      else {
        encoded = encoded + to_bits(it->second, category);
      }      
      
    }
  }

  int chars = ceil((double) encoded.length()/8);

  unsigned char *encoded_bytes;
  encoded_bytes = (unsigned char *) malloc (chars * sizeof(unsigned char));
  if (encoded_bytes == NULL) {
    return 1;
  }
    
  for (i = 0; i < chars; i++) {
    unsigned char c = 0;
    
    // shift zeros and ones to the variable
    for (j = 0; j < 8; j++) {
      c <<= 1;
      if (encoded[k] == '1')
        c += 1;
      else 
        c += 0;
      
      k++;
    }
    
    // save to array
    encoded_bytes[i] = c;
    
    fprintf(outputFile, "%c", c);
  }

  return 0;  
}


/* ----------------------------------------------------------------------- */

void huffEncode_1AC(vector< pair<uint8_t,short> > bytes, int size) {
  map<uint8_t, string> ht_1AC;
  map<uint8_t, string>::iterator it;
  
  huffmanTable_1AC(ht_1AC);
  //cout << "Huffman table (1AC) - size: " << ht_1AC.size() << endl;

  encode_AC(bytes, size, ht_1AC);
}

void huffEncode_0AC(vector< pair<uint8_t,short> > bytes, int size) {
  map<uint8_t, string> ht_0AC;
  map<uint8_t, string>::iterator it;
  
  huffmanTable_0AC(ht_0AC);
  //cout << "Huffman table (0AC) - size: " << ht_0AC.size() << endl;

  encode_AC(bytes, size, ht_0AC);
}


//-----------------------------------------------------------------------------

/* huffmanovo kodovani vypocitava diferencni kategorii hodnoty koeficientu. pak bude na vystupu hodnota ve formatu "KD", kde:
  - K je kategorie diference (pocet bitu, na kterych lze zapsat hodnotu koeficientu)
  - D je diference hodnoty koeficientu v bin. kodu (podle stanov)

 */

/*
  Prijde mi 8x8 diferenci. 
  Vezmu diferenci, najdu jeji kategorii, tu zakoduju a samotnou diferenci bin. zakodovanou hodim za to.
 */
int encode_DC(short bytes[], int size, map<short, string> huff) {
  int i, j;
  int k = 0;
  int category;
  string encoded;
  short abs_n;

  /*
  FILE *outputFile;
  outputFile = fopen ("test","w");
  if (outputFile == NULL) {
    return 1;
  }
  */
  
  // go through differences and encode them
  for (i = 0; i < size; i++) {    
    abs_n = abs(bytes[i]);
    category = get_category_DC(abs_n);
    if (category == -1) return NULL;
         
    // cout << (int) bytes[i] << ": " << huff[bytes[i]] << endl;
    encoded = encoded + huff[category];
    
    // if the difference is a negative number, add "difference-1" in binary to encoded stream
    if (bytes[i] < 0) {
      encoded = encoded + to_bits(bytes[i]-1, category);
    }
    // otherwise, the same with "difference"
    else {
      encoded = encoded + to_bits(bytes[i], category);
    }
  }

  int chars = ceil((double) encoded.length()/8);
  cout << chars << endl;

  unsigned char *encoded_bytes;
  encoded_bytes = (unsigned char *) malloc (chars * sizeof(unsigned char));
  if (encoded_bytes == NULL) {
    return 1;
  }
    
  for (i = 0; i < chars; i++) {
    unsigned char c = 0;
    
    // shift zeros and ones to the variable
    for (j = 0; j < 8; j++) {
      c <<= 1;
      if (encoded[k] == '1')
        c += 1;
      else 
        c += 0;         
      
      k++;
    }
    
    // save to array
    encoded_bytes[i] = c;
    
    //fprintf(outputFile, "%c", c);
  }

  return 0;  
}


void huffEncode_1DC(short bytes[], int size) {
  map<short, string> ht_1DC;
  map<short, string>::iterator it;
  
  huffmanTable_1DC(ht_1DC);
  //cout << "Huffman table (1DC) - size: " << ht_1DC.size() << endl;

  encode_DC(bytes, size, ht_1DC);
}

void huffEncode_0DC(short bytes[], int size) {

  map<short, string> ht_0DC;
  map<short, string>::iterator it;
  
  huffmanTable_0DC(ht_0DC);
  //cout << "Huffman table (0DC) - size: " << ht_0DC.size() << endl;
  
  encode_DC(bytes, size, ht_0DC);    
}

/*
int main(int argc, char* argv[]) {

  int n = 64; 

  // AC test
  //uint8_t bytes[7] = {0x46, 0x18, 0xF0, 0x05, 0xF0, 0xF0, 0x00};
  //short bytes[64] = {0, 0, -32, 0, 32, 18, -18};
  
  //for (int i=0; i<n; i++) {
  //  bytes[i] = 0x00 + i;
  //}
  
  vector< pair<uint8_t,short> > vec;
  vec.push_back(make_pair(0x46, 52));
  vec.push_back(make_pair(0x18, -250));
  vec.push_back(make_pair(0xf0, 0));
  vec.push_back(make_pair(0x05, 16));  
  vec.push_back(make_pair(0xf0, 0));
  vec.push_back(make_pair(0x00, 0));    
  
  huffEncode_1AC(vec, n);
    
    return 0;
}
*/
