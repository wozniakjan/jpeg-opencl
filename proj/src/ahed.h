/*
 * Autor: Lucie Matusova, xmatus21
 * Datum: 30. 4. 2012
 * Soubor: ahed.h
 * Komentar: rozhrani knihovny
 */

#ifndef __KKO_AHED_H__
#define __KKO_AHED_H__

#define __STDC_FORMAT_MACROS

#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <iostream>
#include <string.h>

#include <map>
#include <vector>
#include <bitset>

#define AHEDOK 0
#define AHEDFail -1
#define NYT 0xDEAD
#define BRIDGE 0xBEEF
#define ROOT 0xBABE


using namespace std;

/* Datovy typ zaznamu o (de)kodovani */
typedef struct {
    /* velikost nekodovaneho retezce */
    int64_t uncodedSize;
    /* velikost kodovaneho retezce */
    int64_t codedSize;
} tAHED;

// Node of tree

struct tNode {
    uint64_t freq;
    uint32_t number;
    tNode *parentPtr;
    tNode *lPtr;
    tNode *rPtr;
    uint16_t c;
};

struct tBuff {
    bitset < 8 > data;
    uint8_t idx;
};


/* Nazev:
 *   AHEDEncoding
 * Cinnost:
 *   Funkce koduje vstupni soubor do vystupniho souboru a porizuje zaznam o kodovani.
 * Parametry:
 *   ahed - zaznam o kodovani
 *   inputFile - vstupni soubor (nekodovany)
 *   outputFile - vystupni soubor (kodovany)
 * Navratova hodnota: 
 *    0 - kodovani probehlo v poradku
 *    -1 - pri kodovani nastala chyba
 */
int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile);


/* Nazev:
 *   AHEDDecoding
 * Cinnost:
 *   Funkce dekoduje vstupni soubor do vystupniho souboru a porizuje zaznam o dekodovani.
 * Parametry:
 *   ahed - zaznam o dekodovani
 *   inputFile - vstupni soubor (kodovany)
 *   outputFile - vystupni soubor (nekodovany)
 * Navratova hodnota: 
 *    0 - dekodovani probehlo v poradku
 *    -1 - pri dekodovani nastala chyba
 */
int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE *outputFile);

void printLog(tAHED *ahed, FILE *logFile);


#endif