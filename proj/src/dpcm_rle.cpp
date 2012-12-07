
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>      /* printf */
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */
#include <stdbool.h>
#include <vector>
#include <stdint.h>

using namespace std;

/* 
 (Z roota) Do Huffmanova kodovani jdou DC koeficienty nasledovne:
 Koeficient je napr 32: 
  - pocet bitu, na kt. 32 zakoduju je 6 - kategorie diference je 6
  - normalne binarne cislo 32 (na sesti bitech; podle cisla kategorie) - 100000
  - do Huffmana prijde nasledujici "6 32"
 Koeficient je napr -63:
  - pocet bitu, na kt. -63 zakoduju je 6 - kategorie diference je 6
  - binarne cislo -63 (na sesti bitech; podle cisla kategorie) - 000000
  - do Huffmana prijde nasledujici "6 -63"
  
 Kodovani hodnoty koeficientu bude probihat az v Huffmanove kodovani spolu s kodovanim cisla diferencni kategorie.
 */
string huff_bits(int num, unsigned int nBit){
    string b;
    bool negative = false;
    
    if (num < 0) {
      cout << "zaporne!" << endl;
      num = abs(num);
      negative = true;
    }
    
    unsigned int temp = 1 << (nBit-1), i;

    for(i = 0; i < nBit; ++i) {
        //printf("%d ", ((num&temp)?1:0) );
        if (negative) {
          b = b + ((num & temp) ? "0":"1");
        }
        else {
          b = b + ((num & temp) ? "1":"0");
        }
        
        temp = temp >> 1;
    }
    
    return b;
}  

/*
  Vstupem je pole DC koeficientu (11b cisel). 
  Je treba pocitat diferenci, proto vstupuji dva bloky (128 hodnot). Pokud se jedna o prvni blok, vstupuje pouze prvnich 64 hodnot.
  Pred Huffmanovym kodovanim se zpracuji tak, ze misto hodnoty koeficientu bude na vystupu hodnota ve formatu formatu "K D", kde:
  - K je kategorie diference (pocet bitu, na kterych lze zapsat hodnotu koeficientu)
  - D je diference hodnoty koeficientu - odecteme aktualni hodnotu od hodnoty v predchozim bloku (pri pocitani prvniho bloku obsahuje predchozi blok same nuly)
  
  Vystupem je dvakrat tolik hodnot typu short, ktere jde do Huffmanova kodovani.
 */
void dpcm_DC(short bytes[], int size, short out[]) {
  int i, j;
  int category;
  
  // first block
  if (size == 64) {
    unsigned char a = 0;
    //cout << "first block" << endl;
    
    for (int i=0; i<128; i++) {     
      out[i+1] = a - bytes[i];
      double abs_n = abs((double) out[i+1]);
      
      if (abs_n == 0) category = 0;
      else if (abs_n == 1) category = 1;
      else category = ceil(log2(abs_n));
     
      out[i] = category;
      cout << "[" << out[i] << "," << out[i+1] << "] ";      
      
      i++;
    }    
  }
  else if (size == 128) {
    //cout << "n-th block" << endl;

    for (i=64, j=0; i<128; i++, j++) {
      out[j+1] = bytes[j] - bytes[i];
      double abs_n = abs((double) bytes[j] - bytes[i]);
      
      if (abs_n == 0) category = 0;
      else if (abs_n == 1) category = 1;
      else category = ceil(log2(abs_n));

      out[j] = category;      
      //cout << "[" << out[j] << "," << out[j+1] << "] ";      

      j++;
    }
  }
   
}

uint8_t makeByte (string s) {
  uint8_t byte;
  int j, k;

  for (j=0, k=0; j < 8; j++, k++) {
      byte <<= 1;
    if (s[k] == '1')
      byte += 1;
    else
      byte += 0;
  }

  return byte;
}

/*
 Do rle_AC vstupuje blok 8x8 hodnot. Na vystupu byva kratsi pole 8b hodnot (v urcitych pripadech muze byt i delsi).
 Z kazdeho AC koeficientu v bloku udela 8bitovou hodnotu "RRRRSSSS".
  - SSSS jsou 4 bity znacici, podobne jako u DC, kategorie hodnoty (pocet bitu, na jakem se da hodnota vyjadrit)
  - RRRR (opet 4b) je pocet nulovych koeficientu, ktere koeficientu predchazely. Je-li jich vice nez 15, pouzije se pro zakodovani 8bitova hodnota 0xF0, ktera znaci 15 za sebou jdoucich nulovych koeficientu. Jsme-li na konci a zbyvaji jiz same nuly, pouzije se kod 0x00 (End of Block).
  
  Vraci pole 8b hodnot, ktere postupuje dale do bloku s Huffmanovym kodovanim. 
 */
uint8_t *rle_AC(short bytes[], int *size) {
  int i, j;
  int category;
  int zero = 0;
  vector <uint8_t> out;
  int arr_size = *size;
  
  string s1, s2, s;

  // we work with 8x8 blocks
  //if (arr_size != 64) return NULL;
  
  j = 0;
  for (i=0; i<arr_size; i++) {
    if (bytes[i] == 0) {
      zero++;
      if (zero>15) {
        zero = 0;
        s = huff_bits(240, 8);  // 8 bits result
        out.push_back(makeByte(s));   // put zeros and ones to 8bit variable
        j++;
      }
      else if ((i+1) == arr_size) {
        if (zero>0) {
          out.push_back(makeByte("00000000"));  // EOB (End of Block)
        }
        break;
      } 
      
      continue;
    }
    
    category = ceil(log2(abs(bytes[i])));

    s1 = huff_bits(zero, 4);      // 4 bits - other half of result
    s2 = huff_bits(category, 4);  // 4 bits - first half of result
    s = s1 + s2;
    // cout << " ~" << s << "~ " << endl;

    out.push_back(makeByte(s));   // put zeros and ones to 8bit variable
    
    j++;
    zero = 0;
  }
  
  *size = out.size();
  uint8_t *outAC;
  outAC = (uint8_t *) malloc (*size * sizeof(uint8_t));
  
  for (i = 0; i<*size; i++) {
    outAC[i] = out[i];
  }

  return outAC;
  
}

/*
int main(int argc, char* argv[]) {
  int i;
  // size - in dcmp_DC - 64 means first block, 128 n-th block
  int n = 64;
  // test - values for rle_AC
  short bytes[64] = {0, 0, 0, 0, 52, 0, -250, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0}; // 0..65535

  // short outDC[128];
  // dpcm_DC(bytes, n, outDC);  // bytes could be array filled with random values
  
  // rle_AC will create an array of appropriate size
  uint8_t *outAC = rle_AC(bytes, &n);
  for (i=0; i<n; i++) {
    cout << (int) outAC[i] << " ";
  }
  cout << endl;
  
  return 0;
} */
