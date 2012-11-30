/*
 * Autor: Lucie Matusova, xmatus21
 * Datum: 30. 4. 2012
 * Soubor: ahed.c
 * Komentar: implementace knihovny
 * 
 */

#include "ahed.h"

/*
 * Print statistics to log file.
 */
void printLog(tAHED *ahed, FILE *logFile) {
        fprintf(logFile, "login = xmatus21\n");
        fprintf(logFile, "uncodedSize = %" PRIu64 "\n", ahed->uncodedSize);
        fprintf(logFile, "codedSize = %" PRIu64 "\n", ahed->codedSize);
}

/*
 * Deallocate space in memory occupied by tree.
 */
void freeTree(tNode *root){
    while(1){       
        if(root == NULL){
            break;
        }
        else if(root->lPtr != NULL){
            freeTree(root->lPtr);
            root->lPtr = NULL;
        }
        else if(root->rPtr != NULL){
            freeTree(root->rPtr);
            root->rPtr = NULL;
        }
        else{ 
            free(root);
            return;
        }
    }
}

/*
 * Swap the given nodes in tree.
 */

void swap_nodes(tNode *node1, tNode *node2, map<uint32_t, struct tNode *> &numTable) {
    tNode *pom1 = node1->parentPtr;
    tNode *pom2 = node2->parentPtr;
    
    if (pom1 == NULL || pom2 == NULL){
        fprintf(stderr, "Swapping error - null node.");
        exit(AHEDFail);
    }
    tNode *temp;
    int num;
    
    // nodes have the same parent
    //  swap childrens parents
    if (pom1 == pom2) {        
        temp = pom1->lPtr;        
        pom1->lPtr = pom1->rPtr;
        pom1->rPtr = temp;
        
        num = node1->number;
        node1->number = node2->number;
        node2->number = num;
    }
    else {
    // nodes do not have the same parent
    //  change childrens pointers and parents

        if (pom1->rPtr == node1) {
            pom1->rPtr = node2;
        }
        else {
            pom1->lPtr = node2;
        }
        
        if (pom2->rPtr == node2) {
            pom2->rPtr = node1;
        }
        else {
            pom2->lPtr = node1;
        }         
        
        num = node1->number; 
        node1->number = node2->number;
        node2->number = num;
    }   
    
    
        node1->parentPtr = pom2;
        node2->parentPtr = pom1;
        
    
    numTable[node1->number] = node1;
    numTable[node2->number] = node2;
    
}

/*
 * Update Huffman's tree according to sibling property.
 * 
 * http://www.stringology.org/DataCompression/fgk/index_cs.html
 */
void updateTree(map<uint32_t, struct tNode *> &numTable, 
                tNode *node                              ) {

    uint32_t i;
    tNode *pomNode;
    uint32_t highestNum = node->number;

    map<uint32_t, struct tNode *>::iterator it;
    
    while (node->c != ROOT) {
        // existuje uzel U1 se stejnym ohodnocenim (frekvence) a vyssim poradim (number)
        // dokud neni koren
        for (i = node->number + 1; i < 1000; i++) {
            
            it = numTable.find(i);
            if (it == numTable.end()) {
                return;
            }
            pomNode = it->second;

            // if the frequencies are equal
            if ((pomNode->freq == node->freq)) {
                if ((pomNode != node->parentPtr)) {
                        highestNum = i;
                } else continue;
            } else break;
        }
            
        if (highestNum > node->number) {
            swap_nodes(numTable[highestNum], node, numTable);
        }

        node->freq++;
        node = node->parentPtr;
    }

    node->freq++;

}

/*
 * Print contents of buffer as 8bit chars and erase the printed bits afterwards.
 */

void printBuffer(vector<uint8_t> &buffer, tAHED *ahed, FILE *outputFile) {
    vector<uint8_t>::iterator itV;
    
    uint8_t z;
    int i, j;
    int numOfChars = buffer.size() / 8;

    for (i = 0; i < numOfChars; i++) {
        itV = buffer.begin();

        z = 0;

        // from the end of the buffer to the beginning
        for (j = 0; j < 8; j++) {
            z <<= 1;
            if (*itV == 1)
                z += 1;
            else 
                z += 0;
            
            itV++;
        }

        fprintf(outputFile, "%c", z);
        ahed->codedSize += 1;

        buffer.erase(buffer.begin(), buffer.begin() + 8);
    }
}      

