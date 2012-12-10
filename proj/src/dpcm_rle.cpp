
#include "dpcm_rle.h"


short  previous_block_Y[64] = {0};
short previous_block_Cb[64] = {0};
short previous_block_Cr[64] = {0};

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
  
 Zjisti se diferencni kategorie a za ni se prilepi zakodovania hodnota koeficientu (dvojkovy doplnek).
 */
string to_bits(int num, unsigned int nBit){
  string b;
    
  unsigned int temp = 1 << (nBit-1), i;

  for(i = 0; i < nBit; ++i) {
    //printf("%d ", ((num&temp)?1:0) );

    b = b + ((num & temp) ? "1":"0");
    temp = temp >> 1;
  }
    
  return b;
}  

/*
  Vstupem je pole DC koeficientu (11b cisel), blok 8x8 hodnot. Pocita se diference mezi aktualnim a predchozim blokem. Pokud se jedna o prvni blok, predchozi blok obsahuje same nuly.
  Kazda slozka (Y, Cb, Cr) se pocita zvlast zavolanim prislusne funkce (dc_Y, dc_Cb, dc_Cr).
  Vystupem jsou diference, ktere jdou do Huffmanova kodovani.
 */
short *dpcm_DC(short bytes[], int size, short previous_block[]) {
  
  if (size != 64) return NULL; // should not happen

  short *outDC;
  outDC = (short *) malloc (size * sizeof(short));
  if (outDC == NULL) return NULL;
        
  for (int i=0; i<size; i++) {     
    outDC[i] = previous_block[i] - bytes[i];  // compute the difference
    previous_block[i] = bytes[i];   // save the block to use it for the difference with the next block
    // cout << outDC[i] << endl;      
  }
  
  return outDC;   
}

short *dc_Y(short bytes[], int size) {
  return dpcm_DC(bytes, size, previous_block_Y);
}

short *dc_Cb(short bytes[], int size) {
  return dpcm_DC(bytes, size, previous_block_Cb);
}

short *dc_Cr(short bytes[], int size) {
  return dpcm_DC(bytes, size, previous_block_Cr);
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
  Do rle_AC vstupuje blok 8x8 hodnot. Na vystupu byva kratsi pole 8b hodnot (v nekterych pripadech ale muze byt i delsi).
  Z kazdeho AC koeficientu v bloku udela 8bitovou hodnotu "RRRRSSSS".
   - SSSS jsou 4 bity znacici, podobne jako u DC, kategorie hodnoty (pocet bitu, na jakem se da hodnota vyjadrit)
   - RRRR (opet 4b) je pocet nulovych koeficientu, ktere koeficientu predchazely. Je-li jich vice nez 15, pouzije se pro zakodovani 8bitova hodnota 0xF0, ktera znaci 15 za sebou jdoucich nulovych koeficientu. Jsme-li na konci a zbyvaji jiz same nuly, pouzije se kod 0x00 (End of Block).
  
  Vraci vektor dvojic <uint*_t, short>. Dvojice obsahuji zakodovani rlc a hodnotu koeficientu z bloku. Vektor postupuje dale do Huffmanova kodovani. 
 */
// uint8_t *rlc_AC(short bytes[], int *size) {
vector< pair<uint8_t,short> > rlc_AC(short bytes[], int *size) {
  int i, j;
  int category;
  int zero = 0;

  vector< pair<uint8_t,short> > out;

  int arr_size = *size;
  
  string s1, s2, s;

  //if (arr_size != 64) return NULL;
  
  j = 0;
  for (i=0; i<arr_size; i++) {
    if (bytes[i] == 0) {
      zero++;
      if (zero>15) {
        zero = 0;
        s = to_bits(240, 8);  // 8 bits result
        //out.push_back(makeByte(s));   // put zeros and ones to 8bit variable
        out.push_back(make_pair(makeByte(s), bytes[i]));
        j++;
      }
      else if ((i+1) == arr_size) {
        if (zero>0) {
          // out.push_back(makeByte("00000000"));  // EOB (End of Block)
          out.push_back(make_pair(makeByte(s), bytes[i]));  // put zeros and ones to 8bit variable
        }
        break;
      } 
      
      continue;
    }
   
    category = get_category_AC(abs(bytes[i]));
    // if (category == -1) return NULL;

    s1 = to_bits(zero, 4);      // 4 bits - first half of result
    s2 = to_bits(category, 4);  // 4 bits - other half of result
    s = s1 + s2;

    out.push_back(make_pair(makeByte(s), bytes[i]));  // put zeros and ones to 8bit variable
     
    j++;
    zero = 0;
  }
  
  /*
  *size = out.size();
  uint8_t *outAC;
  outAC = (uint8_t *) malloc (*size * sizeof(uint8_t));
  if (outAC == NULL) return NULL;
  
  for (i = 0; i<*size; i++) {
    outAC[i] = out[i];
  }
  */

  return out;
  
}

/*
int main(int argc, char* argv[]) {
  int i;
  int n = 64;

  short bytes[64] = {0, 0, 0, 0, 52, 0, -250, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0};

  //short *outDC = dc_Y(bytes, n);
  
  //for (i=0; i<n; i++) {
  //  cout << (int) outDC[i] << " ";
  //}
  //cout << endl;
  
  
  vector< pair<uint8_t,short> > ac = rlc_AC(bytes, &n);
  vector< pair<uint8_t,short> >::iterator it;
  
  for(it = ac.begin(); it != ac.end(); ++it) {
    cout << hex << (int) it->first << " - " << dec << it->second << endl;
  }
  
  return 0;
}
*/
