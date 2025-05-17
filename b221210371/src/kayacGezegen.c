#include <stdlib.h>
#include <string.h>
#include "kayacgezegen.h"

static double yaslanma() { return 1.0; }

Gezegen* KayacGezegenOlustur(const char* ad, int gun_saat, Zaman tarih) {
    Gezegen* g = (Gezegen*)malloc(sizeof(Gezegen));
    g->ad = strdup(ad);
    g->gun_saat = gun_saat;
    g->tur = 0;
    g->tarih = tarih;
    g->yaslanma_katsayisi = yaslanma;
    return g;
}