/*
 * Put the NYT code into the buffer.
 */
void getNytCode(vector<uint8_t> &buffer, tNode *nyt) {
    vector<uint8_t> charBuffer;        
    
    // nyt
    tNode *itUp = nyt->parentPtr;
    tNode *itPrev = nyt;
   
    while (itUp != NULL) {                
       // it was on the right
       if (itUp->rPtr == itPrev) {
          //cout << "1";
          charBuffer.push_back(1);
       }// it was on the left
       else if (itUp->lPtr == itPrev) {
          //cout << "0";
           charBuffer.push_back(0);
       } else {

       }

       itUp = itUp->parentPtr;
       itPrev = itPrev->parentPtr;
    }
    //cout << endl;

    buffer.insert(buffer.end(), charBuffer.rbegin(), charBuffer.rend());
   
}

/*
 * Add very first infcoming char to the tree.
 */

int addFirstChar(uint32_t  c, map<uint32_t, struct tNode *> &charTable, map<uint32_t, struct tNode *> &numTable, tNode *nyt, tNode *root, FILE *outputFile) {
    
    tNode *newNodePtr = (tNode *) malloc(sizeof (struct tNode)); // to be saved to map
    if (newNodePtr == NULL) {
        fprintf(stderr, "Malloc error - newNodePtr.");
        return AHEDFail;
    } else {

        nyt->parentPtr = root;
        nyt->number = root->number - 2;
        numTable[nyt->number] = nyt;

        root->lPtr = nyt;
        root->rPtr = newNodePtr;

        newNodePtr->freq = 1;
        newNodePtr->parentPtr = root;
        newNodePtr->lPtr = NULL;
        newNodePtr->rPtr = NULL;
        newNodePtr->number = root->number - 1;
        newNodePtr->c = c;
        numTable[newNodePtr->number] = newNodePtr;
        fputc((uint8_t) c, outputFile);

    }
        // insert new pair of char nad its node to the char table
        charTable[c] = newNodePtr;
    
}

/*
 * Adds a char to the tree and calls tree update.
 */
int addChar(uint32_t c, map<uint32_t, struct tNode *> &charTable, map<uint32_t, struct tNode *> &numTable, tNode *nyt, tNode *root) {
    
    tNode *newNodePtr = (tNode *) malloc(sizeof (struct tNode)); // to be saved to map
    if (newNodePtr == NULL) {
        fprintf(stderr, "Malloc error - newNodePtr.");
        return AHEDFail;
    } else {
        tNode *auxNodePtr = (tNode *) malloc(sizeof (struct tNode));

        if (auxNodePtr == NULL) {
            fprintf(stderr, "Malloc error - helpNodePtr.");
            return AHEDFail;
        } else {
            
            newNodePtr->lPtr = NULL;
            newNodePtr->rPtr = NULL;
            newNodePtr->freq = 1;
            newNodePtr->parentPtr = auxNodePtr;
            newNodePtr->c = c;

            auxNodePtr->lPtr = nyt;
            auxNodePtr->rPtr = newNodePtr;
            auxNodePtr->freq = 0;
            auxNodePtr->parentPtr = nyt->parentPtr;
            auxNodePtr->number = nyt->number;
            auxNodePtr->c = BRIDGE;

            numTable[auxNodePtr->number] = auxNodePtr;

            newNodePtr->number = auxNodePtr->number - 1;

            numTable[newNodePtr->number] = newNodePtr;

            nyt->parentPtr->lPtr = auxNodePtr;
            nyt->parentPtr = auxNodePtr;
            nyt->number = auxNodePtr->number - 2;

            numTable[nyt->number] = nyt;
            // poruseni sourozenecke vlastnosti?
            // aktualizace stromu
            // volam aktualizaci na ten helpnode
            updateTree(numTable, auxNodePtr);
        }
        // insert new pair of char nad its node to the char table
        charTable[c] = newNodePtr;
    }
}

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

