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
                    // Varış zamanını hesapla
                    int saat_farki = bas->mesafe;
                    Zaman varis_zamani = bas->cikis_tarihi;
                    
                    // Saatleri ilerlet
                    for (int i = 0; i < saat_farki; i++) {
                        birSaatIlerle(&varis_zamani);
                    }
                    
                    zamanKopyala(&bas->varis_tarihi, &varis_zamani);
                }
            }
        }
        bas = bas->sonraki;
    }
}

void uzayAraciYazdir(const UzayAraci* a) {
    printf("%-5s | %s → %s | ", a->ad, a->cikis_gezegeni, a->varis_gezegeni);
    
    if (a->durum == IMHA) {
        printf("IMHA | Hedefe kalan saat: -- | Hedefe varacağı tarih: --\n");
    } else {
        const char* durumStr = a->durum == YOLDA ? "YOLDA" : "HEDEFTE";
        printf("%s | Hedefe kalan saat: %d | Hedefe varacağı tarih: ", durumStr, a->kalan_saat);
        zamanYazdir(&a->varis_tarihi);
    }
}