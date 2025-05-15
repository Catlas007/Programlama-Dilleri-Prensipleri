#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gezegen.h"
#include "kayacgezegen.h"
#include "gazdevigezegen.h"
#include "buzdevigezegen.h"
#include "cucegezegen.h"

typedef struct GezegenNode {
    Gezegen* gezegen;
    struct GezegenNode* sonraki;
} GezegenNode;

static Gezegen* gezegenOlusturTureGore(const char* ad, int tur, int gun_saat, Zaman tarih) {
    switch (tur) {
        case 0: return KayacGezegenOlustur(ad, gun_saat, tarih);
        case 1: return GazDeviGezegenOlustur(ad, gun_saat, tarih);
        case 2: return BuzDeviGezegenOlustur(ad, gun_saat, tarih);
        case 3: return CuceGezegenOlustur(ad, gun_saat, tarih);
        default: return NULL;
    }
}

GezegenNode* gezegenleriYukle(const char* dosya_adi) {
    FILE* f = fopen(dosya_adi, "r");
    if (!f) return NULL;

    GezegenNode* bas = NULL;
    GezegenNode* son = NULL;
    char satir[256];

    while (fgets(satir, sizeof(satir), f)) {
        char* ad = strtok(satir, "#");
        int tur = atoi(strtok(NULL, "#"));
        int saat = atoi(strtok(NULL, "#"));
        char* tarihStr = strtok(NULL, "#\n");

        int gun, ay, yil;
        sscanf(tarihStr, "%d.%d.%d", &gun, &ay, &yil);
        Zaman tarih = { yil, ay, gun, 0 };

        Gezegen* g = gezegenOlusturTureGore(ad, tur, saat, tarih);
        if (!g) continue;

        GezegenNode* yeni = (GezegenNode*)malloc(sizeof(GezegenNode));
        yeni->gezegen = g;
        yeni->sonraki = NULL;

        if (!bas) bas = son = yeni;
        else { son->sonraki = yeni; son = yeni; }
    }

    fclose(f);
    return bas;
}

Gezegen* gezegenBul(GezegenNode* bas, const char* ad) {
    while (bas) {
        if (strcmp(bas->gezegen->ad, ad) == 0)
            return bas->gezegen;
        bas = bas->sonraki;
    }
    return NULL;
}

void gezegenListesiniYokEt(GezegenNode* bas) {
    while (bas) {
        GezegenNode* g = bas;
        bas = bas->sonraki;
        free(g->gezegen->ad);
        free(g->gezegen);
        free(g);
    }
}

void gezegenYazdir(const Gezegen* g) {
    printf("Gezegen: %s | Tür: %d | Gün: %d saat | Tarih: ", g->ad, g->tur, g->gun_saat);
    zamanYazdir(&g->tarih);
}