int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile) {
    map<uint32_t, struct tNode *> charTable; // 8b chars + one special escape char
    map<uint32_t, struct tNode *>::iterator it;
    
    map<uint32_t, struct tNode *> numTable; // 8b chars + one special escape char
    
    vector<uint8_t> buffer;
    
    vector<uint8_t> charBuffer;

    tNode *root = (tNode *) malloc(sizeof (struct tNode));
    if (root == NULL) {
        fprintf(stderr, "Malloc error - root.");
        return AHEDFail;
    } else {
        root->freq = 1;
        root->number = 1000;
        root->lPtr = NULL;
        root->rPtr = NULL;
        root->parentPtr = NULL;
        root->c    = ROOT;
        
        numTable[1000] = root;

        tNode *nyt = (tNode *) malloc(sizeof (struct tNode)); // not yet transmitted
        if (nyt == NULL) {
            fprintf(stderr, "Malloc error - nyt.");
            return AHEDFail;
        } else {
            nyt->freq      = 0; // frquence of NYT is still 0.000!
            nyt->lPtr      = NULL;
            nyt->rPtr      = NULL;
            nyt->parentPtr = NULL;
            nyt->number    = 0;
            nyt->c         = NYT;
            
            uint32_t  c;            
            bool firstChar = true;
            uint8_t mask = 0x80;
            int b, k;            
            // reading 8b chars (EOF is 32b)
            while ((c = fgetc(inputFile)) != EOF) { 
                ahed->uncodedSize += 1;
                
                if ((it = charTable.find(c)) != charTable.end()) {
                    tNode *itUp;
                    tNode *itPrev = it->second;
                    
                    charBuffer.clear();
                    
                    for (itUp = itPrev->parentPtr; itUp != NULL; itUp = itUp->parentPtr, itPrev = itPrev->parentPtr) {
                        // it was on the right
                        if (itUp->rPtr == itPrev) {
                            //cout << "1";
                            charBuffer.push_back(1);
                        }// it was on the left
                        else if (itUp->lPtr == itPrev) {
                            //cout << "0";
                            charBuffer.push_back(0);
                        } else {

                        }
                    }

                    buffer.insert(buffer.end(), charBuffer.rbegin(), charBuffer.rend());

                    updateTree(numTable, it->second);

                } else {
                    // find NYT encoding
                    getNytCode(buffer, nyt);

                    // very first char came                        
                    if (firstChar) {
                        addFirstChar(c, charTable, numTable, nyt, root, outputFile);
                        ahed->codedSize += 1;
                        firstChar = false;
                    } else {
                        // push the char into the buffer
                        // char c - 32b
                        for (k = 0; k < 8; ++k) {
                            b = (((uint8_t) c << k) & mask) ? 1 : 0;
                            buffer.push_back(b);
                        }
                        addChar(c, charTable, numTable, nyt, root);
                    }
                }

                printBuffer(buffer, ahed, outputFile);
            }

            getNytCode(buffer, nyt);

            int addZeros = 8 - (buffer.size() % 8);
            for (int g = 1; g <= addZeros; g++) {
                buffer.push_back(0);
            }

            printBuffer(buffer, ahed, outputFile);

        }
    }
    
    freeTree(root);
    return AHEDOK;
}


/******************************************************************/

/******************************************************************/

/*
 * Go through bits of given char and search the tree to find its code.
 *  NYT node is returned in case of following uncoded symbol on input
 *  BRIDGE node is returned when it is necesary to read more bits 
 *   to complete searching in the tree
 *  node with symbol itself is returned when the searching was successful
 */
