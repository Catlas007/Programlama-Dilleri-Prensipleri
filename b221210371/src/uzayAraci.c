#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uzayaraci.h"

UzayAraci* uzayAraciOlustur(const char* ad, const char* c, const char* v, Zaman t, int mesafe) {
    UzayAraci* a = (UzayAraci*)malloc(sizeof(UzayAraci));
    a->ad = strdup(ad);
    a->cikis_gezegeni = strdup(c);
    a->varis_gezegeni = strdup(v);
    a->cikis_tarihi = t;
    a->mesafe = mesafe;
    a->kalan_saat = mesafe;
    a->durum = YOLDA; // başlangıçta aktif varsayılır, simülasyon güncelleyecek
    a->varis_tarihi = t; // geçici, simülasyonda hesaplanacak
    a->sonraki = NULL;
    return a;
}

UzayAraci* uzayAraclariniYukle(const char* dosya_adi) {
    FILE* f = fopen(dosya_adi, "r");
    if (!f) return NULL;

    UzayAraci* bas = NULL;
    UzayAraci* son = NULL;
    char satir[256];

    while (fgets(satir, sizeof(satir), f)) {
        char* ad = strtok(satir, "#");
        char* c = strtok(NULL, "#");
        char* v = strtok(NULL, "#");
        char* tarihStr = strtok(NULL, "#");
        char* mesafeStr = strtok(NULL, "#\n");

        int gun, ay, yil;
        sscanf(tarihStr, "%d.%d.%d", &gun, &ay, &yil);
        Zaman t = { yil, ay, gun, 0 };

        UzayAraci* yeni = uzayAraciOlustur(ad, c, v, t, atoi(mesafeStr));
        if (!bas) bas = son = yeni;
        else { son->sonraki = yeni; son = yeni; }
    }

    fclose(f);
    return bas;
}

void uzayAraciListesiniYokEt(UzayAraci* bas) {
    while (bas) {
        UzayAraci* g = bas;
        bas = bas->sonraki;
        free(g->ad);
        free(g->cikis_gezegeni);
        free(g->varis_gezegeni);
        free(g);
    }
}

void uzayAraclariniGuncelle(UzayAraci* bas, const Zaman* gezegenTarih) {
    while (bas) {
        if (bas->durum == YOLDA) {
            if (zamanKiyasla(&bas->cikis_tarihi, gezegenTarih) <= 0) {
                bas->kalan_saat--;
                if (bas->kalan_saat <= 0) {
                    bas->durum = HEDEFTE;
                    // Vardığı tarih burada hesaplanmalı: örn. gezegen saatine göre
                }
            }
        }
        bas = bas->sonraki;
    }
}

void uzayAraciYazdir(const UzayAraci* a) {
    printf("Arac: %s | %s → %s | Çıkış: ", a->ad, a->cikis_gezegeni, a->varis_gezegeni);
    zamanYazdir(&a->cikis_tarihi);
    if (a->durum == IMHA)
        printf("Durum: IMHA | Kalan Saat: -- | Varış: --\n");
    else {
        printf("Durum: %s | Kalan Saat: %d | Varış: ", 
               a->durum == YOLDA ? "YOLDA" : "HEDEFTE", a->kalan_saat);
        zamanYazdir(&a->varis_tarihi);
    }
}
