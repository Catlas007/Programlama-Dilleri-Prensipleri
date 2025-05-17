#ifndef GEZEGEN_H
#define GEZEGEN_H

#include "zaman.h"

typedef struct Gezegen {
    char* ad;
    int gun_saat;
    int tur; 
    Zaman tarih;
    double (*yaslanma_katsayisi)(void);
} Gezegen;

typedef struct GezegenNode {
    Gezegen* gezegen;
    struct GezegenNode* sonraki;
} GezegenNode;

// Fonksiyon prototipleri
GezegenNode* gezegenleriYukle(const char* dosya_adi);
Gezegen* gezegenBul(GezegenNode* bas, const char* ad);
void gezegenListesiniYokEt(GezegenNode* bas);
void gezegenYazdir(const Gezegen* g);

#endif