tNode * getCharFromTree(tBuff &buff, tNode *startNode) {
    tNode *seekerNode = startNode;

    while (buff.idx < 8) {
        if (buff.data[buff.idx] == 1) {
            seekerNode = seekerNode->rPtr;
        } else {
            seekerNode = seekerNode->lPtr;
        }

        buff.idx--;

        // should not happen
        if (seekerNode == NULL) {
            return NULL;
        }

        if (seekerNode->c == NYT) {
            return seekerNode;
        } else if (seekerNode->c != BRIDGE) {
            return seekerNode;
        }
    }

    return seekerNode;

}

/*
 * Reads buffer and puts bits into the given auxiliary buffer.
 * 
 * If only a part of char is stored in auxBuff from the previous reading,
 *  its reading will be finished and saved in buffer as a whole char.
 * 
 * Returns true if there is a need for more reading and filling auxBuff;
 *  false otherwise.
 */
bool getCharFromBitset(tBuff &buffer, tBuff &auxBuff, tAHED *ahed, FILE* outputFile) {

    while (buffer.idx < 8) {
        if (auxBuff.idx > 7) {
            fprintf(outputFile, "%c", ((uint8_t) auxBuff.data.to_ulong()));

            ahed->uncodedSize += 1;
            auxBuff.idx = 7;

            return false; // keepOnReading = false;
        }

        auxBuff.data[auxBuff.idx] = buffer.data[buffer.idx];

        auxBuff.idx--;
        buffer.idx--;
    }

    return true; // keepOnReading = true
}

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
int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE * outputFile) {
    map<uint32_t, struct tNode *> charTable;
    map<uint32_t, struct tNode *>::iterator it;

    map<uint32_t, struct tNode *> numTable;

    bool firstChar = true;

    // Root
    tNode *root = (tNode *) malloc(sizeof (struct tNode));
    if (root == NULL) {
        fprintf(stderr, "Malloc error - root.");
        return AHEDFail;
    } else {
        root->freq = 1;
        root->number = 1000;
        root->lPtr = NULL;
        root->rPtr = NULL;
        root->parentPtr = NULL;
        root->c = ROOT;

        numTable[1000] = root;

        tNode *nyt = (tNode *) malloc(sizeof (struct tNode)); // not yet transmitted
        if (nyt == NULL) {
            fprintf(stderr, "Malloc error - nyt.");
            return AHEDFail;
        } else {
            nyt->freq = 0; // frequence of NYT is still 0.000!
            nyt->lPtr = NULL;
            nyt->rPtr = NULL;
            nyt->parentPtr = NULL;
            nyt->number = 0;
            nyt->c = NYT;

            /******************************************/

            uint32_t c;
            bool keepOnReading = false;
            tNode *node;
            node = root;

            tBuff buffer;
            tBuff nextChar;
            nextChar.idx = 7;

            // reading 8b chars
            while ((c = fgetc(inputFile)) != EOF) {
                ahed->codedSize += 1;
                // very first char - print to file and make new node for it
                if (firstChar) {
                    addFirstChar(c, charTable, numTable, nyt, root, outputFile);
                    ahed->uncodedSize += 1;
                    firstChar = false;
                } else {
                    buffer.data = c;
                    buffer.idx = 7;
                    
                    // when there is something to read
                    while (buffer.idx < 8) {

                        // there is something in the buffer but it is not complete
                        if (keepOnReading) {
                            // read the rest of the char to nextChar buffer
                            keepOnReading = getCharFromBitset(buffer, nextChar, ahed, outputFile);
                            
                            // if the has been read
                            if (keepOnReading == false) {
                                // add it to the tree
                                addChar(nextChar.data.to_ulong(), charTable, numTable, nyt, root);
                                //  and do not forget to set node to root again so we can search from it
                                node = root;
                            }

                        } else {
                            node = getCharFromTree(buffer, node);
                            if (node->c == BRIDGE) {

                            } else if (node->c == NYT) {
                                keepOnReading = true;
                            } else {
                                fprintf(outputFile, "%c", node->c);
                                ahed->uncodedSize += 1;

                                updateTree(numTable, node);
                                node = root;
                            }

                        }
                    }
                }
            }
        }
        
        freeTree(root);
        
        return AHEDOK;
    }
}
