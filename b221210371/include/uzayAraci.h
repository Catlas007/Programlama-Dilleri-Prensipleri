
#ifndef UZAYARACI_H
#define UZAYARACI_H

#include "zaman.h"

// Enum'u daha açık tanımla
typedef enum { 
    YOLDA = 0, 
    IMHA = 1, 
    HEDEFTE = 2, 
    BEKLEMEDE = 3 
} Durum;

typedef struct UzayAraci {
    char* ad;
    char* cikis_gezegeni;
    char* varis_gezegeni;
    Zaman cikis_tarihi;
    int mesafe;
    int kalan_saat;
    Zaman varis_tarihi;
    Durum durum;
    struct UzayAraci* sonraki;
} UzayAraci;

UzayAraci* uzayAraciOlustur(const char* ad, const char* c, const char* v, Zaman t, int mesafe);
UzayAraci* uzayAraclariniYukle(const char* dosya_adi);
void uzayAraciListesiniYokEt(UzayAraci* bas);
void uzayAraclariniGuncelle(UzayAraci* bas, const Zaman* gezegenTarih);
void uzayAraciYazdir(const UzayAraci* a);

#endif