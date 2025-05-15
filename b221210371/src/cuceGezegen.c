#include <stdlib.h>
#include <string.h>
#include "cucegezegen.h"

static double yaslanma() { return 0.01; }

Gezegen* CuceGezegenOlustur(const char* ad, int gun_saat, Zaman tarih) {
    Gezegen* g = (Gezegen*)malloc(sizeof(Gezegen));
    g->ad = strdup(ad);
    g->gun_saat = gun_saat;
    g->tur = 3;
    g->tarih = tarih;
    g->yaslanma_katsayisi = yaslanma;
    return g;
}
